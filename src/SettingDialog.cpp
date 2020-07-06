#include "SettingDialog.h"
#include <QFileDialog>

#include "ConfigHelper.h"

CSettingDialog::CSettingDialog(QWidget *parent)
    : QDialog(parent), Ui_SettingDlg() {
    setupUi(this);
    QObject::connect(btnLoadPath, &QPushButton::clicked, this, &CSettingDialog::handleSelectLoadBtnClicked);
    QObject::connect(btnRootPath, &QPushButton::clicked, this, &CSettingDialog::handleSelectRootBtnClicked);
    QObject::connect(btnScript, &QPushButton::clicked, this, &CSettingDialog::handleSelectScriptBtnClicked);
    QObject::connect(editRootPath, SIGNAL(textChanged(const QString&)), this, SLOT(handlePathChanged(const QString&)));
    QObject::connect(editLoadPath, SIGNAL(textChanged(const QString&)), this, SLOT(handlePathChanged(const QString&)));
    QObject::connect(editScriptPath, SIGNAL(textChanged(const QString&)), this, SLOT(handlePathChanged(const QString&)));
}

CSettingDialog::~CSettingDialog()
{
}

void CSettingDialog::init() {
    editRootPath->setText(ConfigHelper::singleton().getProtoRootPath());
    editLoadPath->setText(ConfigHelper::singleton().getProtoFilesLoadPath());
    editScriptPath->setText(ConfigHelper::singleton().getLuaScriptPath());

    checkAvailable();
}

void CSettingDialog::handleSelectRootBtnClicked() {
    auto* fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Select Proto Root Path");
    //fileDialog->setDirectory(QDir::currentPath());
    //fileDialog->setOption(QFileDialog::ShowDirsOnly);
    fileDialog->setFileMode(QFileDialog::DirectoryOnly);
    fileDialog->setViewMode(QFileDialog::Detail);
    if (fileDialog->exec()) {
        editRootPath->setText(fileDialog->selectedFiles()[0]);
        ConfigHelper::singleton().saveProtoRootPath(fileDialog->selectedFiles()[0]);
    }

    if (!editRootPath->text().isEmpty()
        && !editLoadPath->text().isEmpty()) {
        okButton->setEnabled(true);
    }
}

void CSettingDialog::handleSelectLoadBtnClicked() {
    auto* fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Select Proto Load Path");
    //fileDialog->setDirectory(QDir::currentPath());
    //fileDialog->setOption(QFileDialog::ShowDirsOnly);
    fileDialog->setFileMode(QFileDialog::DirectoryOnly);
    fileDialog->setViewMode(QFileDialog::Detail);
    if (fileDialog->exec()) {
        editLoadPath->setText(fileDialog->selectedFiles()[0]);
        ConfigHelper::singleton().saveProtoFilesLoadPath(fileDialog->selectedFiles()[0]);
    }
}

void CSettingDialog::handleSelectScriptBtnClicked() {
    auto* fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Select Lua Script Path");
    //fileDialog->setDirectory(QDir::currentPath());
    //fileDialog->setOption(QFileDialog::ShowDirsOnly);
    fileDialog->setFileMode(QFileDialog::AnyFile);
    fileDialog->setViewMode(QFileDialog::Detail);
    if (fileDialog->exec()) {
        editScriptPath->setText(fileDialog->selectedFiles()[0]);
        ConfigHelper::singleton().saveLuaScriptPath(fileDialog->selectedFiles()[0]);
    }
}

void CSettingDialog::handlePathChanged(const QString& newText) {
    if (!editRootPath->text().isEmpty()
        && !editLoadPath->text().isEmpty()
        && !editScriptPath->text().isEmpty()) {
        ConfigHelper::singleton().saveProtoRootPath(editRootPath->text());
        ConfigHelper::singleton().saveProtoFilesLoadPath(editLoadPath->text());
        ConfigHelper::singleton().saveLuaScriptPath(editScriptPath->text());
        okButton->setEnabled(true);
    } else {
        okButton->setEnabled(false);
    }
}

void CSettingDialog::checkAvailable() {
    if (!editRootPath->text().isEmpty()
        && !editLoadPath->text().isEmpty()
        && !editScriptPath->text().isEmpty()) {
        okButton->setEnabled(true);
    } else {
        okButton->setEnabled(false);
    }
}