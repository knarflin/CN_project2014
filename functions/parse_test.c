// parse_test.c
#include"parse.h"
#include<stdio.h>

void test1(){
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

void test2(){
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

int main()
{
	test2();
	return 0;
}