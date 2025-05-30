/*
** EPITECH PROJECT, 2025
** B-YEP-410
** File description:
** NetworkManager
*/

#ifndef NETWORKMANAGER_HPP_
    #define NETWORKMANAGER_HPP_

#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>

class NetworkManager {
public:
    enum class ConnectionState {
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        AUTHENTICATED,
        ERROR
    };

    static NetworkManager& getInstance();
    
    bool connectToServer(const std::string& host, int port);
    void disconnect();
    void sendCommand(const std::string& command);
    void update();
    
    ConnectionState getConnectionState() const { return _connectionState; }
    bool isConnected() const { return _connectionState == ConnectionState::AUTHENTICATED; }
    
    void setCommandCallback(std::function<void(const std::string&)> callback);

private:
    NetworkManager();
    ~NetworkManager();
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;

    void networkThreadFunction();
    void handleServerMessage(const std::string& message);
    void processWelcome();
    void processMapSize(const std::string& message);
    void processCommand(const std::string& command);
    void sendAuthenticationMessage(const std::string& message);

    int _socket;
    std::atomic<ConnectionState> _connectionState;
    std::thread _networkThread;
    std::atomic<bool> _shouldStop;
    
    std::mutex _sendQueueMutex;
    std::queue<std::string> _sendQueue;
    
    std::mutex _receiveQueueMutex;
    std::queue<std::string> _receiveQueue;
    
    std::function<void(const std::string&)> _commandCallback;
    
    std::string _host;
    int _port;
    
    std::string _receiveBuffer;
    
    bool createSocket();
    bool connectSocket();
    void closeSocket();
    void sendData(const std::string& data);
    std::string receiveData();
    void processReceiveBuffer();
};

#endif /* !NETWORKMANAGER_HPP_ */
