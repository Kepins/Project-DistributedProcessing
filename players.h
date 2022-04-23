#ifndef PLAYERS_H
#define PLAYERS_H

#include "messages.h"

#include <semaphore.h>

typedef struct Player {
	int id;
	int points;
	int ready;
	sem_t queue_sem;
	MessagesQueue messagesqueue;
}Player;


void initPlayer(Player* player);
void destroyPlayer(Player* player);


#endif
