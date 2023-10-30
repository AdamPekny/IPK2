/*
    File: narytree.cpp
    Author: Adam Pekný, xpekny00
    Description: Module for operations on n-ary tree that stores expression
*/


#include <iostream>
#include <vector>

#include "tokenizer.h"

 
class Node { // Class to contain tree and its nodes
    private:
        Token token;
        std::vector<Node*> children;
    public:
        Node() = default;
        Node(Token token);
        ~Node();
        
        Node *add_child(Token token);
        Token get_token();
        std::vector<Node*> get_children();
        bool has_children();

        void print_node();
};

Node::Node(Token token){ // Constructor from token
    this->token = token;
}

Node::~Node(){ // Destructor
    for (Node *child : children){
        if (child != nullptr) delete(child);
    }
}

Node *Node::add_child(Token token){ // Method to add child to node
    Node *new_node = new Node(token);
    this->children.push_back(new_node);

    return new_node;
}

Token Node::get_token(){ // Method to retrieve token
    return this->token;
}

std::vector<Node*> Node::get_children(){ // Method to retrieve children of a node
    return this->children;
}

bool Node::has_children(){ // Method to check if node has children
    return this->children.size() > 0;
}

void Node::print_node(){ // Debug method to print nodes token value
    Token token = this->token;
    if (token.get_strval().has_value()){
        std::cout << token.get_strval().value();
    } else {
        std::cout << token.get_intval().value();
    }
    std::cout << std::endl;
}
