// msg_history.h

#ifndef MESSAGE
#define MESSAGE

#ifndef USERNAME_LIMIT_LEN
#define USERNAME_LIMIT_LEN	100
#endif

#define MAX_RECORD_COUNT 1000

#include<string.h>
#include<malloc.h>


struct msg_history{
	struct msg_record{
		char* sender;
		char* receiver;
		char* content;
		int sender_length;
		int receiver_length;
		int content_length;
	}msgRecord[MAX_RECORD_COUNT];
	int msgcnt;
};

int add_message0(struct msg_history* msgHistory, char* _sender, char* _receiver, char* _content){
	int sender_length;
	int content_length;
	int receiver_length;
	if(msgHistory->msgcnt==MAX_RECORD_COUNT)
		return -1;
	sender_length = strlen(_sender);
	content_length = strlen(_content);
	receiver_length = strlen(_receiver);
	msgHistory->msgRecord[msgHistory->msgcnt].sender = (char*) malloc( sizeof(char) * sender_length );
	strcpy(msgHistory->msgRecord[msgHistory->msgcnt].sender, _sender);
	msgHistory->msgRecord[msgHistory->msgcnt].receiver = (char*) malloc( sizeof(char) * receiver_length );
	strcpy(msgHistory->msgRecord[msgHistory->msgcnt].receiver, _receiver);
	msgHistory->msgRecord[msgHistory->msgcnt].content = (char*) malloc( sizeof(char) * content_length );
	strcpy(msgHistory->msgRecord[msgHistory->msgcnt].content, _content);
	msgHistory->msgRecord[msgHistory->msgcnt].sender_length = sender_length;
	msgHistory->msgRecord[msgHistory->msgcnt].content_length = content_length;
	msgHistory->msgRecord[msgHistory->msgcnt].receiver_length = receiver_length;
	msgHistory->msgcnt++;
	return 0;
}

void get_historical_message0(struct msg_history* msgHistory, char* friend_name, int number, char* output){
	int start_number;
	int i;
	char* ptr;

	start_number = number < msgHistory->msgcnt ? msgHistory->msgcnt-number : 0;
	ptr = output;

	for(i=start_number;i<msgHistory->msgcnt;i++){
		if( strcmp(msgHistory->msgRecord[i].sender,friend_name)==0 || strcmp(msgHistory->msgRecord[i].receiver,friend_name)==0){
			strcpy(ptr,msgHistory->msgRecord[i].sender);
			ptr += msgHistory->msgRecord[i].sender_length;
			strcpy(ptr,": ");
			ptr += 2;
			strcpy(ptr,msgHistory->msgRecord[i].content);
			ptr += msgHistory->msgRecord[i].content_length;
			strcpy(ptr,"\r\n");
			ptr+=2;
		}
	}
	ptr[0]=0;

}

#endif