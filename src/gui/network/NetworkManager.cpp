/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel
** File description:
** NetworkManager
*/

#include "NetworkManager.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <memory>

NetworkManager& NetworkManager::getInstance() {
    static NetworkManager instance;
    return instance;
}

NetworkManager::~NetworkManager() {
    disconnect();
}

bool NetworkManager::connectToServer(const std::string& host, int port) {
    if (_connectionState != ConnectionState::DISCONNECTED) {
        disconnect();
    }

    _host = host;
    _port = port;
    _connectionState = ConnectionState::CONNECTING;

    if (!createSocket() || !connectSocket()) {
        _connectionState = ConnectionState::ERROR;
        closeSocket();
        return false;
    }

    _connectionState = ConnectionState::CONNECTED;
    _running = true;
    _networkThread = std::thread(&NetworkManager::networkThreadLoop, this);

    return true;
}

void NetworkManager::disconnect() {
    _running = false;
    
    if (_networkThread.joinable()) {
        _networkThread.join();
    }
    
    closeSocket();
    _connectionState = ConnectionState::DISCONNECTED;
    
    std::lock_guard<std::mutex> sendLock(_sendQueueMutex);
    std::lock_guard<std::mutex> receiveLock(_receiveQueueMutex);
    
    while (!_sendQueue.empty()) _sendQueue.pop();
    while (!_receiveQueue.empty()) _receiveQueue.pop();
    
    _receiveBuffer.clear();
}

bool NetworkManager::isConnected() const {
    return _connectionState == ConnectionState::CONNECTED || 
           _connectionState == ConnectionState::AUTHENTICATED;
}

void NetworkManager::sendCommand(const std::string& command) {
    if (!isConnected()) return;
    
    std::lock_guard<std::mutex> lock(_sendQueueMutex);
    _sendQueue.push(command + "\n");
}

void NetworkManager::setMessageCallback(std::function<void(const std::string&)> callback) {
    _messageCallback = callback;
}

void NetworkManager::update() {
    std::lock_guard<std::mutex> lock(_receiveQueueMutex);
    while (!_receiveQueue.empty()) {
        std::string message = _receiveQueue.front();
        _receiveQueue.pop();
        
        if (_protocolHandler) {
            _protocolHandler->handleCommand(message);
        }
        
        if (_messageCallback) {
            _messageCallback(message);
        }
    }
}

void NetworkManager::setProtocolHandler(std::shared_ptr<ProtocolHandler> handler) {
    _protocolHandler = handler;
}

void NetworkManager::networkThreadLoop() {
    fd_set readSet, writeSet;
    struct timeval timeout;

    while (_running) {
        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        FD_SET(_socket, &readSet);

        {
            std::lock_guard<std::mutex> lock(_sendQueueMutex);
            if (!_sendQueue.empty()) {
                FD_SET(_socket, &writeSet);
            }
        }

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        int ready = select(_socket + 1, &readSet, &writeSet, nullptr, &timeout);
        
        if (ready < 0) {
            _connectionState = ConnectionState::ERROR;
            break;
        }

        if (FD_ISSET(_socket, &readSet)) {
            std::string data = receiveData();
            if (data.empty()) {
                _connectionState = ConnectionState::ERROR;
                break;
            }
            _receiveBuffer += data;
            processReceiveBuffer();
        }

        if (FD_ISSET(_socket, &writeSet)) {
            std::lock_guard<std::mutex> lock(_sendQueueMutex);
            if (!_sendQueue.empty()) {
                std::string data = _sendQueue.front();
                _sendQueue.pop();
                sendData(data);
            }
        }
    }
}

void NetworkManager::processReceiveBuffer() {
    size_t pos = 0;
    while ((pos = _receiveBuffer.find('\n')) != std::string::npos) {
        std::string message = _receiveBuffer.substr(0, pos);
        _receiveBuffer.erase(0, pos + 1);

        if (!message.empty() && message.back() == '\r') {
            message.pop_back();
        }

        handleServerMessage(message);
    }
}

void NetworkManager::handleServerMessage(const std::string& message) {
    if (_connectionState == ConnectionState::CONNECTED && message == "WELCOME") {
        sendAuthenticationMessage();
    } else if (message.substr(0, 4) == "msz ") {
        _connectionState = ConnectionState::AUTHENTICATED;
        std::lock_guard<std::mutex> lock(_receiveQueueMutex);
        _receiveQueue.push(message);
    } else if (_connectionState == ConnectionState::AUTHENTICATED) {
        std::lock_guard<std::mutex> lock(_receiveQueueMutex);
        _receiveQueue.push(message);
    }
}

void NetworkManager::sendAuthenticationMessage() {
    sendData("GRAPHIC\n");
}

bool NetworkManager::createSocket() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    return _socket != -1;
}

bool NetworkManager::connectSocket() {
    struct hostent* hostEntry = gethostbyname(_host.c_str());
    if (!hostEntry) return false;

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    memcpy(&serverAddr.sin_addr, hostEntry->h_addr_list[0], hostEntry->h_length);

    return connect(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == 0;
}

void NetworkManager::closeSocket() {
    if (_socket != -1) {
        close(_socket);
        _socket = -1;
    }
}

void NetworkManager::sendData(const std::string& data) {
    if (_socket != -1) {
        send(_socket, data.c_str(), data.length(), 0);
    }
}

std::string NetworkManager::receiveData() {
    if (_socket == -1) return "";

    char buffer[1024];
    ssize_t bytesReceived = recv(_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived <= 0) return "";

    buffer[bytesReceived] = '\0';
    return std::string(buffer);
}
