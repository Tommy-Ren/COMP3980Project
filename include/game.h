//
// Created by tommy on 11/25/24.
//

#ifndef GAME_H
#define GAME_H

#include "network.h"
#include "object.h"
#include <SDL2/SDL.h>    // For joystick handling
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <time.h>    // For random input
#include <unistd.h>

#define TIME 50000
#define TIME_DIVIDER 100000
#define UNIT_CHANGE 1000
#define MAX_COUNTER 100
#define NUM 8000

void server_start_game(const char *ip_address, in_port_t port, const char *input_method, int *err);
void client_start_game(const char *ip_address, in_port_t port, const char *input_method, int *err);

#endif    // GAME_H
