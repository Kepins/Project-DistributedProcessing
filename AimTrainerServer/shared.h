#ifndef SHARED_H
#define SHARED_H
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <math.h>
#include "players.h"



#define MAX_PLAYERS 10
#define POINTS_WINNING 10
#define WIDTH 1024
#define HEIGHT 720
#define RADIUS 20

typedef struct Shared_variables{
	int num_players;
	Player* players[MAX_PLAYERS];
	int is_running;
	int has_ended;
	int curr_x;
	int curr_y;
	int curr_id;
	sem_t running_sem;
	sem_t round_sem;
}Shared_variables;

Shared_variables shared;


void init();
void deinit();
Player* add_player_to_list();

void update(int player_id);

#endif
