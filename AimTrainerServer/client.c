#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr

#include "messages.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MESSAGE_BUFFER 2000
#define PORT 5000


int main(int argc , char *argv[])
{
	srand(time(0));

	int sock;
	struct sockaddr_in server;
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
		
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( PORT );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}
	
	puts("Connected");
	int ended = 0;
	
	char client_message[MESSAGE_BUFFER];
	char server_messages[MESSAGE_BUFFER];
	int read_size=-1;
	
	while(!ended && read_size !=0) 
	{
		read_size = recv(sock , server_messages , MESSAGE_BUFFER , MSG_DONTWAIT);
		if(read_size!=-1 && read_size !=0){
			server_messages[read_size] = '\0';
			
			//debug
			printf("%s\n", server_messages);
			
			Message msg;
			char* beg_msgstr = &server_messages[0];

			//read_messages moves beg_msgstr
			while(read_messages(&beg_msgstr, &msg)){
				switch(msg.type){
					case ok:{
						printf("ok your id: %d\n", msg.id);
					}break;
					case add_player:{
						printf("Should add player: %d\n", msg.id);
					}break;
					case end:{
						ended=1;
					}
				}
			}
			memset(server_messages, 0, MESSAGE_BUFFER);
		}
		if(!ended){
			printf("Czekam na wiadomosc: ");
			gets(client_message);	
			if(strcmp(client_message, "nop")){
				printf("Wysylam:\"%s\"\n", client_message);
				write(sock , client_message , strlen(client_message));
			}
		}
		
	} 
	return 0;
}
