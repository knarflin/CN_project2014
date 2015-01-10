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
	accountcnt++;
	return 0; // created successfully
}

int authenticate(char* username,char* password){
	int i;
	for(i=0;i<accountcnt;i++){
		if(strcmp(username,accountinfo[i].username)==0){
			if(strcmp(password,accountinfo[i].password)==0)
				return 0; // authentication passed
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

#endif