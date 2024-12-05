//
// Created by tommy and Kiet on 11/25/24.
//

#include "../include/game.h"
#include "../include/network.h"
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

#define PORT 12345
#define ERR_NONE 0

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

// Helper functions dealing with arguments
static void           parse_arguments(int argc, char **argv, struct options *opts);
_Noreturn static void usage(const char *program_name, int exit_code, const char *message);
static void           check_arguments(const char *program_name, const struct options *opts);

int main(int argc, char *argv[])
{
    // Initialize variable
    struct options opts;
    int            err;
    bool           is_server;

    // Set each member of options to 0
    err       = 0;
    is_server = 0;
    memset(&opts, 0, sizeof(opts));

    // Set inport and outport value
    opts.inport  = PORT;
    opts.outport = PORT;

    // Sending data to the client
    parse_arguments(argc, argv, &opts);

    // Checking the arguments of command-line
    check_arguments(argv[0], &opts);

    // Check if the game start as server or client
    if(opts.server == true)
    {
        is_server = 1;
    }

    // Start the game
    if(opts.inaddress != NULL || opts.outaddress != NULL)
    {
        start_game(is_server, opts.inaddress, opts.inport, opts.input, &err);
    }
    else
    {
        usage(argv[0], EXIT_FAILURE, "No passing IP Address");
    }

    return EXIT_SUCCESS;
}

// Function to parsing command-line _arguments
static void parse_arguments(int argc, char **argv, struct options *opts)
{
    // Define option type
    static struct option long_options[] = {
        {"server",  no_argument,       NULL, 's'},
        {"client",  no_argument,       NULL, 'c'},
        {"input",   required_argument, NULL, 'i'},
        {"address", required_argument, NULL, 'n'},
        {"fd",      required_argument, NULL, 'p'},
        {"help",    no_argument,       NULL, 'h'},
        {NULL,      0,                 NULL, 0  }
    };

    // Parsing command-line _arguments
    int opt;
    int err;
    opterr = 0;
    while((opt = getopt_long(argc, argv, "hcsi:n:p:", long_options, NULL)) != -1)
    {
        switch(opt)
        {
            // To get which type to start game
            case 'c':
            {
                opts->client = true;
                break;
            }

            case 's':
            {
                opts->server = true;
                break;
            }

            // Get user input type (keyboard, console or random)
            case 'i':
            {
                opts->input = optarg;
                if(strcmp(opts->input, "kb") != 0 && strcmp(opts->input, "js") != 0 && strcmp(opts->input, "rd") != 0)
                {
                    usage(argv[0], EXIT_FAILURE, "Invalid user input");
                }
                break;
            }

            // Get IP address
            case 'n':
            {
                opts->inaddress  = optarg;
                opts->outaddress = optarg;
                break;
            }

            // Get port
            case 'p':
            {
                opts->inport  = convertPort(optarg, &err);
                opts->outport = convertPort(optarg, &err);

                if(err != ERR_NONE)
                {
                    usage(argv[0], EXIT_FAILURE, "Port most be between 0 and 65535");
                }
                break;
            }

            case 'h':
                usage(argv[0], EXIT_SUCCESS, "How to use program");

            default:
                usage(argv[0], EXIT_FAILURE, "Invalid arguments");
        }
    }
}

// Function to print error message and program help
_Noreturn static void usage(const char *program_name, int exit_code, const char *message)
{
    if(message)
    {
        fprintf(stderr, "%s\n", message);
    }

    fprintf(stderr, "Usage: %s [-h] [-c or -s] [-i <input>] [-n <address>] [-p <port>]\n", program_name);
    fputs("Options:\n", stderr);
    fputs("  -h, --help                           Display this help message\n", stderr);
    fputs("  -c, --client                           Run the game as client\n", stderr);
    fputs("  -s, --server                           Run the game as server\n", stderr);
    fputs("  -i <input>, --input <input>  User input type <input>: 'kb' for keyboard, 'js' for joysticks or 'rd' for random moves\n", stderr);
    fputs("  -n <address>, --address <address>  Create network socket <address>\n", stderr);
    fputs("  -p <port>, --address <address>    Create port for network socket <port>\n", stderr);
    exit(exit_code);
}

// Function to check number of input are passing through command-line
static void check_arguments(const char *program_name, const struct options *opts)
{
    int count = 0;

    // Check if user has run the game as client
    if(opts->client == true)
    {
        count = count + 1;
    }

    // Check if user has run the game as server
    if(opts->server == true)
    {
        count = count + 1;
    }

    // Check number of arguments
    if(count == 0)
    {
        usage(program_name, EXIT_FAILURE, "The game must be run as SERVER OR CLIENT");
    }

    if(count > 1)
    {
        usage(program_name, EXIT_FAILURE, "Only run game as Client OR Server at one time");
    }
}
