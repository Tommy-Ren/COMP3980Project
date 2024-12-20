//
// Created by tommy on 11/25/24.
//

#ifndef NETWORK_H
#define NETWORK_H

#include "object.h"
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

#define BUFFER_SIZE (sizeof(Player) + (MAX_BULLETS * sizeof(Bullet)) + sizeof(int))
#define PORT 12345
#define ERR_NONE 0
#define ERR_NO_DIGITS 1
#define ERR_OUT_OF_RANGE 2
#define ERR_INVALID_CHARS 3

// Declare network content
struct network
{
    int                     sockfd;
    struct sockaddr_storage server_addr;
    struct sockaddr_storage client_addr;
    socklen_t               server_addr_len;
    socklen_t               client_addr_len;
};

// Function to open network socket
struct network *openNetworkSocketServer(const char *ip_address, in_port_t port, int *err);
struct network *openNetworkSocketClient(const char *ip_address, in_port_t port, int *err);
void            setupNetworkAddress(struct sockaddr_storage *addr, socklen_t *addr_len, const char *address, in_port_t port, int *err);

// Convert port if network socket as input
in_port_t convertPort(const char *str, int *err);

// Function to handle game state
void send_game_state(struct network *ctx, const Player *player, const Bullet *bullets, int *game_state);
void receive_game_state(struct network *ctx, Player *player, Bullet *bullets, int *game_state);

// Close the socket
void close_network(int sockfd);
void cleanup_network(struct network *ctx);

#endif    // NETWORK_H
