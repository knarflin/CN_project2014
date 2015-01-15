#ifndef ACCOUNT
#define ACCOUNT

#include<string.h>
#include"cqueue.h"

#ifndef USERNAME_LIMIT_LEN
#define USERNAME_LIMIT_LEN	100
#endif

#ifndef PASSWORD_LIMIT_LEN
#define PASSWORD_LIMIT_LEN	100
#endif

#define MAX_ACCOUNT_CNT 100

int accountcnt;

struct account{
	char username[USERNAME_LIMIT_LEN];
	char password[PASSWORD_LIMIT_LEN];
	int isOnline;
	struct cqueue job_queue;
}accountinfo[MAX_ACCOUNT_CNT];

void account_init(struct account* acc){

}

int create_account(char* username, char* password){
	int i;
	if(strlen(username)>USERNAME_LIMIT_LEN)
		return 3; // username length exceeds upper bound
	if(strlen(password)>PASSWORD_LIMIT_LEN)
		return 4; // password length exceeds upper bound
	if(accountcnt>=MAX_ACCOUNT_CNT)
		return 2; // account number reaches upper bound
	for(i=0;i<accountcnt;i++){
		if(strcmp(username,accountinfo[i].username)==0)
			return 1; // username already exists
	}
	strcpy(accountinfo[accountcnt].username,username);
	strcpy(accountinfo[accountcnt].password,password);
	accountinfo[accountcnt].isOnline = 0;
	accountinfo[accountcnt].job_queue.head = 0;
	accountinfo[accountcnt].job_queue.tail = 0;
	accountcnt++;
	return 0; // created successfully
}

int authenticate(char* username,char* password){
	int i;
	for(i=0;i<accountcnt;i++){
		if(strcmp(username,accountinfo[i].username)==0){
			if(strcmp(password,accountinfo[i].password)==0){
				accountinfo[i].isOnline = 1;
				return 0; // authentication passed
			}
			else
				return 1; // wrong password
		}
	}
	return 2; // username not exist
}

void print_account(){ // just for debugging
	int i;
	for(i=0;i<accountcnt;i++){
		printf("%d. username: %s\n",i,accountinfo[i].username);
		printf("   password: %s\n",accountinfo[i].password);
	}
}

void logout_account(char* username){
	int i;
	for(i=0;i<accountcnt;i++){
		if(strcmp(username,accountinfo[i].username)==0){
			accountinfo[i].isOnline = 0;
		}
	}
}

int is_online(char* username){
	int i;
	for(i=0;i<accountcnt;i++){
		if(strcmp(username,accountinfo[i].username)==0){
			return accountinfo[i].isOnline;
		}
	}
	return -1;
}

// assign the attributes of a job
// return value: 
//		0: successful
//		-1: queue is full
//		-2: no such dst_usr
int job_assign(char* dst_usr, char* _src_usr, char _jobtype, int _seg_count, char* _filename, char* _content){
	int i;
	struct job* jb = (struct job *)malloc( sizeof(struct job) );
	assert(strlen(_src_usr)<=USERNAME_LIMIT_LEN);
	strcpy(jb->src_usr,_src_usr);
	jb->jobtype=_jobtype;
	jb->seg_count=_seg_count;
	jb->filename=_filename;
	jb->content=_content;
	for(i=0;i<accountcnt;i++){
		if(strcmp(dst_usr,accountinfo[i].username)==0){
			return enqueue(&accountinfo[i].job_queue,jb);
		}
	}
	return -2;
}

// return 0 for successful, -1 for empty queue, -2 for no such user
int job_get(char* name, struct job** jb){
	int i;
	for(i=0;i<accountcnt;i++){
		if(strcmp(name,accountinfo[i].username)==0){
			return dequeue(&accountinfo[i].job_queue,jb);
		}
	}
	return -2;
}

#endif