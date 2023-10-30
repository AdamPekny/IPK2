/*
    File: error.cpp
    Author: Adam Pekný, xpekny00
    Description: File responsible for exitting with desired error code and printing error message to stderr
*/

#include <iostream>


using namespace std;
void error_exit(int err_code){
    cerr << "ERROR: ";
    switch (err_code){
        /*Common errors*/
        case 10:
            cerr << "Invalid or unknown argument!";
            break;
        /*Internal errors*/
        case 99:
            cerr << "Internal error!";
            break;
        
        default:
            break;
    }

    cerr << "\n";
    cerr << "Run ./ipkcpd --help for help!\n";
    exit(err_code);
}
