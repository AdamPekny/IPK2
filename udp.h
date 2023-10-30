/*
    File: udp.h
    Author: Adam Pekný, xpekny00
    Description: Interface for UDP module
*/

#include "server.h"
#include "udpmessage.h"


#ifndef UDP_H
#define UDP_H

class UDP{
    private:
        Server *server;
        UDPMessage buf;
        int server_socket;
    public:
        UDP() = default;
        UDP(Server &server);

        void listen();
};

#endif
