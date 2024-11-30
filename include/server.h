//
// Created by Kiet on 05/10/2024.
//

#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BACKLOG 5
#define PORT 9999
#define ERR_NONE 0

// Global variable
static int is_server;

// Declare type of option
struct options
{
    // For start game
    bool client;
    bool server;
    
    // For user input
    char *input;

    // For network socket
    char     *inaddress;
    char     *outaddress;
    in_port_t inport;
    in_port_t outport;
};

#endif    // SERVER_H
