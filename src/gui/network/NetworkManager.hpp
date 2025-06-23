/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel
** File description:
** NetworkManager
*/

#ifndef NETWORKMANAGER_HPP_
#define NETWORKMANAGER_HPP_

#include "../interfaces/INetworkClient.hpp"
#include "ProtocolHandler.hpp"
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <memory>

enum class ConnectionState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    AUTHENTICATED,
    ERROR
};

class NetworkManager : public INetworkClient {
public:
    static NetworkManager& getInstance();
    
    bool connectToServer(const std::string& host, int port) override;
    void disconnect() override;
    bool isConnected() const override;
    void sendCommand(const std::string& command) override;
    void setMessageCallback(std::function<void(const std::string&)> callback) override;
    void update() override;

    ConnectionState getConnectionState() const { return _connectionState; }
    void setProtocolHandler(std::shared_ptr<ProtocolHandler> handler);

private:
    NetworkManager() = default;
    ~NetworkManager();

    std::string _host;
    int _port = 0;
    int _socket = -1;
    ConnectionState _connectionState = ConnectionState::DISCONNECTED;
    
    std::queue<std::string> _sendQueue;
    std::queue<std::string> _receiveQueue;
    std::mutex _sendQueueMutex;
    std::mutex _receiveQueueMutex;
    
    std::string _receiveBuffer;
    std::function<void(const std::string&)> _messageCallback;
    std::shared_ptr<ProtocolHandler> _protocolHandler;
    
    std::thread _networkThread;
    std::atomic<bool> _running{false};

    void networkThreadLoop();
    void processReceiveBuffer();
    void handleServerMessage(const std::string& message);
    void sendAuthenticationMessage();
    
    bool createSocket();
    bool connectSocket();
    void closeSocket();
    void sendData(const std::string& data);
    std::string receiveData();
};

#endif
