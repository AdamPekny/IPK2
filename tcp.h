/*
    File: tcp.h
    Author: Adam Pekný, xpekny00
    Description: Interface for tcp communication
*/

#include <vector>
#include <mutex>
#include <thread>

#include "server.h"


#ifndef TCP_H
#define TCP_H

class TCP{ // Class to establish tcp connection with clients
    private:
        Server *server;
	    int welcome_socket;
        std::vector<int> conn_clients;
        std::vector<std::thread> threads;
        std::mutex conn_clients_mutex;

    public:
        TCP() = default;
        TCP(Server &server);

        void await_connection();
};

#endif
