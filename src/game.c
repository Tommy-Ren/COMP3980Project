//
// Created by tommy and Kiet on 11/25/24.
//

#include "../include/game.h"

// Helper functions prototypes
static void render_screen(const Player *player1, const Player *player2, const Bullet bullets[]);
static void handle_bullets(Bullet bullets[], Player *opponent);
static void check_bullets_collide(Bullet bullets[]);
static void shoot_bullet(Bullet bullets[], const Player *shooter);
static int  find_inactive_bullet(Bullet bullets[]);
static char get_random_input(void);
static char get_joystick_input(SDL_GameController *controller);
void        sleep_in_microseconds(long time);

void server_start_game(const char *ip_address, in_port_t port, const char *input_method, int *err)
{
    // Declare all variables at the beginning
    Player              server_player;
    Player              client_player;
    Bullet              bullets[MAX_BULLETS] = {0};
    struct network     *ctx;
    SDL_GameController *controller = NULL;
    int                 game_state = ACTIVE;

    init_player(&server_player, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2);
    init_player(&client_player, 3 * SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2);

    for(int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].active = INACTIVE;
    }

    // Initialize network
    ctx = openNetworkSocketServer(ip_address, port, err);
    // Check if the operation failed
    if(ctx == NULL)
    {
        perror("Failed to open server network socket");
        if(err)
        {
            *err = -1;    // Indicate error
        }
    }

    // Initialize ncurses
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    timeout(0);

    if(strcmp(input_method, "js") == 0)
    {
        if(SDL_Init(SDL_INIT_GAMECONTROLLER) != 0)
        {
            fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
            cleanup_network(ctx);
            endwin();
            return;
        }

        if(SDL_NumJoysticks() > 0)
        {
            controller = SDL_GameControllerOpen(0);
            if(!controller)
            {
                fprintf(stderr, "Could not open game controller: %s\n", SDL_GetError());
                SDL_Quit();
                cleanup_network(ctx);
                endwin();
                return;
            }
        }
        else
        {
            fprintf(stderr, "No game controllers connected.\n");
            SDL_Quit();
            cleanup_network(ctx);
            endwin();
            return;
        }
    }

    while(game_state == ACTIVE)
    {
        char input = 0;
        if(strcmp(input_method, "kb") == 0)
        {
            // Get keyboard input
            input = (char)getch();
        }
        else if(strcmp(input_method, "js") == 0)
        {
            input = get_joystick_input(controller);
        }
        else if(strcmp(input_method, "rd") == 0)
        {
            // Generate random input based on timer
            input = get_random_input();
        }
        else
        {
            fprintf(stderr, "Unknown input method '%s'.\n", input_method);
            cleanup_network(ctx);
            endwin();
            return;
        }

        if(input != 0)
        {
            // Player 1 controls (WASD + Space to shoot)
            if(input == 'w' || input == 'a' || input == 's' || input == 'd' || input == 'W' || input == 'A' || input == 'S' || input == 'D')
            {
                update_player_position(&server_player, input);
                wrap_player_position(&server_player);
            }
            else if(input == ' ')
            {    // Space to shoot
                shoot_bullet(bullets, &server_player);
            }
        }

        // Update bullets
        handle_bullets(bullets, &server_player);
        handle_bullets(bullets, &client_player);
        check_bullets_collide(bullets);

        if(server_player.active != ACTIVE || client_player.active != ACTIVE)
        {
            game_state = INACTIVE;
        }

        // Send and receive game state
        send_game_state(ctx, &server_player, bullets, &game_state);
        receive_game_state(ctx, &client_player, bullets, &game_state);

        // Check if game has ended
        if(game_state == INACTIVE)
        {
            break;
        }

        // Render the screen
        clear();
        render_screen(&server_player, &client_player, bullets);
        refresh();

        // Delay for smoother gameplay
        sleep_in_microseconds(TIME);
    }

    // Cleanup
    cleanup_network(ctx);
    endwin();

    // Check game-ending conditions
    if(server_player.active != ACTIVE)
    {
        printf("Client player wins!\n");
    }
    else if(client_player.active != ACTIVE)
    {
        printf("Server player wins!\n");
    }
    else
    {
        printf("No player wins!\n");
    }
}

void client_start_game(const char *ip_address, in_port_t port, const char *input_method, int *err)
{
    // Declare all variables at the beginning
    Player              server_player;
    Player              client_player;
    Bullet              bullets[MAX_BULLETS] = {0};
    struct network     *ctx;
    SDL_GameController *controller = NULL;
    int                 game_state = ACTIVE;

    init_player(&server_player, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2);
    init_player(&client_player, 3 * SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2);

    for(int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].active = INACTIVE;
    }

    // Initialize network
    ctx = openNetworkSocketClient(ip_address, port, err);
    if(ctx == NULL)
    {
        perror("Failed to open client network socket");
        if(err)
        {
            *err = -1;    // Indicate error
        }
    }

    // Initialize ncurses
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    timeout(0);

    if(strcmp(input_method, "js") == 0)
    {
        if(SDL_Init(SDL_INIT_GAMECONTROLLER) != 0)
        {
            fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
            cleanup_network(ctx);
            endwin();
            return;
        }

        if(SDL_NumJoysticks() > 0)
        {
            controller = SDL_GameControllerOpen(0);
            if(!controller)
            {
                fprintf(stderr, "Could not open game controller: %s\n", SDL_GetError());
                SDL_Quit();
                cleanup_network(ctx);
                endwin();
                return;
            }
        }
        else
        {
            fprintf(stderr, "No game controllers connected.\n");
            SDL_Quit();
            cleanup_network(ctx);
            endwin();
            return;
        }
    }

    while(game_state == ACTIVE)
    {
        char input = 0;

        // Client should receive game state at the beginning
        receive_game_state(ctx, &server_player, bullets, &game_state);
        if(game_state == INACTIVE)
        {
            break;
        }

        if(strcmp(input_method, "kb") == 0)
        {
            // Get keyboard input
            input = (char)getch();
        }
        else if(strcmp(input_method, "js") == 0)
        {
            input = get_joystick_input(controller);
        }
        else if(strcmp(input_method, "rd") == 0)
        {
            // Generate random input based on timer
            input = get_random_input();
        }
        else
        {
            fprintf(stderr, "Unknown input method '%s'.\n", input_method);
            cleanup_network(ctx);
            endwin();
            return;
        }

        if(input != 0)
        {
            // Player 1 controls (WASD + Space to shoot)
            if(input == 'w' || input == 'a' || input == 's' || input == 'd' || input == 'W' || input == 'A' || input == 'S' || input == 'D')
            {
                update_player_position(&client_player, input);
                wrap_player_position(&client_player);
            }
            else if(input == ' ')
            {    // Space to shoot
                shoot_bullet(bullets, &client_player);
            }
        }

        // Update bullets
        handle_bullets(bullets, &client_player);
        handle_bullets(bullets, &server_player);
        check_bullets_collide(bullets);

        // Send and receive game state
        send_game_state(ctx, &client_player, bullets, &game_state);

        // Render the screen
        clear();
        render_screen(&server_player, &client_player, bullets);
        refresh();

        // Delay for smoother gameplay
        sleep_in_microseconds(TIME);
    }

    // Cleanup
    cleanup_network(ctx);
    endwin();

    // Check game-ending conditions
    if(server_player.active != ACTIVE)
    {
        printf("Client player wins!\n");
    }
    else if(client_player.active != ACTIVE)
    {
        printf("Server player wins!\n");
    }
    else
    {
        printf("No player wins!\n");
    }
}

static void render_screen(const Player *player1, const Player *player2, const Bullet bullets[])
{
    // Draw players
    if(player1->active)
    {
        mvprintw(player1->y, player1->x, "S");
    }
    if(player2->active)
    {
        mvprintw(player2->y, player2->x, "C");
    }

    // Draw bullets for Player 1
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(bullets[i].active)
        {
            mvprintw(bullets[i].y, bullets[i].x, (bullets[i].direction % 2 == 0) ? "|" : "-");
        }
    }
}

static void handle_bullets(Bullet bullets[], Player *opponent)
{
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(bullets[i].active)
        {
            move_bullet(&bullets[i]);
            if(is_bullet_shoot(&bullets[i], opponent))
            {
                opponent->active  = 0;    // Opponent is hit
                bullets[i].active = 0;    // Deactivate the bullet
            }
        }
    }
}

static void check_bullets_collide(Bullet bullets[])
{
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(bullets[i].active)
        {
            for(int j = 0; j < MAX_BULLETS; j++)
            {
                if(bullets[j].active && i != j)
                {
                    if(bullets[i].x == bullets[j].x && bullets[i].y == bullets[j].y)
                    {
                        bullets[i].active = 0;
                        bullets[j].active = 0;
                    }
                }
            }
        }
    }
}

static void shoot_bullet(Bullet bullets[], const Player *shooter)
{
    int bullet_index = find_inactive_bullet(bullets);
    if(bullet_index != -1)
    {
        init_bullet(&bullets[bullet_index], shooter->x, shooter->y, shooter->direction);
    }
}

static int find_inactive_bullet(Bullet bullets[])
{
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(!bullets[i].active)
        {
            return i;
        }
    }
    return -1;    // No inactive bullet found
}

static char get_random_input(void)
{
    static int counter = 0;
    int        random_direction;

    counter++;

    // Check for shoot every 100 frames
    if(counter % MAX_COUNTER == 0)
    {
        return ' ';    // Space to shoot
    }

    random_direction = rand() % 4;    // Random direction (0-3)
    switch(random_direction)
    {
        case 0:
            return 'w';    // Up
        case 1:
            return 'a';    // Left
        case 2:
            return 's';    // Down
        case 3:
            return 'd';    // Right
        default:
            return '0';
    }
}

static char get_joystick_input(SDL_GameController *controller)
{
    SDL_Event        event;
    static int       move_counter   = 0;    // To handle overly sensitive joystick
    static const int move_threshold = 2;    // Adjust this value for responsiveness

    (void)controller;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_CONTROLLERAXISMOTION)
        {
            // Increment the counter for motion events
            move_counter++;
            if(move_counter < move_threshold)
            {
                return 0;    // Skip overly frequent events
            }
            move_counter = 0;    // Reset counter after handling

            if(event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
            {
                if(event.caxis.value < -NUM)
                {
                    return 'w';    // Up
                }
                if(event.caxis.value > NUM)
                {
                    return 's';    // Down
                }
            }
            else if(event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
            {
                if(event.caxis.value < -NUM)
                {
                    return 'a';    // Left
                }

                if(event.caxis.value > NUM)
                {
                    return 'd';    // Right
                }
            }
        }
        else if(event.type == SDL_CONTROLLERBUTTONDOWN)
        {
            if(event.cbutton.button == 0)    // Button 0 (A button) for shooting
            {
                return ' ';    // Space to shoot
            }
        }
    }
    return 0;
}

void sleep_in_microseconds(long time)
{
    struct timespec req;
    struct timespec rem;
    req.tv_sec  = time / TIME_DIVIDER;
    req.tv_nsec = (time % TIME_DIVIDER) * UNIT_CHANGE;

    while(nanosleep(&req, &rem) == -1 && errno == EINTR)
    {
        req = rem;    // Continue with the remaining time if interrupted
    }
}
