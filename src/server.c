#include "../include/server.h"
#include "../include/open.h"

// Helper functions dealing with arguments
static void           checkArguments(const char *binary_name, const struct options *opts);
static void           parseArguments(int argc, char **argv, struct options *opts);
static void           handleArguments(const char *program_name, const char str);
_Noreturn static void usage(const char *program_name, int exit_code, const char *message);
static int            getInput(const struct options *opts, int *err);

int main(int argc, char *argv[])
{
    // Initialize variable
    struct options opts;
    int            fd_server;
    int            err;
    char (*filter_fun)(char) = NULL;

    // Set each member of options to 0
    err = 0;
    memset(&opts, 0, sizeof(opts));

    // Set inport and outport value
    opts.inport  = PORT;
    opts.outport  = PORT;

    // Sending data to the client
    parseArguments(argc, argv, &opts);

    // Check arguments
    check_arguments(argv[0], &opts);

    // Initialize input and output file descriptor
    fd_server = getInput(&opts, &err);

    // Check condition if input file can't open
    if(fd_server < 0)
    {
        const char *msg;
        msg = strerror(err);
        printf("Error opening input: %s\n", msg);
        goto err_in;
    }
    else
    {
        // Start running Server in the loop
        runServer(fd_server, filter_fun, err);
    }

err_in:
    return EXIT_SUCCESS;
}

// Function to parsing command-line arguments
static void parseArguments(int argc, char **argv, struct options *opts)
{
    // Define option type
    static struct option long_options[] = {
        {"server",	no_argument,       NULL, 's'},
        {"client",	no_argument,       NULL, 'c'},
        {"input", 	required_argument, NULL, 'i'},
        {"address", required_argument, NULL, 'n'},
        {"port",    required_argument, NULL, 'p'},
        {"help",    no_argument,       NULL, 'h'},
        {NULL,      0,                 NULL,  0 }
    };

    // Parsing command-line arguments
    int opt;
    int err;
    opterr = 0;
    while((opt = getopt_long(argc, argv, "hsci:n:p:", long_options, NULL)) != -1)
    {
        switch(opt)
        {
            case 's':
            {
                opts->server = true;
                break;
            }
            case 'c':
            {
                opts->client = true;
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
static void handleArguments(const char *program_name, const char str)
{
    // If user input is keyboard
    if (str == 'k')
    {
        openKeyboard();
    }

    // If user input is joysticks
    else if (str == 'j') 
    {

    }

    // If user input is random
    else if (str == 'r') 
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

    fprintf(stderr, "Usage: %s [-h] [-s or -c] [-i <input>] [-n <address>] [-p <port>]\n", program_name);
    fputs("Options:\n", stderr);
    fputs("  -h, --help                           Display this help message\n", stderr);
    fputs("  -s, --server                           Run the game as server\n", stderr);
    fputs("  -c, --client                           Run the game as client\n", stderr);
    fputs("  -i <input>, --input <input>	'k' for keyboard, 'j' for joysticks, 'r' for random\n", stderr);
    fputs("  -n <address>, --address <address>	Create network socket <address>\n", stderr);
    fputs("  -p <port>, --address <address>	Create port for network socket <port>\n", stderr);
    exit(exit_code);
}

// Function to check number of input are passing through command-line
static void checkArguments(const char *program_name, const struct options *opts)
{
    int count;
    // Check if user has run the game as client
    if(opts->client == true)
    {
        count += 1;
    }
    // Check if user has run the game as server
    if(opts->server == true)
    {
        count += 1;
    }
    // Check number of arguments
    if (count != 1) 
    {
        usage(program_name, EXIT_FAILURE, "Only run game as Client OR Server");
    }
}

// Function to get which type of input are passing through command-line
static int getInput(const struct options *opts, int *err)
{
    int fd;

    // Network socket server as type
    if(opts->inaddress != NULL)
    {
        fd = openNetworkSocketServer(opts->inaddress, opts->inport, BACKLOG, err);
    }

    // Invalid type
    else
    {
        fd = -1;
    }

    return fd;
}

// Function to run a server in a loop
// Exit when exit_flag is given
_Noreturn void runServer(int fd_server, char (*filter_fun)(char), int err)
{
    // Main server loop
    while(true)
    {
        int fd_client;

        // Create client file descriptor
        fd_client = accept(fd_server, NULL, 0);

        // Check condition if client file descriptor can't create
        if(fd_client < 0)
        {
            const char *msg;
            msg = strerror(err);
            printf("Error accpet client connection: %s\n", msg);
            continue;
        }

        // Processed data
        createProcess(fd_client, filter_fun, &err);
    }
}

// Function to create a process for each request to process data
void createProcess(int fd_client, char (*filter_fun)(char), int *err)
{
    pid_t pid = fork();

    // Fork creation failed
    if(pid < 0)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0)
    {
        // In child process
        int result = handleClient(fd_client, filter_fun, err);
        if(result == -1)
        {
            perror("Memory allocation error");
        }
        else if(result == -2)
        {
            perror("Read error");
        }
        else if(result == -3)
        {
            perror("Write error");
        }
        exit(result);
    }
    // In parent process
    close(fd_client);
}

// Function to handle client requests
int handleClient(int fd_client, char (*filter_fun)(char), int *err)
{
    char buffer[BUFSIZ];
    int  result     = 0;
    bool haveFilter = false;

    // Read-Process-Write loop
    printf("Client connected.\n");
    while(1)
    {
        ssize_t nread;

        // Set buffer to 0
        memset(buffer, 0, sizeof(buffer));

        // Step 1: Read data from client
        nread = dataRead(fd_client, buffer, sizeof(buffer) - 1, err);
        if(nread <= 0)    // Check for read errors or client disconnection
        {
            if(nread == 0)
            {
                printf("Client disconnected.\n");
            }
            else
            {
                perror("Error reading from client");
                result = -2;    // Update result on error
            }
            break;
        }

        buffer[nread] = '\0';

        // Check if not having filter, get the filter from buffer
        if(!haveFilter)
        {
            const char *filter;

            // Assign filter function pointer
            filter = buffer;
            printf("Filter from client: %s\n", filter);
            handleArguments(filter, &filter_fun);
            haveFilter = true;
            continue;
        }

        printf("Content from client: %s\n", buffer);

        // Step 2: Process data
        dataProcessing(buffer, nread, filter_fun);

        // Step 3: Send processed data back to client
        if(dataSend(buffer, fd_client, nread, err) < 0)
        {
            perror("Error sending data to client");
            result = -3;    // Update result on error
            break;
        }
    }

    // Close client connection when done
    close(fd_client);
    return result;
}
