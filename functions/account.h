#ifndef ACCOUNT
#define ACCOUNT

#include<string.h>

#ifndef ACCOUNT_LIMIT_LEN
#define ACCOUNT_LIMIT_LEN	100
#endif

#ifndef PASSWORD_LIMIT_LEN
#define PASSWORD_LIMIT_LEN	100
#endif

#define MAX_ACCOUNT_CNT 100

char account_username[MAX_ACCOUNT_CNT][ACCOUNT_LIMIT_LEN];
char account_password[MAX_ACCOUNT_CNT][PASSWORD_LIMIT_LEN];
int accountcnt;

int create_account(char* username, char* password){
	int i;
	if(accountcnt>=MAX_ACCOUNT_CNT)
		return 2; // username already exists
	for(i=0;i<accountcnt;i++){
		if(strcmp(username,account_username[i])==0)
			return 1; // account number reaches upper bound
	}
	strcpy(account_username[accountcnt],username);
	strcpy(account_password[accountcnt],password);
	accountcnt++;
	return 0; // created successfully
}

int authenticate(char* username,char* password){
	int i;
	for(i=0;i<accountcnt;i++){
		if(strcmp(username,account_username[i])==0){
			if(strcmp(password,account_password[i])==0)
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
		printf("%d. username: %s\n",i,account_username[i]);
		printf("   password: %s\n",account_password[i]);
	}
}

#endif