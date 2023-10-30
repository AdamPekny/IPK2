/*
    File: tokenizer.cpp
    Author: Adam Pekný, xpekny00
    Description: Module responsible for parsing payload to tokens and vaidating lexical analysis
*/

#include <iostream>
#include <string>
#include <optional>
#include <ctype.h>


// Types of tokens
enum TokenType {L_PAR, R_PAR, OPERATOR, OPERAND, SPACE, MSGEND};

class Token{ // Class to hold token data and methods to work with it
    private:
        TokenType type;
        std::optional<std::string> strval;
        std::optional<size_t> intval;

    public:
        Token() = default;
        Token(std::string value, TokenType type);

        TokenType get_type();
        std::optional<std::string> get_strval();
        std::optional<size_t> get_intval();
        std::optional<long> eval_operator(long arg1 ,long arg2);

        class IntConversionException : std::exception {};
        class ZeroDivisionException : std::exception {};
};

Token::Token(std::string value, TokenType type){
    if (type == OPERAND){
        char *endptr;
        size_t tmp = strtoul(value.c_str(), &endptr, 10);

        if (*endptr != '\0') throw Token::IntConversionException();

        this->intval = tmp;
    } else {
        this->strval = value;
    }

    this->type = type;  
}

TokenType Token::get_type(){
    return this->type;
}

std::optional<std::string> Token::get_strval(){
    return this->strval;
}

std::optional<size_t> Token::get_intval(){
    return this->intval;
} 

std::optional<long> Token::eval_operator(long arg1 ,long arg2){ // Method to evaluate token operator if it is OPERATOR token
    std::optional<long> result;

    if (this->type != OPERATOR) return result;

    switch (this->strval.value().c_str()[0]){
    case '+':
        result = arg1 + arg2;
        break;
    case '-':
        result = arg1 - arg2;
        break;
    case '*':
        result = arg1 * arg2;
        break;
    case '/':
        if (arg2 == 0) throw Token::ZeroDivisionException();
        result = arg1 / arg2;
        break;
    default:
        break;
    }

    return result;
}


// Tokenizer to parse payload to tokens
enum TokenizerStates {START_S, OPERAND_S};

class Tokenizer{
    private:
        std::string message;
        size_t char_idx;

        bool cis_operator(char c);

    public:
        Tokenizer() = default;
        Tokenizer(std::string message);

        Token get_token();

        class LexemeException : std::exception {};
        class StateException : std::exception {};
};

Tokenizer::Tokenizer(std::string message){
    this->message = message;
    this->char_idx = 0;
}

bool Tokenizer::cis_operator(char c){ // Method to decide if character is operator
    bool result = false;
    switch (c){
        case '+':
        case '-':
        case '*':
        case '/':
            result = true;
            break;

        default:
            break;
    }

    return result;
}

Token Tokenizer::get_token(){ // Method to retireve next token (automatically moves to next one in next call)
    Token new_token;
    std::string token_value = "";
    TokenType token_type = MSGEND;
    TokenizerStates current_state = START_S;
    char current_char;
    
    while (this->char_idx < this->message.length()){
        current_char = this->message[this->char_idx];
        if (current_state == START_S){
            if (current_char == '('){
                token_type = L_PAR;
            } else if (current_char == ')'){
                token_type = R_PAR;
            } else if (current_char == ' '){
                token_type = SPACE;
            } else if (this->cis_operator(current_char)){
                token_type = OPERATOR;
            } else if (isdigit(current_char)){
                current_state = OPERAND_S;
            } else {
                throw Tokenizer::LexemeException();
            }

            token_value.push_back(current_char);
            this->char_idx++;
            if (token_type != MSGEND){
                break;
            }
        } else if (current_state == OPERAND_S){
            if (isdigit(current_char)){
                token_value.push_back(current_char);
                this->char_idx++;
            } else {
                token_type = OPERAND;
                break;
            }
        } else {
            throw Tokenizer::StateException();
        }

    }
    
    try{
        new_token = Token(token_value, token_type);
    }
    catch(const Token::IntConversionException &e){
        throw Tokenizer::LexemeException();
    }

    return new_token;
}



