/*
    File: server.cpp
    Author: Adam Pekný, xpekny00
    Description: Module for validating and storing server data
*/

#include <iostream>
#include <string.h>
#include <regex>


class Server{ // Class for validating and storing server data
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

Server::Server(std::string host, std::string mode, size_t port){ // Constructor
    if (!validate_data(host, mode, port)) throw std::invalid_argument("Provided arguments are wrong!");

    this->host = host;
    this->mode = mode;
    this->port = port;
}

bool Server::validate_data(std::string host, std::string mode, size_t port){ // Method for validating data
    if (port > 2e16 - 1) return false;
    
    if (mode != "udp" && mode != "tcp") return false;

    std::regex ipv4_re("^((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[0]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[0]?[0-9][0-9]?)$");

    if (!std::regex_match(host, ipv4_re)) return false;

    return true;
}

std::string Server::gethost(){ // Method for retrieving host ipv4
    return this->host;
}

std::string Server::getmode(){ // Method for retrieving mode
    return this->mode;
}

size_t Server::getport(){ // Method for retrieving port number
    return this->port;
}
