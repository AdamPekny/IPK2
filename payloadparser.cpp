/*
    File: payload_parser.cpp
    Author: Adam Pekný, xpekny00
    Description: Module for parsing payload expression
*/

#include <iostream>
#include <string>
#include <vector>

#include "narytree.h"
#include "tokenizer.h"


/*
GRAMMAR:
start -> payload MSGEND
payload -> L_PAR OPERATOR <args>
args -> SPACE <expr> SPACE <expr> <margs>
expr -> OPERAND | <payload>
margs -> R_PAR | SPACE <expr> <margs>
*/

class PayloadParser{ // Class for parsing and calculating payload (expression)
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

bool PayloadParser::parse_payload(Token start_token, Node *parent_node){ // Method to parse payload
    // payload -> L_PAR OPERATOR <args> 
    Token token = start_token;
    if (token.get_type() != L_PAR) return false;
    
    token = this->tokenizer.get_token();
    if (token.get_type() != OPERATOR) return false;
    Node *current_node = parent_node->add_child(token);

    if (!this->parse_args(this->tokenizer.get_token(), current_node)) return false;

    return true;
}

bool PayloadParser::parse_args(Token start_token, Node *parent_node){ // Method to parse args
    // args -> SPACE <expr> SPACE <expr> <margs>
    Token token = start_token;
    if (token.get_type() != SPACE) return false;

    if (!this->parse_expr(this->tokenizer.get_token(), parent_node)) return false;

    token = this->tokenizer.get_token();
    if (token.get_type() != SPACE) return false;

    if (!this->parse_expr(this->tokenizer.get_token(), parent_node)) return false;
    
    if (!this->parse_margs(this->tokenizer.get_token(), parent_node)) return false;

    return true;
}

bool PayloadParser::parse_expr(Token start_token, Node *parent_node){ // Method to parse expression
    // expr -> OPERAND | <payload>
    Token token = start_token;
    if (token.get_type() == OPERAND){
        parent_node->add_child(token);
        return true;
    }

    if (!this->parse_payload(token, parent_node)) return false;

    return true;
}

bool PayloadParser::parse_margs(Token start_token, Node *parent_node){ // Method to parse more args
    // margs -> R_PAR | SPACE <expr> <margs>
    Token token = start_token;
    if (token.get_type() == R_PAR) return true;

    if (token.get_type() != SPACE) return false;

    if (!this->parse_expr(this->tokenizer.get_token(), parent_node)) return false;

    if (!this->parse_margs(this->tokenizer.get_token(), parent_node)) return false;

    return true;
}

PayloadParser::PayloadParser(std::string payload){ // Constructor that initializes parser
    this->tokenizer = Tokenizer(payload);
    Token root_token = Token("+", OPERATOR);
    this->expr_tree_root = Node(root_token);
}

void PayloadParser::parse(){ // Method to parse payload to n-ary tree and check syntax and lexical errors
    // start -> payload MSGEND
    try{
        Token token = this->tokenizer.get_token();
        if (!this->parse_payload(token, &this->expr_tree_root)) throw PayloadParser::SyntaxException();
    
        token = this->tokenizer.get_token();
        if (token.get_type() != MSGEND) throw PayloadParser::SyntaxException();
    } catch (const Tokenizer::LexemeException &e){
        throw PayloadParser::LexException();
    }

}

long PayloadParser::eval_tree(Node *root){ // Method to recursively evaluate nary tree
    // If there is nothing more to evaluate return value
    if (root->get_token().get_type() == OPERAND) return root->get_token().get_intval().value();

    long result;
    size_t arg1;
    size_t arg2;

    // If there is only one operand throw error
    if (root->get_children().size() < 2) throw PayloadParser::CalcException();

    if (root->get_children()[0]->get_token().get_type() == OPERAND){ // First operand is nummeric
        arg1 = root->get_children()[0]->get_token().get_intval().value();
    } else { // First operand is expression
        arg1 = this->eval_tree(root->get_children()[0]);
    }

    if (root->get_children()[1]->get_token().get_type() == OPERAND){ // Second operand is nummeric
        arg2 = root->get_children()[1]->get_token().get_intval().value();
    } else { // Second operand is expression
        arg2 = this->eval_tree(root->get_children()[1]);
    }

    // Get result of first two operands
    result = root->get_token().eval_operator(arg1, arg2).value();


    for (size_t i = 2; i < root->get_children().size(); i++){ // Evaluate for the rest of operands
        if (root->get_children()[i]->get_token().get_type() == OPERAND){ // Operand is nummeric 
            result = root->get_token().eval_operator(result, root->get_children()[i]->get_token().get_intval().value()).value();
        } else { // Operand is expression
            result = root->get_token().eval_operator(result, this->eval_tree(root->get_children()[i])).value();
        }
    }

    return result;
}

long PayloadParser::calc(){ // Method to start payload evaluation
    Node *root = this->expr_tree_root.get_children()[0];
    
    long result;

    try{
        result = this->eval_tree(root);
    }
    catch (const Token::ZeroDivisionException &e){
        throw PayloadParser::CalcException();
    }

    if (result < 0) throw PayloadParser::CalcException();
    

    return result;
}
