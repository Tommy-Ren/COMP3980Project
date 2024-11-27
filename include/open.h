//
// Created by Kiet on 03/11/2024.
//

#ifndef OPEN_H
#define OPEN_H

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define ERR_NONE 0
#define ERR_NO_DIGITS 1
#define ERR_OUT_OF_RANGE 2
#define ERR_INVALID_CHARS 3

// Function to open keyboard and screen
int openKeyboard(void);
int openStdout(void);
int openStderr(void);

// Function to setup network socket
int  openNetworkSocketClient(const char *address, in_port_t port, int *err);
int  openNetworkSocketServer(const char *address, in_port_t port, int backlog, int *err);
void setupNetworkAddress(struct sockaddr_storage *addr, socklen_t *addr_len, const char *address, in_port_t port, int *err);
int  acceptConnection(const struct sockaddr_storage *addr, socklen_t addr_len, int backlog, int *err);
int  connectToServer(struct sockaddr_storage *addr, socklen_t addr_len, int *err);

// Convert port if network socket as input
in_port_t convertPort(const char *str, int *err);

#endif    // OPEN_H
