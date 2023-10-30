/*
    File: udpmessage.cpp
    Author: Adam Pekný, xpekny00
    Description: Module responsible parsing udp messages
*/

#include <string>
#include <cstring>

#define UDPBUFSIZE 258


class UDPMessage{
    private:
        char buf[UDPBUFSIZE + 1];

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

char *UDPMessage::payload_start(){ // Poiter to start of payload
    return (this->get_opcode() ? this->buf + 3 : this->buf + 2);
}

size_t UDPMessage::payload_max(){ // Max length of payload
    return (this->get_opcode() ? UDPBUFSIZE - 3 : UDPBUFSIZE - 2);
}

void UDPMessage::set_payloadlen(uint8_t len){ // Set stated payload length
    if (this->get_opcode()){
        this->buf[2] = (char) len;
    } else {
        this->buf[1] = (char) len;
    }
}

bool UDPMessage::set_buf(std::string message){ // Set content of whole buffer with message
    if (message.length() > UDPBUFSIZE) return false;

    bzero(this->buf, UDPBUFSIZE);
    std::memcpy(this->buf, message.c_str(), message.length());
    
    return true;
}

bool UDPMessage::set_payload(std::string payload){ // Set payload in buffer
    if (payload.length() > this->payload_max()) return false;

    bzero(this->payload_start(), this->payload_max());    
    std::memcpy(this->payload_start(), payload.c_str(), payload.length());

    this->set_payloadlen(payload.length());
    
    return true;
}

bool UDPMessage::set_opcode(uint8_t opcode){ // Set opcode (can be 0 or 1)
    if (opcode != 0 && opcode != 1) return false;

    this->buf[0] = opcode;

    return true;
}

bool UDPMessage::set_statuscode(uint8_t status_code){ // Set status code
    if (!this->get_opcode()) return false;

    this->buf[1] = status_code;

    return true;
}

char *UDPMessage::get_buf(){
    return this->buf;
}

std::string UDPMessage::get_payload(){
    return std::string(this->payload_start());
}

uint8_t UDPMessage::get_payloadlen(){
    return (uint8_t) (this->get_opcode() ? this->buf[2] : this->buf[1]);
}

uint8_t UDPMessage::get_opcode(){
    return (uint8_t) this->buf[0];
}

uint8_t UDPMessage::get_statuscode(){
    if (!this->get_opcode()) return 3;

    return this->buf[1];
}

size_t UDPMessage::get_bufsize(){
    return UDPBUFSIZE;
}

void UDPMessage::buf_bzero(){
    bzero(this->buf, UDPBUFSIZE + 1);
}
