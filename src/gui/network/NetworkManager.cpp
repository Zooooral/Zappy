/*
** EPITECH PROJECT, 2025
** B-YEP-410
** File description:
** NetworkManager
*/

#include "NetworkManager.hpp"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <sstream>

NetworkManager& NetworkManager::getInstance()
{
    static NetworkManager instance;
    return instance;
}

NetworkManager::NetworkManager() : 
    _socket(-1), 
    _connectionState(ConnectionState::DISCONNECTED),
    _shouldStop(false),
    _port(0)
{
}

NetworkManager::~NetworkManager()
{
    disconnect();
}

bool NetworkManager::connectToServer(const std::string& host, int port)
{
    if (_connectionState != ConnectionState::DISCONNECTED) {
        return false;
    }

    _host = host;
    _port = port;
    _connectionState = ConnectionState::CONNECTING;
    
    if (!createSocket()) {
        _connectionState = ConnectionState::ERROR;
        return false;
    }
    
    if (!connectSocket()) {
        closeSocket();
        _connectionState = ConnectionState::ERROR;
        return false;
    }
    
    _connectionState = ConnectionState::CONNECTED;
    _shouldStop = false;
    _networkThread = std::thread(&NetworkManager::networkThreadFunction, this);
    
    return true;
}

void NetworkManager::disconnect()
{
    if (_connectionState == ConnectionState::DISCONNECTED) {
        return;
    }
    
    _shouldStop = true;
    
    if (_networkThread.joinable()) {
        _networkThread.join();
    }
    
    closeSocket();
    _connectionState = ConnectionState::DISCONNECTED;
    
    {
        std::lock_guard<std::mutex> sendLock(_sendQueueMutex);
        std::queue<std::string> emptySendQueue;
        _sendQueue.swap(emptySendQueue);
    }
    
    {
        std::lock_guard<std::mutex> receiveLock(_receiveQueueMutex);
        std::queue<std::string> emptyReceiveQueue;
        _receiveQueue.swap(emptyReceiveQueue);
    }
}

void NetworkManager::sendCommand(const std::string& command)
{
    if (_connectionState != ConnectionState::AUTHENTICATED) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(_sendQueueMutex);
    _sendQueue.push(command + "\n");
}

void NetworkManager::sendAuthenticationMessage(const std::string& message)
{
    std::lock_guard<std::mutex> lock(_sendQueueMutex);
    _sendQueue.push(message + "\n");
}

void NetworkManager::update()
{
    std::lock_guard<std::mutex> lock(_receiveQueueMutex);
    
    while (!_receiveQueue.empty()) {
        std::string message = _receiveQueue.front();
        _receiveQueue.pop();
        handleServerMessage(message);
    }
}

void NetworkManager::setCommandCallback(std::function<void(const std::string&)> callback)
{
    _commandCallback = callback;
}

void NetworkManager::networkThreadFunction()
{
    while (!_shouldStop) {
        fd_set readSet, writeSet;
        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        FD_SET(_socket, &readSet);
        
        bool hasSendData = false;
        {
            std::lock_guard<std::mutex> lock(_sendQueueMutex);
            hasSendData = !_sendQueue.empty();
        }
        
        if (hasSendData) {
            FD_SET(_socket, &writeSet);
        }
        
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;
        
        int result = select(_socket + 1, &readSet, &writeSet, nullptr, &timeout);
        
        if (result < 0) {
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

void NetworkManager::handleServerMessage(const std::string& message)
{
    std::cout << "[SERVER->GUI] " << message << std::endl;
    if (_connectionState == ConnectionState::CONNECTED && message == "WELCOME") {
        processWelcome();
    } else if (_connectionState == ConnectionState::CONNECTED && message.substr(0, 4) == "msz ") {
        _connectionState = ConnectionState::AUTHENTICATED;
        processMapSize(message);
    } else if (_connectionState == ConnectionState::AUTHENTICATED) {
        if (message.substr(0, 4) == "msz ") {
            processMapSize(message);
        } else {
            processCommand(message);
        }
    }
}

void NetworkManager::processWelcome()
{
    sendAuthenticationMessage("GRAPHIC");
}

void NetworkManager::processMapSize(const std::string& message)
{
    if (_commandCallback) {
        _commandCallback(message);
    }
}

void NetworkManager::processCommand(const std::string& command)
{
    if (_commandCallback) {
        _commandCallback(command);
    }
}

bool NetworkManager::createSocket()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    return _socket != -1;
}

bool NetworkManager::connectSocket()
{
    struct hostent* hostEntry = gethostbyname(_host.c_str());
    if (!hostEntry) {
        return false;
    }
    
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    memcpy(&serverAddr.sin_addr, hostEntry->h_addr_list[0], hostEntry->h_length);
    
    int result = connect(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    return result == 0;
}

void NetworkManager::closeSocket()
{
    if (_socket != -1) {
        close(_socket);
        _socket = -1;
    }
}

void NetworkManager::sendData(const std::string& data)
{
    if (_socket != -1) {
        send(_socket, data.c_str(), data.length(), 0);
    }
}

std::string NetworkManager::receiveData()
{
    if (_socket == -1) {
        return "";
    }
    
    char buffer[1024];
    ssize_t bytesReceived = recv(_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesReceived <= 0) {
        return "";
    }
    
    buffer[bytesReceived] = '\0';
    return std::string(buffer);
}

void NetworkManager::processReceiveBuffer()
{
    size_t pos = 0;
    while ((pos = _receiveBuffer.find('\n')) != std::string::npos) {
        std::string message = _receiveBuffer.substr(0, pos);
        _receiveBuffer.erase(0, pos + 1);
        
        if (!message.empty() && message.back() == '\r') {
            message.pop_back();
        }
        
        {
            std::lock_guard<std::mutex> lock(_receiveQueueMutex);
            _receiveQueue.push(message);
        }
    }
}
