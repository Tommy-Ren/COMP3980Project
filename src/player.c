//
// Created by tommy on 11/25/24.
//

#include "../include/player.h"

void init_player(Player *player, int start_x, int start_y) {
    player->x = start_x;
    player->y = start_y;
}

void update_player_position(Player *player, char input) {
    switch (input) {
        case 'w': // Move up
        case 'W':
            player->y--;
        break;
        case 's': // Move down
        case 'S':
            player->y++;
        break;
        case 'a': // Move left
        case 'A':
            player->x--;
        break;
        case 'd': // Move right
        case 'D':
            player->x++;
        break;
        case 'k': // Move up (Arrow key simulation)
            player->y--;
        break;
        case 'j': // Move down (Arrow key simulation)
            player->y++;
        break;
        case 'h': // Move left (Arrow key simulation)
            player->x--;
        break;
        case 'l': // Move right (Arrow key simulation)
            player->x++;
        break;
        default:
            // No action for other inputs
                break;
    }
}

void wrap_player_position(Player *player) {
    if (player->x < 0) {
        player->x = SCREEN_WIDTH - 1;
    } else if (player->x >= SCREEN_WIDTH) {
        player->x = 0;
    }

    if (player->y < 0) {
        player->y = SCREEN_HEIGHT - 1;
    } else if (player->y >= SCREEN_HEIGHT) {
        player->y = 0;
    }
}

