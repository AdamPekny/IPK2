/*
    File: tcpmessage.cpp
    Author: Adam Pekný, xpekny00
    Description: Module responsible for parsing tcp messages
*/

#include <string>
#include <cstring>
#include <vector>

#include <iostream>

#define TCPBUFSIZE 2048


class TCPMessage{ // Class to hold message data and methods to work with it
    private:
        char buf[TCPBUFSIZE + 1] = {0}; // +1 to always ensure null terminator
        std::string query;

        size_t payload_starti();

    public:
        TCPMessage() = default;

        void concat_buf();
        bool load_cmd(std::string &cmd);
        bool set_buf(std::string message);
        bool set_keyword(std::string keyword);
        bool set_payload(std::string payload);

        char *get_buf();
        std::string get_keyword();
        std::string get_payload();
        size_t get_bufsize();
        void buf_bzero();

        bool validate_keyword_buf();
        bool validate_query_buf();
};

size_t TCPMessage::payload_starti(){ // Get index of payload start
    size_t starti = 0;

    for (char &c : this->buf){
        if (c == ' ' || c == '\n') break;
        
        starti++;
    }

    return starti + 1;
}

void TCPMessage::concat_buf(){ // Concatenate buffer to query
    this->query += this->buf;
    this->buf_bzero();
}

bool TCPMessage::load_cmd(std::string &cmd){ // Load command from query to cmd
    cmd = "";
    for (size_t i = 0; i < this->query.size(); i++){
        cmd.push_back(this->query[i]);
        if (this->query[i] == '\n'){
            this->query = this->query.substr(i + 1, this->query.length() - i - 1);
            return true;
        }
    }
    return false;
}

bool TCPMessage::set_buf(std::string message){ // Set whole buffer with message
    if (message.length() > TCPBUFSIZE) return false;

    bzero(this->buf, TCPBUFSIZE);
    std::memcpy(this->buf, message.c_str(), message.length());
    
    return true;
}

bool TCPMessage::set_keyword(std::string keyword){ // Set keyword of buffer (should be used after resetting buffer)
    if (keyword != "HELLO" && keyword != "BYE" && keyword != "SOLVE" && keyword != "RESULT") return false;

    std::memcpy(this->buf, keyword.c_str(), keyword.length());
    this->buf[keyword.length()] = '\n';

    return true;
}

bool TCPMessage::set_payload(std::string payload){ // Set payload in buffer (should be used after setting keyword)
    size_t payl_starti = this->payload_starti();
    if (payload.length() > TCPBUFSIZE - payl_starti) return false;

    char *payload_startp = this->buf + payl_starti;
    this->buf[payl_starti - 1] = ' ';
    std::memcpy(payload_startp, payload.c_str(), payload.length());
    this->buf[payl_starti + payload.length()] = '\n';

    return true;
}

char *TCPMessage::get_buf(){ // Retrieve buffer
    return this->buf;
}

std::string TCPMessage::get_keyword(){ // Retrieve keyword from buffer
    char keyword_buf[TCPBUFSIZE + 1];
    bzero(keyword_buf, TCPBUFSIZE);

    std::memcpy(keyword_buf, this->buf, this->payload_starti() - 1);

    return std::string(keyword_buf);
}

std::string TCPMessage::get_payload(){ // Retrieve payload from buffer
    return std::string(this->buf + this->payload_starti());
}

size_t TCPMessage::get_bufsize(){ // Retrieve buffer max size
    return TCPBUFSIZE;
}

void TCPMessage::buf_bzero(){ // Reset buffer
    bzero(this->buf, TCPBUFSIZE + 1);
}

bool TCPMessage::validate_keyword_buf(){ // Validate buffer against keyword command
    if (this->get_keyword() != "HELLO" && this->get_keyword() != "BYE") return false;

    if (this->get_keyword().length() != std::strlen(this->buf) - 1) return false;

    if (this->buf[std::strlen(this->buf) - 1] != '\n') return false;

    return true;
}

bool TCPMessage::validate_query_buf(){ // Validate buffer against command with query
    if (this->get_keyword() != "SOLVE") return false;

    if (this->get_keyword().length() + this->get_payload().length() != std::strlen(this->buf) - 1) return false;

    if (this->buf[std::strlen(this->buf) - 1] != '\n') return false;

    return true;
}
