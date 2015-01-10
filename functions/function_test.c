// parse_test.c
#include<stdio.h>
#include"parse.h"
#include"filter.h"
#include"account.h"
#include"cqueue.h"

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
		if(isfiledata){
			printf("Is file content!\n");
			printf("filename: %s\n",filenameptr);
			printf("datagram_cnt: %d\n",datagram_cnt);
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

	i=authenticate("Bob","crispy");
	printf("Logging in account \"Bob\"...\n");
	if(i==0) printf("Correct password!\n");
	else if(i==1) printf("Wrong password!\n");
	else if(i==2) printf("Username not exist!\n");
	printf("\n");

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
	struct job jb[10]; // an array saving jobs
	struct job* jptr;

	// create jobs
	job_assign(&jb[0],"Alice",'m',0,NULL,"Good night!");
	job_assign(&jb[1],"Bob",'f',12,"autorun.exe","Hello!");
	job_assign(&jb[2],"Cathy",'m',0,NULL,"Oh!");
	job_assign(&jb[3],"Derrick",'f',0,"angry_bird.exe","Ah?");

	// initialize job queues
	for(i=0;i<=2;i++)
		queue_init(&accountinfo[i].job_queue);

	// assign each job to some account
	enqueue(&accountinfo[0].job_queue,&jb[0]);
	enqueue(&accountinfo[0].job_queue,&jb[3]);
	enqueue(&accountinfo[1].job_queue,&jb[1]);
	enqueue(&accountinfo[2].job_queue,&jb[2]);

	// print job queue of each account
	for(i=0;i<4;i++){
		printf("----- Job queue of account #%d -----\n",i);
		print_queue(&accountinfo[i].job_queue);
		printf("\n");
	}

	// dequeue a job from job queue of account 1 
	printf("---Dequeue a job from job queue of account 1---\n");
	i=dequeue(&accountinfo[1].job_queue,&jptr); 
	if(i==0)
		print_job(jptr);
	else if(i==1)
		printf("Job queue is empty!\n");
	printf("\n");

	// dequeue a job from job queue of account 1 again. This time, the queue is empty.
	printf("---Dequeue a job from job queue of account 1 again---\n");
	i=dequeue(&accountinfo[1].job_queue,&jptr); 
	if(i==0)
		print_job(jptr);
	else if(i==-1)
		printf("Job queue is empty!\n");
};

int main()
{
	//parse_test1();
	parse_test2();
	//filter_test();
	//account_test();
	//jobqueue_test(); // replace it with the function you want to test
	return 0;
}