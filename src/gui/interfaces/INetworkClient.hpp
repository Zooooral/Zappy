/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel
** File description:
** INetworkClient
*/

#ifndef INETWORKCLIENT_HPP_
#define INETWORKCLIENT_HPP_

#include <string>
#include <functional>

class INetworkClient {
public:
    virtual ~INetworkClient() = default;
    virtual bool connectToServer(const std::string& host, int port) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    virtual void sendCommand(const std::string& command) = 0;
    virtual void setMessageCallback(std::function<void(const std::string&)> callback) = 0;
    virtual void update() = 0;
};

#endif
