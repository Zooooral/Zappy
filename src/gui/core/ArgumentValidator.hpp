/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel
** File description:
** ArgumentValidator
*/

#ifndef ARGUMENTVALIDATOR_HPP_
#define ARGUMENTVALIDATOR_HPP_

#include <string>
#include <memory>

class INetworkClient;

class ArgumentValidator {
public:
    struct Arguments {
        std::string host;
        int port;
        bool valid;
        std::string errorMessage;
        
        Arguments() : host(""), port(0), valid(false), errorMessage("") {}
    };

    static Arguments parseArguments(int argc, char** argv);
    static bool validateConnection(const std::string& host, int port);
    static void printHelp(const char* programName);

private:
    static bool isValidPort(int port);
    static bool isValidHostname(const std::string& host);
    static std::unique_ptr<INetworkClient> createTestClient();
};

#endif
