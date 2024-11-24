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

// Declare type of option
struct options
{
    // For network socket
    char     *inaddress;
    char     *outaddress;
    in_port_t inport;
    in_port_t outport;
};

// Get the data to command-line
static void           parseArguments(int argc, char **argv, struct options *opts);
static void           handleArguments(const char *filter, char (**filter_fun)(char));
_Noreturn static void usage(const char *program_name, int exit_code, const char *message);
static int            getInput(const struct options *opts, int *err);

// Run server and Create process
_Noreturn void runServer(int fd_server, char (*filter_fun)(char), int err);
void           createProcess(int fd_client, char (*filter_fun)(char), int *err);
int            handleClient(int fd_client, char (*filter_fun)(char), int *err);

#endif    // SERVER_H
