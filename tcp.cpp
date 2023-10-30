/*
    File: tcp.cpp
    Author: Adam Pekný, xpekny00
    Description: Module responsible for tcp communication
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#include <vector>
#include <thread>
#include <mutex>

#include "server.h"
#include "tcpmessage.h"
#include "payloadparser.h"
#include "interrupt.h"
#include "error.h"


class TCPHandleClient{ // Class for handling individula clients in threads
    private:
        int socket;
        std::vector<int> *conn_clients;

        int get_clienti();
        void calc_client(TCPMessage &buf);

    public:
        void operator()(std::vector<int> *conn_clients, int comm_socket, std::mutex *conn_clients_mutex);
};

int TCPHandleClient::get_clienti(){ // Method to retrieve index of client communication socket in active clients vector
    for (size_t i = 0; i < this->conn_clients->size(); i++){
        if ((*this->conn_clients)[i] == this->socket) return i;
    }

    return -1;
}

void TCPHandleClient::calc_client(TCPMessage &buf){ // Method to handle calculations for client
    TCPMessage buff; // Buffer for individual queries
    int res;
    std::string cmd;

    for (;;){ // Infinite loop to keep communication with client
        while (!buf.load_cmd(cmd)){ // Keep loading messages until \n is present in message
            res = recv(this->socket, buf.get_buf(), buf.get_bufsize(), 0);
            if (res <= 0) return; // On error or socket shutdown end communication
            buf.concat_buf(); // Concatenate message to previous ones
        }
        // Get partial query
        buff.buf_bzero();
        buff.set_buf(cmd);

        std::string keyword = buff.get_keyword();
        std::string payload = buff.get_payload();
        payload = payload.substr(0, payload.length() - 1);
        
        if (keyword == "SOLVE" && buff.validate_query_buf()) { // Solve is valid and found
            // Prepare buffer for response
            buff.buf_bzero();
            PayloadParser parser = PayloadParser(payload);
            long result;

            try{ // Parse partial query
                parser.parse();
            } catch (const PayloadParser::LexException &e){
                break;
            } catch (const PayloadParser::SyntaxException &e){
                break;
            }

            try{ // Evaluate result
                result = parser.calc();
            } catch (const PayloadParser::CalcException &e){
                std::cout << "erorr calc" << std::endl;
                break;
            }

            // Return result
            buff.set_keyword("RESULT");
            buff.set_payload(std::to_string(result));
            send(this->socket, buff.get_buf(), strlen(buff.get_buf()), 0);
        } else { // Partial query is not valid
            break;
        }
        
    }

    // Send response with result
    buff.buf_bzero();
    buff.set_keyword("BYE");

    send(this->socket, buff.get_buf(), strlen(buff.get_buf()), 0);
}

// Overloads operator in order to be used in thread
// Handle connection until HELLO is received and closing of connection
void TCPHandleClient::operator()(std::vector<int> *conn_clients, int comm_socket, std::mutex *conn_clients_mutex){
    // Get communication socket and vector of active clients
    conn_clients_mutex->lock();
    this->socket = comm_socket;
    this->conn_clients = conn_clients;
    conn_clients_mutex->unlock();

    TCPMessage buff2;
    TCPMessage buff;
    int res;
    std::string cmd;

    do { // Get first message and continue until \n is present
        res = recv(this->socket, buff2.get_buf(), buff2.get_bufsize(), 0);
        if (res <= 0) return;
        buff2.concat_buf();
    } while (!buff2.load_cmd(cmd));
    
    // Get partial query
    buff.buf_bzero();
    buff.set_buf(cmd);
    
    if (buff.get_keyword() == "HELLO" && buff.validate_keyword_buf()){ // Partual query is HELLO and valid
        // Send HELLO back
        buff.buf_bzero();
        buff.set_keyword("HELLO");
        send(this->socket, buff.get_buf(), strlen(buff.get_buf()), 0);
        // Move to calculations handling
        this->calc_client(buff2);
    } else { // Partial query is not HELLO query or is not valid
        buff.buf_bzero();
        buff.set_keyword("BYE");
        send(this->socket, buff.get_buf(), strlen(buff.get_buf()), 0);
    }

    // When communication with client is being terminated handle socket
    conn_clients_mutex->lock();
    int new_clienti = this->get_clienti();

    if (new_clienti >= 0){
        // Remove client from active clients and close socket
        this->conn_clients->erase(this->conn_clients->begin() + new_clienti);
        close(this->socket);
    }
    conn_clients_mutex->unlock();
}


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

TCP::TCP(Server &server){ // Constructor that starts server listening on address and port
	struct sockaddr_in sa;
    int rc;
    this->server = &server;

        
	if ((this->welcome_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){ // Create welcome socket
		error_exit(99);
	}
	
    int enable = 1;
    setsockopt(welcome_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

	memset(&sa,0,sizeof(sa));
	sa.sin_family = AF_INET;
    inet_pton(AF_INET, this->server->gethost().c_str(), &sa.sin_addr);
	sa.sin_port = htons(this->server->getport());	
        
    
    
	if ((rc = bind(this->welcome_socket, (struct sockaddr*)&sa, sizeof(sa))) < 0){ // Bind socket to address
		error_exit(99);
	}

	if ((listen(this->welcome_socket, 1)) < 0){ // Start listening
		error_exit(99);
	}
}

void TCP::await_connection(){ // Method to establish connections with clients
	struct sockaddr_in sa_client;
	socklen_t sa_client_len = sizeof(sa_client);

    struct sigaction sigIntHandler;
    set_cc_handler(&sigIntHandler);

	while(1){
		int comm_socket = accept(this->welcome_socket, (struct sockaddr*)&sa_client, &sa_client_len);

        if (force_quit){ // C-c handling
            for (int &socket : this->conn_clients){
                send(socket, "BYE\n", strlen("BYE\n"), 0);
                shutdown(socket, SHUT_RDWR);
            }

            for (size_t i = 0; i < this->threads.size(); i++){
                if (this->threads[i].joinable()) this->threads[i].join();
            }
            break;
        }

		if (comm_socket > 0){ // Create thread for client and handle him in it
            this->conn_clients_mutex.lock();
            this->conn_clients.push_back(comm_socket);
			std::thread t(TCPHandleClient(), &this->conn_clients, comm_socket, &this->conn_clients_mutex);
            this->threads.push_back(std::move(t));
            this->conn_clients_mutex.unlock();
		}
	}
    close(this->welcome_socket);
}
