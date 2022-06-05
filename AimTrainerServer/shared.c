#include "shared.h"
#include "messages.h"


void init(){
	extern Shared_variables shared;
	shared.num_players = 0;
	memset(shared.players, 0, sizeof(Player*)*MAX_PLAYERS);
	shared.is_running = 0;
	shared.has_ended = 0;
	shared.curr_x = 0;
	shared.curr_y = 0;
	shared.curr_id = 0;
	sem_init(&shared.running_sem, 0, 1);
	sem_init(&shared.round_sem, 0, 1);
}
void deinit(){
	extern Shared_variables shared;
	sem_destroy(&shared.running_sem);
	sem_destroy(&shared.round_sem);
}

Player* add_player_to_list(){
	extern Shared_variables shared;
	if(shared.num_players + 1 == MAX_PLAYERS){
		return NULL;
	}
	else{
		Player* new_player = (Player*)malloc(sizeof(Player));
		initPlayer(new_player);
		
		Message* message = (Message*)malloc(sizeof(Message));
		message->type = ok;
		message->id = new_player->id;
		sem_wait(&new_player->queue_sem);
		pushMessage(&new_player->messagesqueue, message);
		sem_post(&new_player->queue_sem);
		
		
		//send message to every existing player about new player
		for(int i=0;i<shared.num_players;i++){
			Message* message = (Message*)malloc(sizeof(Message));
			message->type = add_player;
			message->id = shared.num_players;
			sem_wait(&shared.players[i]->queue_sem);
			pushMessage(&shared.players[i]->messagesqueue, message);
			sem_post(&shared.players[i]->queue_sem);
		}
		//send messages to new player about existing players
		for(int i=0;i<shared.num_players;i++){
			Message* message = (Message*)malloc(sizeof(Message));
			message->type = add_player;
			message->id = i;
			sem_wait(&new_player->queue_sem);
			pushMessage(&new_player->messagesqueue, message);
			sem_post(&new_player->queue_sem);
		}
		shared.players[shared.num_players] = new_player;
		shared.num_players++;
		return new_player;
	}
}

void update(int player_id){
	extern Shared_variables shared;
	Player* player_who_scored = shared.players[player_id];
	player_who_scored -> points +=1;
	if(player_who_scored -> points == POINTS_WINNING){
		shared.is_running = 0;
		shared.has_ended = 1;
		for(int i=0;i<shared.num_players;i++){
			Message* message = (Message*)malloc(sizeof(Message));
			message->type = end;
			message->id = player_who_scored->id;
			sem_wait(&shared.players[i]->queue_sem);
			pushMessage(&shared.players[i]->messagesqueue, message);
			sem_post(&shared.players[i]->queue_sem);
		}
	}
	else{
		shared.curr_x = (rand() % (WIDTH - 2 * RADIUS)) + RADIUS;
		shared.curr_y = (rand() % (HEIGHT - 2 * RADIUS)) + RADIUS;
		while(shared.curr_y<50){
			shared.curr_y = (rand() % (HEIGHT - 2 * RADIUS)) + RADIUS;
		}
		shared.curr_id += 1;
		for(int i=0;i<shared.num_players;i++){
			//send information about player who scored point
			Message* message = (Message*)malloc(sizeof(Message));
			message->type = point;
			message->id = player_who_scored->id;
			sem_wait(&shared.players[i]->queue_sem);
			pushMessage(&shared.players[i]->messagesqueue, message);
			sem_post(&shared.players[i]->queue_sem);
			
			//send information about new point
			message = (Message*)malloc(sizeof(Message));
			message->type = draw;
			message->x = shared.curr_x;
			message->y = shared.curr_y;
			message->id = shared.curr_id;
			sem_wait(&shared.players[i]->queue_sem);
			pushMessage(&shared.players[i]->messagesqueue, message);
			sem_post(&shared.players[i]->queue_sem);
		}
	}
}

void init_players_ready(){
	extern Shared_variables shared;
	sem_wait(&shared.running_sem);
	int all_ready = 1;
	for(int i=0;i<shared.num_players;i++){
		if(!shared.players[i]->ready){
			all_ready = 0;
			break;
		}
	}
	if(all_ready && !shared.is_running){

		shared.is_running = 1;
		shared.curr_x = (rand() % (WIDTH - 2 * RADIUS)) + RADIUS;
		shared.curr_y = (rand() % (HEIGHT - 2 * RADIUS)) + RADIUS;
		while(shared.curr_y<50){
			shared.curr_y = (rand() % (HEIGHT - 2 * RADIUS)) + RADIUS;
		}
		for(int i=0;i<shared.num_players;i++){
			//send information about the game starting
			Message* message = (Message*)malloc(sizeof(Message));
			message->type = start;
			sem_wait(&shared.players[i]->queue_sem);
			pushMessage(&shared.players[i]->messagesqueue, message);
			sem_post(&shared.players[i]->queue_sem);

			//send information about new point
			message = (Message*)malloc(sizeof(Message));
			message->type = draw;
			message->x = shared.curr_x;
			message->y = shared.curr_y;
			message->id = shared.curr_id;
			sem_wait(&shared.players[i]->queue_sem);
			pushMessage(&shared.players[i]->messagesqueue, message);
			sem_post(&shared.players[i]->queue_sem);
		}
	}
	
	sem_post(&shared.running_sem);	
}

int hit(int x, int y){
	extern Shared_variables shared;
	if(sqrt(pow(x-shared.curr_x, 2) + pow(y-shared.curr_y, 2)) <= RADIUS){
		return 1;
	}
	return 0;
}




