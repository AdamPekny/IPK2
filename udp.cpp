/*
    File: udp.cpp
    Author: Adam Pekný, xpekny00
    Description: Module responsible for UDP communication
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <signal.h>
#include <cstring>
#include <optional>

#include "server.h"
#include "payloadparser.h"
#include "error.h"
#include "udpmessage.h"
#include "interrupt.h"


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

UDP::UDP(Server &server){
    this->server = &server;

    int optval;
    struct sockaddr_in server_address;
    /* Create socket */
	if ((this->server_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0){
		error_exit(99);
	}
    /* Surpress default action of socket */ 
    optval = 1;
    setsockopt(this->server_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    /* Set server address and bind port to that address */
    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, this->server->gethost().c_str(), &server_address.sin_addr);
    server_address.sin_port = htons((unsigned short) this->server->getport());
	
    if (bind(this->server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
        error_exit(99);
    }
}

void UDP::listen(){
	int bytestx, bytesrx;
    socklen_t clientlen;
    struct sockaddr_in client_address;
    PayloadParser payload_parser;
    
    struct sigaction sigIntHandler;
    set_cc_handler(&sigIntHandler);

    while(1){   
    	/* Catch message from client */
        this->buf.buf_bzero();
        clientlen = sizeof(client_address);
        bytesrx = recvfrom(this->server_socket, this->buf.get_buf(), this->buf.get_bufsize(), 0, (struct sockaddr *) &client_address, &clientlen);
        
        if (force_quit){
            close(this->server_socket);
            exit(0);
        }

        if (bytesrx < 0) error_exit(99);


        /* Load ackquired message payload to parser */
        payload_parser = PayloadParser(this->buf.get_payload());

        if (this->buf.get_opcode() != 0){ // Check if opcode is valid
            this->buf.set_statuscode(1);
            this->buf.set_payload("Opcode Error!");
        }

        // Set buffer to response buffer
        this->buf.set_opcode(1);
        this->buf.set_statuscode(0);

        try{ // Parse the payload and check for lex and syntax errors
            payload_parser.parse();
        } catch (const PayloadParser::LexException &e){
            this->buf.set_statuscode(1);
            this->buf.set_payload("Error parsing message: Lexical error!");
        } catch (const PayloadParser::SyntaxException &e){
            this->buf.set_statuscode(1);
            this->buf.set_payload("Error parsing message: Syntax error!");
        }
        
        try{ // Calculate result and check for mathematical errors
            if (!this->buf.get_statuscode()) this->buf.set_payload(std::to_string(payload_parser.calc()));
        } catch (const PayloadParser::CalcException &e){
            this->buf.set_statuscode(1);
            this->buf.set_payload("Error parsing message: Calculation error!");
        }
        
        /* Send message back to client  */        
        bytestx = sendto(this->server_socket, this->buf.get_buf(), this->buf.get_bufsize(), 0, (struct sockaddr *) &client_address, clientlen);
        if (bytestx < 0) perror("ERROR: sendto:");
    }
}
