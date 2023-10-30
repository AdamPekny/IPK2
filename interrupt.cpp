/*
    File: interrupt.cpp
    Author: Adam Pekný, xpekny00
    Description: File responsible for interrupt signal handling
*/

#include <iostream>

#include <signal.h>

#include "interrupt.h"

// Handler function for C-c
void cc_handler(sig_atomic_t sig){
    if (sig == SIGINT){
        force_quit = 1;
    }
}

void set_cc_handler(struct sigaction *sigIntHandler){
    // Initialize struct for C-c signal handling
    sigIntHandler->sa_handler = cc_handler;
    sigemptyset(&sigIntHandler->sa_mask);
    sigIntHandler->sa_flags = 0;

    sigaction(SIGINT, sigIntHandler, NULL);
}
