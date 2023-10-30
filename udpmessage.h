/*
    File: udpmessage.h
    Author: Adam Pekný, xpekny00
    Description: Interface for udpmessage module
*/

#include <string>


#ifndef UDPMESSAGE_H
#define UDPMESSAGE_H

#define UDPBUFSIZE 258


class UDPMessage{
    private:
        char buf[UDPBUFSIZE + 1];
        size_t bufsize;

        char *payload_start();
        size_t payload_max();
        void set_payloadlen(uint8_t len);
    public:
        UDPMessage() = default;

        bool set_buf(std::string message);
        bool set_payload(std::string payload);
        bool set_opcode(uint8_t opcode);
        bool set_statuscode(uint8_t status_code);

        char *get_buf();
        std::string get_payload();
        uint8_t get_payloadlen();
        uint8_t get_opcode();
        uint8_t get_statuscode();
        size_t get_bufsize();
        void buf_bzero();
};

#endif
