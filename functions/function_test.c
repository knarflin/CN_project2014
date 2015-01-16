// function_test.c
#include<stdio.h>
#include"parse.h"
#include"filter.h"
#include"account.h"
#include"cqueue.h"
#include"msg_history.h"

//---------------------------- parse_test1 ----------------------------
void parse_test1(){
	char* msg="<login> <account>Ruby<\\>  \r <signup>\n \r <password>crispy<\\> <nam";
	char dst[2][32];
	char* dest[2]={dst[0],dst[1]};
	char* pos=msg;

	int i;

	// parse commands consecutively
	for(i=0;i<6;i++){
		if(parse(&pos,dest,NULL,NULL,NULL)) // return 1 for succeed
			printf("%s: %s\n",dst[0],dst[1]);
		else // return 0 for fragment
			printf("!!! Error: fragment! !!!\n");
	}
}

//---------------------------- parse_test2 for <filedata,autorun.exe,15>xxx<\> ----------------------------
void parse_test2(){
	char filename[64];
	char* filenameptr=filename;
	int datagram_cnt; // 
	int isfiledata;
	char* msg="<filedata,autorun.exe,15>Hello world!<\\>";
	char dst[2][32];
	char* dest[2]={dst[0],dst[1]};
	char* pos=msg;
	if(parse(&pos,dest,filenameptr,&datagram_cnt,&isfiledata)){
		if(isfiledata){ // 1 for isfiledata, 0 for not
			printf("Is file data!\n");
			printf("filename: %s\n",filenameptr);
			printf("datagram_cnt: %d\n",datagram_cnt);
			printf("File data: %s\n",dst[1]);
		}
		else
			printf("Not file data!\n");
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
	accountcnt = 0;
	
	i=create_account("Alice","lovely");
	printf("Creating account Alice...\n");
	if(i==0) printf("Successful!\n");
	else if(i==1) printf("Username already exists!\n");
	printf("\n");

	i=create_account("Bob","crispy");
	printf("Creating account Bob...\n");
	if(i==0) printf("Successful!\n");
	else if(i==1) printf("Username already exists!\n");
	printf("\n");

	i=create_account("Alice","happy");
	printf("Creating account Alice...\n");
	if(i==0) printf("Successful!\n");
	else if(i==1) printf("Username already exists!\n");
	printf("\n");

	i=authenticate("Alice","creepy");
	printf("Logging in account \"Alice\"...\n");
	if(i==0) printf("Correct password!\n");
	else if(i==1) printf("Wrong password!\n");
	else if(i==2) printf("Username not exist!\n");
	printf("\n");

		// test if Bob is online
		if(is_online("Bob"))
			printf("Bob is online!\n\n");
		else
			printf("Bob is not online...\n\n");

	// login account "Bob"
	i=authenticate("Bob","crispy");
	printf("Logging in account \"Bob\"...\n");
	if(i==0) printf("Correct password!\n");
	else if(i==1) printf("Wrong password!\n");
	else if(i==2) printf("Username not exist!\n");
	printf("\n");

		// test if Bob is online
		if(is_online("Bob"))
			printf("Bob is online!\n\n");
		else
			printf("Bob is not online!\n\n");

	// logout account "Bob"
	printf("Logging out account \"Bob\"...\n");
	logout_account("Bob");

		// test if Bob is online
		if(is_online("Bob"))
			printf("Bob is online!\n\n");
		else
			printf("Bob is not online!\n\n");

	i=authenticate("Cathy","sweety");
	printf("Logging in account \"Cathy\"...\n");
	if(i==0) printf("Correct password!\n");
	else if(i==1) printf("Wrong password!\n");
	else if(i==2) printf("Username not exist!\n");
	printf("\n");

	print_account(); 
}

// ---------------------------- jobqueue_test() ----------------------------
void jobqueue_test(){
	int i;
	struct job* jptr;

	create_account("Alice","lovely");
	create_account("Bob","crispy");
	create_account("Cathy","sweety");
	create_account("Derrick","hungry");

	// create jobs
	// int job_assign(char* dst_usr, char* _src_usr, char _jobtype, int _seg_count, char* _filename, char* _content) in account.h
	// return value:	0:successful	-1:queue is full	-2:no such dst_usr

	// 1.1 Alice says to Bob, 'Good night!'
	i=job_assign("Bob","Alice",'m',0,NULL,"Good morning!");
	if(i==0) printf("Successful!\n\n");
	else if(i==-2) printf("No such destination user!\n\n");

	i=job_assign("Bob","Alice",'m',0,NULL,"Good afternoon!");
	if(i==0) printf("Successful!\n\n");
	else if(i==-2) printf("No such destination user!\n\n");

	i=job_assign("Bob","Alice",'m',0,NULL,"Good night!");
	if(i==0) printf("Successful!\n\n");
	else if(i==-2) printf("No such destination user!\n\n");

	// 1.2 Bob sends autorun.ext to Obama
	i=job_assign("Obama","Bob",'f',12,"autorun.exe","Hello!"); 
	if(i==0) printf("Successful!\n\n");
	else if(i==-2) printf("No such destination user!\n\n");

	 // 1.3 "Oh!", Cathy says to Bob.
	job_assign("Bob","Cathy",'m',0,NULL,"Oh!");

	 // 1.4 Derrick sends angry_bird.exe to Alice
	job_assign("Alice","Derrick",'f',0,"angry_bird.exe","Ah?");


	// 2. print job queue of each account
	for(i=0;i<4;i++){
		printf("----- Job queue of %s -----\n",accountinfo[i].username);
		print_queue(&accountinfo[i].job_queue);
		printf("\n");
	}

	
	// 3. get (and dequeue) jobs
	// int job_get(char* name, struct job* jb) in account.h
	// return value:	0:successful	-1:empty queue	-2:no such dst_usr
	 
	// 3.1 Get a job from job queue of Alice
	printf("---Get a job from job queue of Alice---\n");
	i=job_get("Alice",&jptr); 
	if(i==0){
		print_job(jptr);
		// the server should do the job now...
		free(jptr); // after the job is done, free it
	}
	else if(i==-1) printf("Job queue is empty!\n");
	else if(i==-2) printf("No such user!\n");
	printf("\n");

	// 3.2 Get a job from job queue of Alice again. This time, the job queue is empty.
	printf("---Get a job from job queue of Alice again---\n");
	i=job_get("Alice",&jptr); 
	if(i==0){
		print_job(jptr);
		// the server should do the job now...
		free(jptr);
	}
	else if(i==-1) printf("Job queue is empty!\n");
	else if(i==-2) printf("No such user!\n");
	printf("\n");

	// 3.3 Get a job from job queue of Obama.
	printf("---Get a job from job queue of Obama---\n");
	i=job_get("Obama",&jptr); 
	if(i==0){
		print_job(jptr);
		// the server should do the job now...
		free(jptr);
	}
	else if(i==-1) printf("Job queue is empty!\n");
	else if(i==-2) printf("No such user!\n");
	printf("\n");

};

void msg_history_test(){
	char msg[1024];
	create_account("Alice","lovely");
	create_account("Bob","crispy");
	create_account("Cathy","sweety");
	create_account("Derrick","hungry");

	job_assign("Bob","Alice",'m',0,NULL,"Bob!");
	job_assign("Alice","Bob",'m',0,NULL,"Yes?");
	job_assign("Bob","Alice",'m',0,NULL,"Nice day, ah? :)");
	job_assign("Alice","Bob",'m',0,NULL,"...");
	job_assign("Alice","Cathy",'m',0,NULL,"Alice?");
	job_assign("Alice","Bob",'m',0,NULL,"Really a nice day...");

	// get_historical_message: return 0 for ok, 1 for no such username (the 1st parameter)
	get_historical_message("Alice","Bob",16,msg); // get 16 latest chatting records between Alice and Bob
	printf("%s",msg);
}

int main()
{
	//parse_test1();
	//parse_test2();
	//filter_test();
	//account_test();
	jobqueue_test(); // replace it with the function you want to test
	//msg_history_test();
	return 0;
}
