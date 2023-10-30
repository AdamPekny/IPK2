/*
    File: ipkcpd.cpp
    Author: Adam Pekný, xpekny00
    Description: Main file of server program
*/

#include <iostream>
#include <unistd.h>
#include <string.h>

#include "error.h"
#include "tcp.h"
#include "udp.h"
#include "interrupt.h"

// Flag indicating C-c exit
int force_quit = 0;

// Function for displaying help if requested by user
bool display_help(int argc, char *arg1){
    if (argc == 2){
        if (!strcmp(arg1, "--help")){
            std::cout << "Usage:\n\tipkcpd -h <host> -p <port> -m <mode>\n\tipkcpd --help\n";
            std::cout << "Options:\n";
            std::cout << "\t<host> -> IPv4 address of host\n";
            std::cout << "\t<port> -> Port number the host is listening on\n";
            std::cout << "\t<mode> -> tcp or udp\n";
        }
        return true;
    }

    return false;
}


size_t strtsize(char *str){ // Function to convert string to to size_t
    long tmp;
    char *endptr;
    tmp = strtoul(str, &endptr, 10);

    if (*endptr != '\0') error_exit(10);

    return (size_t) tmp;
}

int main(int argc, char *argv[]){ // Main body of server program
    int opt;
    std::string host, mode;
    size_t port;

    // Parse arguments and store them
    if (display_help(argc, argv[1])) return 0;

    if (argc != 7) error_exit(10);

    while((opt = getopt(argc, argv, "h:p:m:")) != -1){
        switch (opt){
        case 'h':
            host = optarg;
            break;
        case 'm':
            mode = optarg;
            break;
        case 'p':
            port = strtsize(optarg);
            break;
        default:
            break;
        }
    }

    // Initialize server
    Server server;

    try{
        server = Server(host, mode, port);
    }
    catch(const std::invalid_argument &e){
        error_exit(10);
    }
    
    // Run connection based on mode chosen
    if (server.getmode() == "tcp"){
        TCP conn = TCP(server);

        conn.await_connection();
    } else {
        UDP conn = UDP(server);

        conn.listen();
    }

    return 0;
}
