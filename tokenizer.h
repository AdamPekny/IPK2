/*
    File: tokenizer.h
    Author: Adam Pekný, xpekny00
    Description: Interface for tokenizer module
*/

#include <string>
#include <optional>


#ifndef TOKENIZER_H
#define TOKENIZER_H

enum TokenType {L_PAR, R_PAR, OPERATOR, OPERAND, SPACE, MSGEND};


class Token{
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

#endif
