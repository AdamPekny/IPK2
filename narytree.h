/*
    File: narytree.h
    Author: Adam Pekný, xpekny00
    Description: Interface for nary tree module
*/

#include <vector>

#include "tokenizer.h"

#ifndef NARYTREE_H
#define NARYTREE_H

class Node {
    private:
        Token token;
        std::vector<Node*> children;
    public:
        Node() = default;
        Node(Token token);
        
        Node *add_child(Token token);
        Token get_token();
        std::vector<Node*> get_children();
        bool has_children();

        void print_node();
};

#endif
