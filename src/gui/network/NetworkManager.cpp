/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel
** File description:
** NetworkManager
*/

#include "NetworkManager.hpp"
#include "../core/Constants.hpp"
#include <iostream>
#include <cstring>
#include <memory>
#include <chrono>

NetworkManager& NetworkManager::getInstance() {
    static NetworkManager instance;
    return instance;
}

NetworkManager::~NetworkManager() {
    disconnect();
}

bool NetworkManager::connectToServer(const std::string& host, int port) {
    if (_connectionState.load() != ConnectionState::DISCONNECTED) {
        disconnect();
    }

    _host = host;
    _port = port;
    _connectionState.store(ConnectionState::CONNECTING);

    if (!createSocket() || !connectSocket()) {
        _connectionState.store(ConnectionState::ERROR);
        closeSocket();
        return false;
    }

    _connectionState.store(ConnectionState::CONNECTED);
    _running.store(true);
    _networkThread = std::thread(&NetworkManager::networkThreadLoop, this);

    return true;
}

void NetworkManager::disconnect() {
    safeShutdown();
}

void NetworkManager::safeShutdown() {
    _running.store(false);
    
    {
        std::lock_guard<std::mutex> lock(_shutdownMutex);
        _shutdownCV.notify_all();
    }
    
    if (_networkThread.joinable()) {
        _networkThread.join();
    }
    
    closeSocket();
    _connectionState.store(ConnectionState::DISCONNECTED);
    
    {
        std::lock_guard<std::mutex> sendLock(_sendQueueMutex);
        std::lock_guard<std::mutex> receiveLock(_receiveQueueMutex);
        
        while (!_sendQueue.empty()) _sendQueue.pop();
        while (!_receiveQueue.empty()) _receiveQueue.pop();
    }
    
    _receiveBuffer.clear();
}

bool NetworkManager::isConnected() const {
    ConnectionState state = _connectionState.load();
    return state == ConnectionState::CONNECTED || 
           state == ConnectionState::AUTHENTICATED;
}

void NetworkManager::sendCommand(const std::string& command) {
    if (!isConnected()) return;
    
    std::lock_guard<std::mutex> lock(_sendQueueMutex);
    _sendQueue.push(command + "\n");
}

void NetworkManager::setMessageCallback(std::function<void(const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(_callbackMutex);
    _messageCallback = std::move(callback);
}

void NetworkManager::update() {
    std::lock_guard<std::mutex> lock(_receiveQueueMutex);
    while (!_receiveQueue.empty()) {
        std::string message = _receiveQueue.front();
        _receiveQueue.pop();
        
        if (_protocolHandler) {
            try {
                _protocolHandler->handleCommand(message);
            } catch (const std::exception& e) {
                std::cerr << "Protocol handler error: " << e.what() << std::endl;
            }
        }
        
        {
            std::lock_guard<std::mutex> callbackLock(_callbackMutex);
            if (_messageCallback) {
                try {
                    _messageCallback(message);
                } catch (const std::exception& e) {
                    std::cerr << "Message callback error: " << e.what() << std::endl;
                }
            }
        }
    }
}

ConnectionState NetworkManager::getConnectionState() const {
    return _connectionState.load();
}

void NetworkManager::setProtocolHandler(std::shared_ptr<ProtocolHandler> handler) {
    _protocolHandler = std::move(handler);
}

void NetworkManager::networkThreadLoop() {
    fd_set readSet, writeSet;
    struct timeval timeout;

    while (_running.load()) {
        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        
#ifdef _WIN32
        FD_SET(_socket, &readSet);
#else
        if (_socket != INVALID_SOCKET_VALUE) {
            FD_SET(_socket, &readSet);
        }
#endif

        {
            std::lock_guard<std::mutex> lock(_sendQueueMutex);
            if (!_sendQueue.empty() && _socket != INVALID_SOCKET_VALUE) {
                FD_SET(_socket, &writeSet);
            }
        }

        timeout.tv_sec = 0;
        timeout.tv_usec = zappy::constants::NETWORK_TIMEOUT_US;

        int ready = select(static_cast<int>(_socket + 1), &readSet, &writeSet, nullptr, &timeout);
        
        if (ready < 0) {
            _connectionState.store(ConnectionState::ERROR);
            break;
        }

        if (ready == 0) {
            // Timeout
            continue;
        }

        if (FD_ISSET(_socket, &readSet)) {
            std::string data = receiveData();
            if (data.empty()) {
                _connectionState.store(ConnectionState::ERROR);
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
    ConnectionState currentState = _connectionState.load();
    
    if (currentState == ConnectionState::CONNECTED && message == "WELCOME") {
        sendAuthenticationMessage();
    } else if (message.substr(0, 4) == "msz ") {
        _connectionState.store(ConnectionState::AUTHENTICATED);
        std::lock_guard<std::mutex> lock(_receiveQueueMutex);
        _receiveQueue.push(message);
    } else if (currentState == ConnectionState::AUTHENTICATED) {
        std::lock_guard<std::mutex> lock(_receiveQueueMutex);
        _receiveQueue.push(message);
    }
}

void NetworkManager::sendAuthenticationMessage() {
    sendData("GRAPHIC\n");
}

bool NetworkManager::createSocket() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    return _socket != INVALID_SOCKET_VALUE;
}

bool NetworkManager::connectSocket() {
    struct hostent* hostEntry = gethostbyname(_host.c_str());
    if (!hostEntry) {
        std::cerr << "Failed to resolve hostname: " << _host << std::endl;
        return false;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(static_cast<uint16_t>(_port));
    memcpy(&serverAddr.sin_addr, hostEntry->h_addr_list[0], hostEntry->h_length);

    int result = connect(_socket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr));
    if (result != 0) {
        std::cerr << "Failed to connect: " << zappy::network::NetworkPlatform::getErrorString(getLastError()) << std::endl;
        return false;
    }
    
    return true;
}

void NetworkManager::closeSocket() noexcept {
    if (_socket != INVALID_SOCKET_VALUE) {
        ::closeSocket(_socket);
        _socket = INVALID_SOCKET_VALUE;
    }
}

void NetworkManager::sendData(const std::string& data) {
    if (_socket != INVALID_SOCKET_VALUE) {
        send(_socket, data.c_str(), static_cast<int>(data.length()), 0);
    }
}

std::string NetworkManager::receiveData() {
    if (_socket == INVALID_SOCKET_VALUE) return "";

    char buffer[zappy::constants::NETWORK_BUFFER_SIZE];
    int bytesReceived = recv(_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived <= 0) return "";

    buffer[bytesReceived] = '\0';
    return std::string(buffer);
}
