#include "messages.h"

void serialize_message(Message* message, char* msgstr) {
	message_type type = message->type;
	switch (type) {
		case ok:
			sprintf(msgstr, "%d %d", type, message->id);
			break;
		case click:
			sprintf(msgstr, "%d %d %d %d", type, message->x, message->y, message->id);
			break;
		case point:
			sprintf(msgstr, "%d %d", type, message->id);
			break;
		case add_player:
			sprintf(msgstr, "%d %d", type, message->id);
			break;
		case draw:
			sprintf(msgstr, "%d %d %d %d", type, message->x, message->y, message->id);
			break;
		case end:
			sprintf(msgstr, "%d %d", type, message->id);
			break;
		default:
			sprintf(msgstr, "%d", type);
			break;
	}
	int len = strlen(msgstr);
	msgstr[len] = MESSAGE_SEPARATOR;
	msgstr[len+1] = '\0';
}

int read_messages(char** beg_msgstr, Message* message){
	if(*beg_msgstr != NULL && *beg_msgstr !='\0'){
		char* end_msgstr = *beg_msgstr;
		while(*end_msgstr != MESSAGE_SEPARATOR){
			++end_msgstr;
		}
		char* new_beg_msgstr = NULL;
		if(*end_msgstr == MESSAGE_SEPARATOR && *(end_msgstr+1)!='\0'){
			new_beg_msgstr = end_msgstr+1;
		}
		*end_msgstr = '\0';
		deserialize_message(*beg_msgstr, message);
		
		//so the message is not modified after all
		*end_msgstr = MESSAGE_SEPARATOR;
		*beg_msgstr = new_beg_msgstr;
		return 1;
	}
	return 0;
}

void deserialize_message(char* msgstr, Message* message) {
	message_type type;
	sscanf(msgstr, "%d ", &type);
	switch (type) {
	case ok:
		sscanf(msgstr, "%d %d", &message->type, &message->id);
	case click:
		sscanf(msgstr, "%d %d %d %d", &message->type, &message->x, &message->y, &message->id);
		break;
	case point:
		sscanf(msgstr, "%d %d", &message->type, &message->id);
		break;
	case add_player:
		sscanf(msgstr, "%d %d", &message->type, &message->id);
		break;
	case draw:
		sscanf(msgstr, "%d %d %d %d", &message->type, &message->x, &message->y, &message->id);
		break;
	case end:
		sscanf(msgstr, "%d %d", &message->type, &message->id);
	default:
		sscanf(msgstr, "%d", &message->type);
		break;
	}
}

void initMessagesQueue(MessagesQueue* queue) {
	queue->size = 0;
	queue->root.message = NULL;
	queue->root.next = NULL;
}

int availableMessages(MessagesQueue* queue) {
	return queue->size;
}

void pushMessage(MessagesQueue* queue, Message* message) {
	Node* actNode = &queue->root;
	for (int i = 0; i < queue->size; i++) {
		actNode = actNode->next;
	}
	actNode->next = (Node*)malloc(sizeof(Node));
	actNode->next->message = message;
	actNode->next->next = NULL;
	queue->size += 1;
}

Message* popMessage(MessagesQueue* queue) {

	Node* toDestroy = queue->root.next;
	Message* ret = toDestroy->message;	
	queue->root.next = toDestroy->next;
	queue->size -= 1;
	free(toDestroy);
	return ret;

}

void destroyMessagesQueue(MessagesQueue* queue) {
	for (int i = 0; i < queue->size; i++) {
		Message* m = popMessage(queue);
		free(m);
	}
}
