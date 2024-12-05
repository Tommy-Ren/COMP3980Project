#include "../include/object.h"

void init_player(Player *player, int start_x, int start_y)
{
    player->x         = start_x;
    player->y         = start_y;
    player->direction = FACE_UP;
    player->alive     = ACTIVE;
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

void move_bullet(Bullet *bullet, Player *player)
{
    // Store the old position
    int old_x = bullet->x;
    int old_y = bullet->y;

    // Check intermediate positions for collision
    int dx    = bullet->x - old_x;
    int dy    = bullet->y - old_y;
    int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

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

    for(int i = 1; i <= steps; i++)
    {
        int intermediate_x = (old_x + (i * dx) / steps + SCREEN_WIDTH) % SCREEN_WIDTH;
        int intermediate_y = (old_y + (i * dy) / steps + SCREEN_HEIGHT) % SCREEN_HEIGHT;

        if(is_bullet_shoot_intermediate(intermediate_x, intermediate_y, player))
        {
            player->alive  = DEAD;
            bullet->active = DEAD;
            return;
        }
    }
}

int is_bullet_shoot(const Bullet *bullet, const Player *player)
{
    if(!bullet->active || !player->alive)
    {
        return 0;
    }
    return (bullet->x == player->x && bullet->y == player->y);
}

int is_bullet_shoot_intermediate(int x, int y, const Player *player)
{
    if(!player->alive)
    {
        return 0;
    }
    return (x == player->x && y == player->y);
}
