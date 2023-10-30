/*
    File: tcpmessage.h
    Author: Adam Pekný, xpekny00
    Description: Interface for TCP messages
*/

#include <string>


#ifndef TCPMESSAGE_H
#define TCPMESSAGE_H

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

#endif
