#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>

// Screen size
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24

// Maximum number of bullets
#define MAX_BULLETS 200

// Facing directions
#define FACE_UP 0
#define FACE_RIGHT 1
#define FACE_DOWN 2
#define FACE_LEFT 3

#define ACTIVE 1
#define INACTIVE 0

typedef struct
{
    int x;            // X position of the player
    int y;            // Y position of the player
    int direction;    // Direction of the player: 0=Up, 1=Right, 2=Down, 3=Left
    int active;       // 1 ACTIVE, 0 INACTIVE
} Player;

typedef struct
{
    int x;            // X position of the bullet
    int y;            // Y position of the bullet
    int direction;    // Direction of the bullet: 0=Up, 1=Right, 2=Down, 3=Left
    int active;       // 1 ACTIVE, 0 INACTIVE
} Bullet;

void init_player(Player *player, int start_x, int start_y);
void update_player_position(Player *player, char input);
void wrap_player_position(Player *player);

void init_bullet(Bullet *bullet, int start_x, int start_y, int direction);
void move_bullet(Bullet *bullet);
int  is_bullet_shoot(const Bullet *bullet, const Player *player);

#endif    // OBJECT_H
