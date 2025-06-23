/*
** EPITECH PROJECT, 2025
** src/gui/network/NetworkPlatform.hpp
** File description:
** Cross-platform network abstraction
*/

#ifndef NETWORKPLATFORM_HPP_
#define NETWORKPLATFORM_HPP_

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    
    using socket_t = SOCKET;
    constexpr socket_t INVALID_SOCKET_VALUE = INVALID_SOCKET;
    
    inline int closeSocket(socket_t sock) { return closesocket(sock); }
    inline int getLastError() { return WSAGetLastError(); }
    inline void initializeNetworking() { 
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    }
    inline void cleanupNetworking() { WSACleanup(); }
    
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
    
    using socket_t = int;
    constexpr socket_t INVALID_SOCKET_VALUE = -1;
    
    inline int closeSocket(socket_t sock) { return close(sock); }
    inline int getLastError() { return errno; }
    inline void initializeNetworking() { }
    inline void cleanupNetworking() { }
    
#endif

#include <string>

namespace zappy {
namespace network {

class NetworkPlatform {
public:
    static bool initialize();
    static void cleanup();
    static std::string getErrorString(int error);
    static bool setNonBlocking(socket_t socket);
};

}
}

#endif /* !NETWORKPLATFORM_HPP_ */
