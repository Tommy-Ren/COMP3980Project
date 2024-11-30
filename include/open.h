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
#include <arpa/inet.h>
#include <unistd.h>

#include "../include/player.h"

#define BUFFER_SIZE sizeof(Player)
#define ERR_NONE 0
#define ERR_NO_DIGITS 1
#define ERR_OUT_OF_RANGE 2
#define ERR_INVALID_CHARS 3

// Declare network content
struct network {
    int sockfd;
    struct sockaddr_storage local_addr;
    struct sockaddr_storage peer_addr;
    socklen_t local_addr_len;
    socklen_t peer_addr_len;
};

// Function to open keyboard and screen
int openKeyboard(void);
int openStdout(void);
int openStderr(void);

// Function to open network socket
void openNetworkSocketServer(const char *ip_address, in_port_t port, int *err);
void openNetworkSocketClient(const char *ip_address, in_port_t port, int *err);
void setupNetworkAddress(struct sockaddr_storage *addr, socklen_t *addr_len, const char *address, in_port_t port, int *err);

// Convert port if network socket as input
in_port_t convertPort(const char *str, int *err);

// For send and receiver player position
void send_player_position(const Player *player);
void receive_player_position(Player *player);

#endif    // OPEN_H
