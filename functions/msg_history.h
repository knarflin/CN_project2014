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
		char* content;
	}msgRecord[MAX_RECORD_COUNT];
	int msgcnt;
};

int add_message(struct msg_history msgHistory, char* _sender, char* _content){
	int sender_length;
	int content_length;
	if(msgHistory.msgcnt==MAX_RECORD_COUNT)
		return -1;
	sender_length = strlen(_sender);
	content_length = strlen(_content);
	msgHistory.msgRecord[msgHistory.msgcnt].sender = (char*) malloc( sizeof(char) * sender_length );
	msgHistory.msgRecord[msgHistory.msgcnt].content = (char*) malloc( sizeof(char) * content_length );
	msgHistory.msgcnt++;
	return 0;
}
	



#endif