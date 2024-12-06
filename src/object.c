#include "../include/object.h"

void init_player(Player *player, int start_x, int start_y)
{
    player->x         = start_x;
    player->y         = start_y;
    player->direction = FACE_UP;
    player->active    = ACTIVE;
}

void update_player_position(Player *player, char input)
{
    switch(input)
    {
        case 'w':    // Move up
        case 'W':
            player->y--;
            player->direction = FACE_UP;
            break;
        case 's':    // Move down
        case 'S':
            player->y++;
            player->direction = FACE_DOWN;
            break;
        case 'a':    // Move left
        case 'A':
            player->x--;
            player->direction = FACE_LEFT;
            break;
        case 'd':    // Move right
        case 'D':
            player->x++;
            player->direction = FACE_RIGHT;
            break;
        default:
            // No action for other inputs
            break;
    }
}

void wrap_player_position(Player *player)
{
    if(player->x < 0)
    {
        player->x = SCREEN_WIDTH - 1;
    }
    else if(player->x >= SCREEN_WIDTH)
    {
        player->x = 0;
    }

    if(player->y < 0)
    {
        player->y = SCREEN_HEIGHT - 1;
    }
    else if(player->y >= SCREEN_HEIGHT)
    {
        player->y = 0;
    }
}

void init_bullet(Bullet *bullet, int start_x, int start_y, int direction)
{
    bullet->x         = start_x;
    bullet->y         = start_y;
    bullet->direction = direction;
    bullet->active    = ACTIVE;
}

void move_bullet(Bullet *bullet)
{
    if(!bullet->active)
    {
        return;    // Skip if the bullet is inactive
    }

    // Move the bullet based on its direction
    switch(bullet->direction)
    {
        case FACE_UP:
            bullet->y--;
            break;
        case FACE_RIGHT:
            bullet->x++;
            break;
        case FACE_DOWN:
            bullet->y++;
            break;
        case FACE_LEFT:
            bullet->x--;
            break;
        default:
            return;
    }

    // Wrap around the screen edges
    if(bullet->x < 0)
    {
        bullet->x = SCREEN_WIDTH - 1;
    }
    else if(bullet->x >= SCREEN_WIDTH)
    {
        bullet->x = 0;
    }
    if(bullet->y < 0)
    {
        bullet->y = SCREEN_HEIGHT - 1;
    }
    else if(bullet->y >= SCREEN_HEIGHT)
    {
        bullet->y = 0;
    }
}

int is_bullet_shoot(const Bullet *bullet, const Player *player)
{
    if(!bullet->active || !player->active)
    {
        return 0;
    }

    // Calculate the bullet's next position based on its direction, with wrapping
    int next_x = bullet->x;
    int next_y = bullet->y;

    switch(bullet->direction)
    {
        case FACE_UP:
            next_y = (bullet->y - 1 + SCREEN_HEIGHT) % SCREEN_HEIGHT;
            break;
        case FACE_DOWN:
            next_y = (bullet->y + 1) % SCREEN_HEIGHT;
            break;
        case FACE_LEFT:
            next_x = (bullet->x - 1 + SCREEN_WIDTH) % SCREEN_WIDTH;
            break;
        case FACE_RIGHT:
            next_x = (bullet->x + 1) % SCREEN_WIDTH;
            break;
        default:
            return 0;
    }

    // Check if the bullet passes through or lands on the player's position
    if((bullet->x == player->x && bullet->y == player->y) ||    // Bullet already at player
       (next_x == player->x && next_y == player->y) ||          // Bullet moves to player
       (bullet->x == player->x && next_y == player->y) ||       // Vertical path intersection
       (next_x == player->x && bullet->y == player->y))
    {
        return 1;
    }

    return 0;
}
