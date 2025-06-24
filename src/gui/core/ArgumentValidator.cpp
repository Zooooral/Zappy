/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel
** File description:
** ArgumentValidator
*/

#include "ArgumentValidator.hpp"
#include "../network/NetworkManager.hpp"
#include "../interfaces/INetworkClient.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <regex>
#include <chrono>
#include <thread>

ArgumentValidator::Arguments ArgumentValidator::parseArguments(int argc, char** argv)
{
    Arguments args;

    if (argc < 5) {
        args.errorMessage = "Missing required arguments. GUI requires both port and machine.";
        return args;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 >= argc) {
                args.errorMessage = "Missing port value after -p";
                return args;
            }
            args.port = std::atoi(argv[i + 1]);
            if (!isValidPort(args.port)) {
                args.errorMessage = "Invalid port number: " + std::to_string(args.port);
                return args;
            }
            i++;
        } else if (strcmp(argv[i], "-h") == 0) {
            if (i + 1 >= argc) {
                args.errorMessage = "Missing hostname value after -h";
                return args;
            }
            args.host = argv[i + 1];
            if (!isValidHostname(args.host)) {
                args.errorMessage = "Invalid hostname: " + args.host;
                return args;
            }
            i++;
        } else {
            args.errorMessage = "Unknown argument: " + std::string(argv[i]);
            return args;
        }
    }

    if (args.host.empty()) {
        args.errorMessage = "Missing required argument: -h machine";
        return args;
    }

    if (args.port == 0) {
        args.errorMessage = "Missing required argument: -p port";
        return args;
    }

    args.valid = true;
    return args;
}

bool ArgumentValidator::validateConnection(const std::string& host, int port)
{
    auto testClient = createTestClient();
    if (!testClient) {
        NetworkManager& networkManager = NetworkManager::getInstance();
        
        std::cout << "[GUI] Testing connection to " << host << ":" << port << "..." << std::endl;

        bool connected = networkManager.connectToServer(host, port);
        
        if (connected) {
            std::cout << "[GUI] Connection test successful" << std::endl;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            networkManager.disconnect();
            return true;
        }

        std::cout << "[GUI] Connection test failed - server unreachable" << std::endl;
        return false;
    }

    std::cout << "[GUI] Testing connection to " << host << ":" << port << "..." << std::endl;

    bool connected = testClient->connectToServer(host, port);
    
    if (connected) {
        std::cout << "[GUI] Connection test successful" << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        testClient->disconnect();
        return true;
    }

    std::cout << "[GUI] Connection test failed - server unreachable" << std::endl;
    return false;
}

void ArgumentValidator::printHelp(const char* programName)
{
    std::cout << "USAGE: " << programName << " -p port -h machine" << std::endl;
    std::cout << "  option\tdescription" << std::endl;
    std::cout << "  -p port\tport number (1024-65535)" << std::endl;
    std::cout << "  -h machine\thostname or IP address of the server" << std::endl;
    std::cout << std::endl;
    std::cout << "The GUI requires a valid connection to the Zappy server." << std::endl;
    std::cout << "Connection will be tested before starting the interface." << std::endl;
}

bool ArgumentValidator::isValidPort(int port)
{
    return port >= 1024 && port <= 65535;
}

bool ArgumentValidator::isValidHostname(const std::string& host)
{
    if (host.empty() || host.length() > 255) {
        return false;
    }

    std::regex hostnameRegex(
        R"(^([a-zA-Z0-9]([a-zA-Z0-9\-]{0,61}[a-zA-Z0-9])?\.)*[a-zA-Z0-9]([a-zA-Z0-9\-]{0,61}[a-zA-Z0-9])?$|^(\d{1,3}\.){3}\d{1,3}$|^localhost$)"
    );

    return std::regex_match(host, hostnameRegex);
}

std::unique_ptr<INetworkClient> ArgumentValidator::createTestClient()
{
    // Singleton can't create unique_ptr. Return nullptr for now.
    return nullptr;
}
