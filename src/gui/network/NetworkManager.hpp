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
#include "NetworkPlatform.hpp"
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <memory>
#include <condition_variable>

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
    
    // Rule of 5 implementation
    ~NetworkManager();
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;
    NetworkManager(NetworkManager&&) = delete;
    NetworkManager& operator=(NetworkManager&&) = delete;
    
    bool connectToServer(const std::string& host, int port) override;
    void disconnect() override;
    bool isConnected() const override;
    void sendCommand(const std::string& command) override;
    void setMessageCallback(std::function<void(const std::string&)> callback) override;
    void update() override;

    ConnectionState getConnectionState() const;
    void setProtocolHandler(std::shared_ptr<ProtocolHandler> handler);

private:
    NetworkManager() = default;

    std::string _host;
    int _port = 0;
    socket_t _socket = INVALID_SOCKET_VALUE;
    std::atomic<ConnectionState> _connectionState{ConnectionState::DISCONNECTED};
    
    std::queue<std::string> _sendQueue;
    std::queue<std::string> _receiveQueue;
    mutable std::mutex _sendQueueMutex;
    mutable std::mutex _receiveQueueMutex;
    mutable std::mutex _callbackMutex;
    
    std::string _receiveBuffer;
    std::function<void(const std::string&)> _messageCallback;
    std::shared_ptr<ProtocolHandler> _protocolHandler;
    
    std::thread _networkThread;
    std::atomic<bool> _running{false};
    std::condition_variable _shutdownCV;
    std::mutex _shutdownMutex;

    void networkThreadLoop();
    void processReceiveBuffer();
    void handleServerMessage(const std::string& message);
    void sendAuthenticationMessage();
    
    bool createSocket();
    bool connectSocket();
    void closeSocket() noexcept;
    void sendData(const std::string& data);
    std::string receiveData();
    
    void safeShutdown();
};

#endif /* !NETWORKMANAGER_HPP_ */
