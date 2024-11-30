//
// Created by tommy on 11/25/24.
//

#ifndef PLAYER_H
#define PLAYER_H

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24

typedef struct {
    int x; // Current X position of the player
    int y; // Current Y position of the player
} Player;

/**
 * Initializes a player at a given position.
 * @param player Pointer to the Player structure.
 * @param start_x Initial X position.
 * @param start_y Initial Y position.
 */
void init_player(Player *player, int start_x, int start_y);

/**
 * Updates the position of a player based on input.
 * The input is a character representing movement (WASD or arrow keys).
 * @param player Pointer to the Player structure.
 * @param input Character representing movement direction.
 */
void update_player_position(Player *player, char input);

/**
 * Ensures the player wraps around the screen when reaching the edges.
 * @param player Pointer to the Player structure.
 */
void wrap_player_position(Player *player);

#endif // PLAYER_H

