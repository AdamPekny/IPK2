/*
    File: interrupt.h
    Author: Adam Pekný, xpekny00
    Description: Interface file for interrupt module
*/

#ifndef INTERRUPT_H
#define INTERRUPT_H
// Flag indicating C-c
extern int force_quit;

void set_cc_handler(struct sigaction *sigIntHandler);

#endif
