#include "../include/open.h"

int openKeyboard(void)
{
    return STDIN_FILENO;
}

int openStdout(void)
{
    return STDOUT_FILENO;
}

int openStderr(void)
{
    return STDERR_FILENO;
}

// Function to open server network socket
void openNetworkSocketServer(const char *ip_address, in_port_t port, int *err) {
    struct network ctx;
    ctx.sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (ctx.sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up local address structure
    setupNetworkAddress(&(ctx.local_addr), &(ctx.local_addr_len), ip_address, port, err);
    if (*err != 0) {
        close(ctx.sockfd);
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the local address
    if (bind(ctx.sockfd, (struct sockaddr *)&(ctx.local_addr), ctx.local_addr_len) < 0) {
        perror("Bind failed");
        close(ctx.sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Server initialized and waiting for client...\n");

    // Wait for the first message to get the client's address
    Player temp;
    if (recvfrom(ctx.sockfd, &temp, BUFFER_SIZE, 0, (struct sockaddr *)&(ctx.peer_addr), &(ctx.peer_addr_len)) < 0) {
        perror("Failed to receive initial message");
        close(ctx.sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Client connected!\n");
}

// Function to open client network socket
void openNetworkSocketClient(const char *ip_address, in_port_t port, int *err) {
    struct network ctx;
    ctx.sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (ctx.sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up peer address structure
    setupNetworkAddress(&(ctx.peer_addr), &(ctx.peer_addr_len), ip_address, port, err);
    if (*err != 0) {
        close(ctx.sockfd);
        exit(EXIT_FAILURE);
    }

    // Send an initial message to the server
    Player temp = {0, 0};
    if (sendto(ctx.sockfd, &temp, BUFFER_SIZE, 0, (struct sockaddr *)&(ctx.peer_addr), ctx.peer_addr_len) < 0) {
        perror("Failed to send initial message");
        close(ctx.sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server!\n");
}

// Set up address structure
void setupNetworkAddress(struct sockaddr_storage *addr, socklen_t *addr_len, const char *address, in_port_t port, int *err) {
    in_port_t net_port = htons(port);
    memset(addr, 0, sizeof(struct sockaddr_storage));
    *err = 0;

    // Try to interpret the address as IPv4
    if (inet_pton(AF_INET, address, &((struct sockaddr_in *)addr)->sin_addr) == 1) {
        struct sockaddr_in *ipv4_addr = (struct sockaddr_in *)addr;
        ipv4_addr->sin_family = AF_INET;
        ipv4_addr->sin_port = net_port;
        *addr_len = sizeof(struct sockaddr_in);
    }
    // If IPv4 fails, try interpreting it as IPv6
    else if (inet_pton(AF_INET6, address, &((struct sockaddr_in6 *)addr)->sin6_addr) == 1) {
        struct sockaddr_in6 *ipv6_addr = (struct sockaddr_in6 *)addr;
        ipv6_addr->sin6_family = AF_INET6;
        ipv6_addr->sin6_port = net_port;
        *addr_len = sizeof(struct sockaddr_in6);
    }
    // If neither IPv4 nor IPv6, set an error
    else {
        fprintf(stderr, "%s is not a valid IPv4 or IPv6 address\n", address);
        *err = errno;
    }
}

// Function to convert port if network socket as type
in_port_t convertPort(const char *str, int *err)
{
    in_port_t port;
    char     *endptr;
    long      val;

    *err  = ERR_NONE;
    port  = 0;
    errno = 0;
    val   = strtol(str, &endptr, 10);    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    // Check if no digits were found
    if(endptr == str)
    {
        *err = ERR_NO_DIGITS;
        return port;
    }

    // Check for out-of-range errors
    if(val < 0 || val > UINT16_MAX)
    {
        *err = ERR_OUT_OF_RANGE;
        return port;
    }

    // Check for trailing invalid characters
    if(*endptr != '\0')
    {
        *err = ERR_INVALID_CHARS;
        return port;
    }

    port = (in_port_t)val;
    return port;
}

// Function to send player position
void send_player_position(const Player *player) {
    struct network ctx;
    if (sendto(ctx.sockfd, player, BUFFER_SIZE, 0, (struct sockaddr *)&(ctx.peer_addr), ctx.peer_addr_len) < 0) {
        perror("Failed to send player position");
    }
}

// Function to receive player position
void receive_player_position(Player *player) {
    struct network ctx;
    if (recvfrom(ctx.sockfd, player, BUFFER_SIZE, 0, NULL, NULL) < 0) {
        perror("Failed to receive player position");
    }
}