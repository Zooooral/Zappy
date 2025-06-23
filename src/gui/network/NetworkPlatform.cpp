/*
** EPITECH PROJECT, 2025
** src/gui/network/NetworkPlatform.cpp
** File description:
** Cross-platform network implementation
*/

#include "NetworkPlatform.hpp"
#include <iostream>
#include <cstring>

namespace zappy {
namespace network {

bool NetworkPlatform::initialize() {
    initializeNetworking();
    return true;
}

void NetworkPlatform::cleanup() {
    cleanupNetworking();
}

std::string NetworkPlatform::getErrorString(int error) {
#ifdef _WIN32
    char* messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer, 0, nullptr);
    
    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
#else
    return std::string(strerror(error));
#endif
}

bool NetworkPlatform::setNonBlocking(socket_t socket) {
#ifdef _WIN32
    u_long mode = 1;
    return ioctlsocket(socket, FIONBIO, &mode) == 0;
#else
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) return false;
    return fcntl(socket, F_SETFL, flags | O_NONBLOCK) == 0;
#endif
}

}
}
