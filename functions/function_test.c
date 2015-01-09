// parse_test.c
#include<stdio.h>
#include"parse.h"
#include"filter.h"
#include"account.h"


//---------------------------- parse_test1 ----------------------------
void parse_test1(){
	char* msg="<login> <account>Ruby<\\>  \r <signup>\n \r <password>crispy<\\> <nam";
	char dst[2][32];
	char* dest[2]={dst[0],dst[1]};
	char* pos=msg;

	int i;

	// parse commands consecutively
	for(i=0;i<6;i++){
		if(parse(&pos,dest,NULL,NULL,NULL))
			printf("%s: %s\n",dst[0],dest[1]);
		else
			printf("!!! Error: fragment! !!!\n");
	}
}

//---------------------------- parse_test2 ----------------------------
void parse_test2(){
	int fd; // file descriptor
	int datagramcount; // 
	int isfiledata;
	char* msg="<filedata,2,15>Hello world!<\\>";
	char dst[2][32];
	char* dest[2]={dst[0],dst[1]};
	char* pos=msg;
	if(parse(&pos,dest,&fd,&datagramcount,&isfiledata)){
		if(isfiledata){
			printf("Is file content!\n");
			printf("fd: %d\n",fd);
			printf("datagramcount: %d\n",datagramcount);
			printf("File content: %s\n",dst[1]);
		}
		else
			printf("Not file content!\n");
	}
	else
		printf("!!! Error: fragment! !!!\n");
}


//---------------------------- filter_test() ----------------------------
void filter_test(){
	char str[64]="fuck! oh FUCKkying? What a abcde fUCk!";
	filter(str);
	printf("%s\n",str);
}

//---------------------------- account_test() ----------------------------
void account_test(){
	int i;

	i=create_account("Alice","lovely");
	printf("Creating account Alice...\n");
	if(i==0) printf("Successful!\n");
	else if(i==1) printf("Username already existed!\n");
	printf("\n");

	i=create_account("Bob","crispy");
	printf("Creating account Bob...\n");
	if(i==0) printf("Successful!\n");
	else if(i==1) printf("Username already existed!\n");
	printf("\n");

	i=create_account("Alice","happy");
	printf("Creating account Alice...\n");
	if(i==0) printf("Successful!\n");
	else if(i==1) printf("Username already existed!\n");
	printf("\n");

	i=authenticate("Alice","creepy");
	printf("Logging in account \"Alice\"...\n");
	if(i==0) printf("Correct password!\n");
	else if(i==1) printf("Wrong password!\n");
	else if(i==2) printf("Username not existed!\n");
	printf("\n");

	i=authenticate("Bob","crispy");
	printf("Logging in account \"Bob\"...\n");
	if(i==0) printf("Correct password!\n");
	else if(i==1) printf("Wrong password!\n");
	else if(i==2) printf("Username not existed!\n");
	printf("\n");

	i=authenticate("Cathy","sweety");
	printf("Logging in account \"Cathy\"...\n");
	if(i==0) printf("Correct password!\n");
	else if(i==1) printf("Wrong password!\n");
	else if(i==2) printf("Username not existed!\n");
	printf("\n");

	print_account();

}

int main()
{
	account_test(); // replace it with the function you want to test
	return 0;
}