/*
    File: payload_parser.h
    Author: Adam Pekný, xpekny00
    Description: Interface for parser module
*/

#include <string>
#include <stack>

#include "narytree.h"
#include "tokenizer.h"

#ifndef PARSER_H
#define PARSER_H

class PayloadParser{
    private:
        std::string mode;
        Tokenizer tokenizer;
        Node expr_tree_root;

        bool parse_payload(Token start_token, Node *parent_node);
        bool parse_expr(Token start_token, Node *parent_node);
        bool parse_args(Token start_token, Node *parent_node);
        bool parse_margs(Token start_token, Node *parent_node);
        long eval_tree(Node *root);

    public:
        PayloadParser() = default;
        PayloadParser(std::string payload);

        void parse();
        long calc();

        class LexException : std::exception {};
        class SyntaxException : std::exception {};
        class CalcException : std::exception {};
};

#endif
