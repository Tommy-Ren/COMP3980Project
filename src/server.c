#include "../include/server.h"
#include "../include/open.h"

// Helper functions dealing with arguments
static void           parseArguments(int argc, char **argv, struct options *opts);
static void           handleArguments(const char *program_name, const char *str);
_Noreturn static void usage(const char *program_name, int exit_code, const char *message);
static void           checkArguments(const char *program_name, const struct options *opts);
static void           getSocket(const char *program_name, const struct options *opts, int *err);

int main(int argc, char *argv[])
{
    // Initialize variable
    struct options opts;
    int            fd;
    int            err;

    // Set each member of options to 0
    err = 0;
    memset(&opts, 0, sizeof(opts));

    // Set inport and outport value
    opts.inport  = PORT;
    opts.outport  = PORT;

    // Sending data to the client
    parseArguments(argc, argv, &opts);

    // Initialize input and output file descriptor
    checkArguments(argv[0], &opts);
    getSocket(argv[0], &opts, &err);
}

// Function to parsing command-line arguments
static void parseArguments(int argc, char **argv, struct options *opts)
{
    // Define option type
    static struct option long_options[] = {
        {"server",       no_argument,       NULL, 's'},
        {"client",       no_argument,       NULL, 'c'},
        {"input",  required_argument, NULL, 'i'},
        {"address",  required_argument, NULL, 'n'},
        {"fd",     required_argument, NULL, 'p'},
        {"help",       no_argument,       NULL, 'h'},
        {NULL,         0,                 NULL, 0  }
    };

    // Parsing command-line arguments
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
                is_server = 0;
                break;
            }

            case 's':
            {
                opts->server = true;
                is_server = 1;
                break;
            }
            
            // Get user input type (keyboard, console or random)
            case 'i':
            {
                opts->input = optarg;
                handleArguments(argv[0], opts->input);
                break;
            }

            // Get IP address
            case 'n':
            {
                opts->inaddress = optarg;
                opts->outaddress = optarg;
                break;
            }

            // Get port
            case 'p':
            {
                opts->inport = convertPort(optarg, &err);
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

// Function to handle arguments
static void handleArguments(const char *program_name, const char* str)
{
    // If user input is keyboard
    if (strcmp(str, "kb") == 0)
    {
        openKeyboard();
    } 
    
    // If user input is joysticks
    else if (strcmp(str, "js") == 0) 
    {

    }

    // If user input is random
    else if (strcmp(str, "rd") == 0) 
    {

    }

    // If invalid user input
    else
    {
        usage(program_name, EXIT_FAILURE, "Invalid user input");
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
static void checkArguments(const char *program_name, const struct options *opts)
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
    if (count > 1) 
    {
        usage(program_name, EXIT_FAILURE, "Only run game as Client OR Server at one time");
    }
}

// Function to get which type of input are passing through command-line
static void getSocket(const char *program_name, const struct options *opts, int *err)
{

    // Network socket server as type
    if(opts->inaddress != NULL)
    {
        // If the user choose server type
        if (is_server == 1 ) 
        {
            openNetworkSocketServer(opts->inaddress, opts->inport, err);
        } 
        
        // If the user choose client type
        else if (is_server == 0) 
        {
            openNetworkSocketClient(opts->inaddress, opts->inport, err);
        } 
    }

    // Invalid type
    else
    {
        usage(program_name, EXIT_FAILURE, "IP Address is invalid");
    }

}
