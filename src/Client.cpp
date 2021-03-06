#include "Client.h"
#include "NetManager.h"
#include "ProtoManager.h"
#include "LuaScriptSystem.h"

#include <google/protobuf/util/json_util.h>

CClient::CClient() {
    m_sendMsgDataBuf.reserve(MAX_BUFF_SIZE);
}

CClient::~CClient() {
    m_sendMsgDataBuf.clear();
    for (auto& pair : m_msgs) {
        delete pair.second;
    }
    m_msgs.clear();
}

void CClient::connectTo(std::string ip, Port port, std::function<void(bool)> callback) {
    m_connectCallback = callback;

    NetManager::instance().connect(ip.c_str(), port, 131073,131073);
}

bool CClient::sendMsg(const google::protobuf::Message& message) {
    size_t messageSize = message.ByteSizeLong();
    if (messageSize > m_sendMsgDataBuf.capacity()) {
        m_sendMsgDataBuf.reserve(messageSize);
    }

    if (!message.SerializeToArray(m_sendMsgDataBuf.data(), m_sendMsgDataBuf.capacity())) {
        return false;
    }

    return NetManager::instance().sendProtoMsg(m_socketID
                                                , message.GetTypeName().c_str()
                                                , m_sendMsgDataBuf.data()
                                                , message.ByteSizeLong());
}

unsigned int CClient::getSocketID() {
    return m_socketID;
}

void CClient::disconnect() {
    LOG_INFO("You close connection, socket id: {}", m_socketID);
    if (m_socketID != 0) {
        NetManager::instance().disconnect(m_socketID);
        m_socketID = 0;
    }
}

bool CClient::isConnected() {
    if (m_socketID != 0) {
        return NetManager::instance().isConnected(m_socketID);
    }
    return false;
}

void CClient::onConnectSucceed(const char* strRemoteIp, Port port, SocketId socketId) {
    m_socketID = socketId;
    m_connectCallback(true);
}

void CClient::onDisconnect(SocketId socketId) {
    m_socketID = 0;
    LuaScriptSystem::instance().Invoke("__APP_on_client_disconnected", static_cast<lua_api::IClient*>(this));
}

void CClient::onError(SocketId socketId, ec_net::ENetError error) {
    m_connectCallback(false);
}

void CClient::onParseMessage(const char* msgFullName, const char* pData, size_t size) {
}

void CClient::connect(const char* ip, Port port, BuffSize recv, BuffSize send, const char* tag) {
    NetManager::instance().connect(ip, port, recv, send);
    m_connectCallback = [this, t = std::string(tag)] (bool bResult) {
        if (!bResult) {
            return;
        }

        LuaScriptSystem::instance().Invoke("__APP_on_client_connected"
                                            , static_cast<lua_api::IClient*>(this)
                                            , t);
    };
}

void CClient::sendJsonMsg(const char* msgFullName, const char* jsonData) {
    if (!isConnected()) {
        return;
    }

    google::protobuf::Message* pMessage = ProtoManager::instance().createMessage(msgFullName);
    if (pMessage == nullptr) {
        LOG_WARN("Cannot find the message name:{}", msgFullName);
        return;
    }

    google::protobuf::util::JsonStringToMessage(jsonData, pMessage);

    if (!sendMsg(*pMessage)) {
        LOG_WARN("Cannot send the message:{}, json data:{}", msgFullName, jsonData);
    }
    delete pMessage;
}
