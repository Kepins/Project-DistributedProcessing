#include "players.h"
#include "shared.h"
#include "messages.h"

void initPlayer(Player* player){
	extern Shared_variables shared;
	player->id = shared.num_players;
	player->points = 0;
	player->ready = 0;
	sem_init(&player->queue_sem, 0, 1);
	initMessagesQueue(&player->messagesqueue);
}

void destroyPlayer(Player* player){
	destroyMessagesQueue(&player->messagesqueue);
	sem_destroy(&player->queue_sem);
}

