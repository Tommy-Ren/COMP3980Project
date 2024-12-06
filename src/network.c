#include "../include/network.h"

struct network *openNetworkSocketServer(const char *ip_address, in_port_t port, int *err)
{
    struct network *ctx = malloc(sizeof(struct network));
    Player          temp;

    if(ctx == NULL)
    {
        perror("Memory allocation for network context failed!");
        *err = errno;
        return NULL;
    }

    ctx->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(ctx->sockfd < 0)
    {
        perror("Socket creation failed");
        *err = errno;
        cleanup_network(ctx);
        return NULL;
    }

    setupNetworkAddress(&(ctx->server_addr), &(ctx->server_addr_len), ip_address, port, err);
    if(*err != 0)
    {
        cleanup_network(ctx);
        return NULL;
    }

    if(bind(ctx->sockfd, (struct sockaddr *)&(ctx->server_addr), ctx->server_addr_len) < 0)
    {
        perror("Bind failed");
        *err = errno;
        cleanup_network(ctx);
        return NULL;
    }

    printf("Server initialized and waiting for client...\n");

    ctx->client_addr_len = sizeof(ctx->client_addr);

    if(recvfrom(ctx->sockfd, &temp, sizeof(temp), 0, (struct sockaddr *)&(ctx->client_addr), &(ctx->client_addr_len)) < 0)
    {
        perror("Failed to receive initial message");
        *err = errno;
        cleanup_network(ctx);
        return NULL;
    }

    printf("Client connected! Address: %s\n", inet_ntoa(((struct sockaddr_in *)&ctx->client_addr)->sin_addr));

    return ctx;
}

// Function to open client network socket
struct network *openNetworkSocketClient(const char *ip_address, in_port_t port, int *err)
{
    struct network *ctx  = malloc(sizeof(struct network));
    Player          temp = {0};

    if(ctx == NULL)
    {
        perror("Failed to allocate memory for network context");
        *err = errno;
        return NULL;
    }

    ctx->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(ctx->sockfd < 0)
    {
        perror("Socket creation failed");
        *err = errno;
        cleanup_network(ctx);
        return NULL;
    }

    setupNetworkAddress(&(ctx->client_addr), &(ctx->client_addr_len), ip_address, port, err);
    if(*err != 0)
    {
        cleanup_network(ctx);
        return NULL;
    }

    if(sendto(ctx->sockfd, &temp, sizeof(Player), 0, (struct sockaddr *)&(ctx->client_addr), ctx->client_addr_len) < 0)
    {
        perror("Failed to send initial message");
        *err = errno;
        cleanup_network(ctx);
        return NULL;
    }

    printf("Connected to server! Address: %s\n", inet_ntoa(((struct sockaddr_in *)&ctx->client_addr)->sin_addr));
    return ctx;
}

// Set up address structure
void setupNetworkAddress(struct sockaddr_storage *addr, socklen_t *addr_len, const char *address, in_port_t port, int *err)
{
    in_port_t net_port = htons(port);
    memset(addr, 0, sizeof(struct sockaddr_storage));
    *err = 0;

    // Try to interpret the address as IPv4
    if(inet_pton(AF_INET, address, &((struct sockaddr_in *)addr)->sin_addr) == 1)
    {
        struct sockaddr_in *ipv4_addr = (struct sockaddr_in *)addr;
        ipv4_addr->sin_family         = AF_INET;
        ipv4_addr->sin_port           = net_port;
        *addr_len                     = sizeof(struct sockaddr_in);
    }
    // If IPv4 fails, try interpreting it as IPv6
    else if(inet_pton(AF_INET6, address, &((struct sockaddr_in6 *)addr)->sin6_addr) == 1)
    {
        struct sockaddr_in6 *ipv6_addr = (struct sockaddr_in6 *)addr;
        ipv6_addr->sin6_family         = AF_INET6;
        ipv6_addr->sin6_port           = net_port;
        *addr_len                      = sizeof(struct sockaddr_in6);
    }
    // If neither IPv4 nor IPv6, set an error
    else
    {
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

void send_game_state(struct network *ctx, const Player *player, const Bullet *bullets, int *game_state)
{
    // Calculate the size of the data to be sent
    char *buffer = malloc(BUFFER_SIZE);

    // Check if buffer created successfully
    if(buffer == NULL)
    {
        fprintf(stderr, "Buffer allocation failed!\n");
        return;
    }

    // Initialize the buffer with 0s
    memset(buffer, 0, BUFFER_SIZE);

    // Copy player state to the buffer
    memcpy(buffer, player, sizeof(Player));

    // Copy bullets to the buffer after player data
    memcpy(buffer + sizeof(Player), bullets, MAX_BULLETS * sizeof(Bullet));

    // Extract game state from the buffer
    memcpy(buffer + sizeof(Player) + MAX_BULLETS * sizeof(Bullet), game_state, sizeof(*game_state));

    // Send the buffer to the peer
    if(sendto(ctx->sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&ctx->client_addr, ctx->client_addr_len) < 0)
    {
        perror("Failed to send game state");
    }

    // Free the buffer after use
    free(buffer);
}

void receive_game_state(struct network *ctx, Player *player, Bullet *bullets, int *game_state)
{
    char *buffer = malloc(BUFFER_SIZE);

    // Check if buffer created successfully
    if(buffer == NULL)
    {
        fprintf(stderr, "Buffer allocation failed!\n");
        return;
    }
    memset(buffer, 0, BUFFER_SIZE);

    // Receive data from the peer
    if(recvfrom(ctx->sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL) < 0)
    {
        perror("Failed to receive game state");
        return;
    }

    // Extract player state from the buffer
    memcpy(player, buffer, sizeof(Player));

    // Extract bullets from the buffer
    memcpy(bullets, buffer + sizeof(Player), MAX_BULLETS * sizeof(Bullet));

    // Extract game state from the buffer
    memcpy(game_state, buffer + sizeof(Player) + MAX_BULLETS * sizeof(Bullet), sizeof(*game_state));

    // Free buffer
    free(buffer);
}

// Close network socket
void close_network(int sockfd)
{
    if(sockfd >= 0)
    {
        close(sockfd);
    }
}

// Free all network resouces
void cleanup_network(struct network *ctx)
{
    if(ctx)
    {
        if(ctx->sockfd >= 0)
        {
            close(ctx->sockfd);
        }
        free(ctx);
    }
}
