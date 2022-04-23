#ifndef MESSAGES_H
#define MESSAGES_H
#include <stdio.h>

#define MESSAGE_SEPARATOR ';'

typedef enum message_type { ready, click, ok, start, cant_join, point, draw, add_player, end } message_type;



typedef struct Message {
	message_type type;
	int x, y;
	int id;
} Message;

typedef struct Node Node;
struct Node {
	Message* message;
	Node* next;
};

typedef struct MessagesQueue {

	Node root;
	int size;

} MessagesQueue;

void serialize_message(Message* message, char* msgstr);
// beg_msgstr will be set to the beggining of the next msg or NULL if there is no next msg
int read_messages(char** beg_msgstr, Message* message);
void deserialize_message(char* msgstr, Message* message);

void initMessagesQueue(MessagesQueue* queue);
void pushMessage(MessagesQueue* queue, Message* message);
int availableMessages(MessagesQueue* queue);
Message* popMessage(MessagesQueue* queue);
void destroyMessagesQueue(MessagesQueue* queue);



#endif
