/*
    File: server.h
    Author: Adam Pekný, xpekny00
    Description: Interface for server module
*/

#include <iostream>
#include <string.h>

#ifndef SERVER_H
#define SERVER_H

class Server{
private:
    std::string host;
    std::string mode;
    size_t port;
    
    bool validate_data(std::string host, std::string mode, size_t port);
public:
    Server() = default;
    Server(std::string host, std::string mode_str, size_t port);

    std::string gethost();
    std::string getmode();
    size_t getport();
};

#endif
