#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <stdlib.h>
#include <time.h>
#include <fcntl.h>

#include "messages.h"
#include "players.h"
#include "shared.h"


#define MESSAGE_BUFFER 2000
#define PORT 5000



sem_t connection_sem;

void* connection_handler(void *socket_desc) {
	/* Get the socket descriptor */
	int sock = * (int *)socket_desc;
	sem_post(&connection_sem);

	extern Shared_variables shared;
	
	int ended = 0;
	int close_connection = 0;
	Player* player=NULL;
	sem_wait(&shared.running_sem);
	if(shared.is_running){
		close_connection=1;
	}
	else{
		player = add_player_to_list();
		if(player == NULL){
			close_connection=1;
		}
		
	}
	sem_post(&shared.running_sem);
	
	int read_size=-1;
	char server_message[MESSAGE_BUFFER] , client_messages[MESSAGE_BUFFER];
	
	if(close_connection){
		Message message;
		message.type = cant_join;
		serialize_message(&message, server_message);
		write(sock , server_message , strlen(server_message));
	}
	else{
		while(!ended && read_size !=0) 
		{			
			//read messages
			read_size = recv(sock , client_messages , MESSAGE_BUFFER , MSG_DONTWAIT);
			
			if(read_size!=-1 && read_size !=0){
				//process message from the client
				client_messages[read_size] = '\0';
				
				//debug
				fprintf(stderr, "%d %s\n",player->id,client_messages);
				
				
				Message msg;
				char* beg_msgstr = &client_messages[0];

				//read_messages moves beg_msgstr
				while(read_messages(&beg_msgstr, &msg)){
					switch(msg.type){
						case ready:{
							if(!player->ready){
								player->ready = 1;
								init_players_ready();
							}
						}break;
						case click:{
							sem_wait(&shared.round_sem);
							if(shared.is_running&&hit(msg.x, msg.y)&&msg.id==shared.curr_id){
								update(player->id);
							}
							sem_post(&shared.round_sem);
						}break;
					}
				}
				memset(client_messages, 0, MESSAGE_BUFFER);
			}	
		
		
		
		
		
			//send all pending messages to the player
			MessagesQueue* msq = &player->messagesqueue;
			while(availableMessages(msq)){
				sem_wait(&player->queue_sem);
				Message* msg= popMessage(msq);
				sem_post(&player->queue_sem);
				serialize_message(msg, server_message);
				write(sock , server_message , strlen(server_message));
				if(msg->type == end){
					ended = 1;	
				}
				if(msg->type == 3){
					sleep(5);
				}
				free(msg);
				memset(server_message, 0, MESSAGE_BUFFER);
			}		
		
		} 
	}
	fprintf(stderr, "Client disconnected\n"); 
	if(player!=NULL){
		destroyPlayer(player);
		free(player);
	}	
	
	close(sock);
	pthread_exit(NULL);
}

#define MAX_THREADS 30

int main(int argc, char *argv[]) {
	
	srand(time(0));
	sem_init(&connection_sem, 0, 1);
	extern Shared_variables shared;
	//init globals
	init(&shared);
	
	
	int listenfd = 0, connfd = 0;
	struct sockaddr_in serv_addr; 
	
	int threads_count = 0;
	pthread_t* thread_ids[MAX_THREADS];

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT); 
	
	
	fcntl(listenfd, F_SETFL, fcntl(listenfd, F_GETFL) | O_NONBLOCK);
	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

	listen(listenfd, 10); 

	while (!shared.has_ended) {
		
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
		if (connfd != -1){
			sem_wait(&connection_sem);
			int sock = connfd;
			fprintf(stderr, "Connection accepted\n"); 
			
			if(threads_count< MAX_THREADS){
				thread_ids[threads_count] = (pthread_t*)malloc(sizeof(pthread_t));
				pthread_create(thread_ids[threads_count], NULL, connection_handler , (void *) &sock);
				threads_count++;
			}
		}
		else{
			sleep(1);
		}
	}
	for(int i=0;i<threads_count;i++)
	{
		pthread_join(*thread_ids[i] , NULL);
		free(thread_ids[i]);
	}
	sem_destroy(&connection_sem);
	deinit();
	fprintf(stderr, "Game ended\n"); 
}


