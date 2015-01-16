#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "parse.h"
#include "cqueue.h"
#include "filter.h"
#include "account.h"

#define ERR_EXIT(a) { perror(a); exit(1); }
const char ImDebug[] = "***\n";
const int bufSize 		= 1000;
const int ThisIsASignUp 	= ('s' - '0');
const int ThisIsAnAccount 	= ('a' - '0');
const int ThisIsALogIn 		= ('l' - '0');
const int ThisIsAUserName 	= ('u' - '0');
const int ThisIsAPassword 	= ('p' - '0');
const int ThisIsALogOut 	= -('l' - '0');
const int ThisIsGoTONextStage 	= ('G' - '0');
const int ThisIsAKnock 		= ('k' - '0');

typedef struct {
	char 		hostname[512];  // server's hostname
	unsigned short 	port;  		// port to listen
	int 		listen_fd;  	// fd to wait for a new connection!!!
} server;

typedef struct {
	char	host[512];  	// client's host
	int	conn_fd;  	// fd to talk with client!!!
	char 	buf[512];  	// data sent by/to client
	size_t 	buf_len;  	// bytes used by buf
	// you don't need to change this.
	int account;
	int wait_for_write;  	// used by handle_read to know if the header is read or not.
} request;

server 	 svr;  			// server
request* requestP = NULL; 	// point to a list of requests!!!
int maxfd;  			//即server.c本身有的fd table大小 size of open file descriptor table, size of request list
const int TableSize = 10;
const char* accept_read_header 	= "ACCEPT_FROM_READ";
const char* accept_write_header = "ACCEPT_FROM_WRITE";
const char* reject_header 	= "REJECT\n";
const char* Balance 		= "Balance:";
const char* OperationFail 	= "Operation fail.";
const char* ThisOccupied 	= "This account is occupied.";
const char* ThisAvailable 	= "This account is available.";
const char SIGNUP[] 		= "signup";

// Forwards
static int  handle_read ( request* reqP		);	//handle each write from each server
static void init_server ( unsigned short port	);
static void init_request( request* reqP		);
const  char signupGood[] = "<signup-good>";
const  char signupBadU[] = "<signup-badU>";
const  char loginGood[]  = "<login-good>";
const  char loginBadU[]  = "<login-badU>";
const  char loginBadP[]  = "<login-badP>";
const  char logoutConfirmed[] = "<logout-con>"

typedef struct thrdData
{
	int conn_fd;
	int listen_fd;
} TD;

int dealwithLoginout( 	int conn_fd, char OurBuf[], char dst1[], int twoArguement, 
			int continuityIndex, char acc[], int* islogin, int* istonextstage)
{
	int  i;
	char justBuf[bufSize];
	char dstbuf [bufSize];
	for(i = 0; i<bufSize	 ; i++) { dstbuf[i] = '\0';	}
	for(i = 0; i<strlen(dst1); i++) { dstbuf[i] = dst1[i]; 	}

	if(continuityIndex == ThisIsAUserName)
	{
		if(twoArguement == 0 || OurBuf[0] != 'u')
		{
			printf("Hey, U should do: <username>userName + enter\n");
			return 0;
		}
		else
		{
			sprintf(acc, "%s", dstbuf);
			printf("there's a username: %s\n", acc);
			return ThisIsAPassword;
		}
	}
	else if(continuityIndex == ThisIsAPassword)
	{
		if(twoArguement == 0 || OurBuf[0] != 'p')
		{
			printf("Hey, U should do: <password>userPassword + enter\n");
			return 0;
		}
		else
		{
			printf("there's a password\n");
			//TODO: validate
			if(*islogin == 1)
			{	
				int i = authenticate(acc, dstbuf);
				printf("Logging in account...\n");
				if(i==0) 
				{
					printf("Correct password, Bravo!\n");
					sprintf(justBuf, "%s", loginGood);
					write(conn_fd, justBuf, strlen(justBuf));
					*istonextstage = ThisIsGoTONextStage;
				}
				else if(i==1) 
				{
					printf("Wrong password!\n");
					sprintf(justBuf, "%s", loginBadP);
					write(conn_fd, justBuf, strlen(justBuf));
				}
				else if(i==2) 
				{
					printf("Username not exist!\n");
					sprintf(justBuf, "%s", loginBadU);
					write(conn_fd, justBuf, strlen(justBuf));
				}

				*islogin = 0;
				return 0;
			}
			else if(*islogin == 0)
			{
				int i=create_account(acc, dstbuf);
				printf("acc = %s; dst1 = %s\n", acc, dstbuf);
				printf("Creating account~~~\n");
				if(i==0) 
				{
					printf("Successful!\n");
					sprintf(justBuf, "%s", signupGood);
					write(conn_fd, justBuf, strlen(justBuf));
				}
				else if(i==1) 
				{
					printf("%u: Username already exists!\n", pthread_self());
					sprintf(justBuf, "%s", signupBadU);
					write(conn_fd, justBuf, strlen(justBuf));
				}

				return 0;
			}
		}
	}
	else if(continuityIndex == 0)
	{
		if(OurBuf[0] == 'l' && OurBuf[3] == 'i')
		{
			printf("there's a login\n");
			*islogin = 1;
			printf("in fun: *islogin = %d\n", *islogin);
			return ThisIsAUserName;
		}
		else if(OurBuf[0] == 's')
		{
			printf("there's a signup\n");
			*islogin = 0;
			return ThisIsAUserName;
		}
		else if(OurBuf[0] == 'l' && OurBuf[3] == 'o')
		{
			printf("there's an logout; but this version's thread still operates...\n");
			sprintf(justBuf, "%s", signupBadU); //here
			write(conn_fd, justBuf, strlen(justBuf));
			*islogin = 1126;
			return 0;
		}
		else
		{
			printf("for this version of server, there's nothing i can do\n");
			*islogin = 1126;
			return 0;
		}
	}
	return -1126;
}

const char userOnline[]  = "<user-online>";
const char userOffline[] = "<user-offline>";

int KnockMtransFiletrans( int conn_fd, char OurBuf[], char* dst1, int twoArguement,
			  int continuityIndex, char acc[], int* islogin, int *istonextstage )
{
	char justBuf[bufSize];
	if(continuityIndex == 0)//???
	{
		if(twoArguement == 0 || OurBuf[0] != 'k')
		{
			printf("Hey, U should do: <knock>userName + enter\n");
			return ThisIsAKnock;
		}
		else
		{
			printf("there's a knock, dont be afraid~\n");
			//TODO: change name
			if( is_online(dst1) == 1 )
			{
				sprintf(justBuf, "%s", userOnline);
				write(conn_fd, justBuf, strlen(justBuf));
				printf("%s is online!\n", dst1);
			}
			else
			{
				sprintf(justBuf, "%s", userOffline);
				write(conn_fd, justBuf, strlen(justBuf));
				printf("%s is NOT online...\n", dst1);
			}
			return 0;
		}
	}
	else 
		return -1126;
}

void* threadAnothSrv(void* arg)
{
	char OurBuf[bufSize];
	char acc[bufSize];
	int  continuityIndex 	= 0;
	int  islogin 		= 0;
	int  istonextstage 	= 0;
	int  twoArguement;
	TD 	*thrdPtr 	= arg;
	fd_set 	readset;

	while(1)
	{
		// TODO: parse messages; given messages parsed, gonna act
		int debug = 0;
		int ret =  handle_read(&requestP[thrdPtr->conn_fd]); 
		//printf("thrd: %u, %d, ret = %d\n",tid ,thrdPtr->conn_fd, ret);
		if (ret < 0) {
			fprintf(stderr, "bad request from %s\n", requestP[thrdPtr->conn_fd].host);
			continue;
		}
		char* pos;
		char dst[2][bufSize];
		char* dest[2]={dst[0],dst[1]};
		pos = requestP[thrdPtr->conn_fd].buf;

		//TODO: do sth corresponding to client's input
		while(1)
		{
			//printf("msg = %s\n", pos);
			if(parse(&pos,dest,NULL,NULL,NULL)) // return 1 for succeed
			{
				//printf("%s: %s\n",dst[0],dest[1]);
				//TODO: we must act
				if(strlen(dest[1]) == 0)
					twoArguement = 0;
				else
					twoArguement = 1;
				sprintf(OurBuf, dst[0], strlen(dst[0]));
				printf("%d: islogin = %d; OurBuf = %s %d, dst[1] = %s!\n", pthread_self(), islogin, OurBuf, debug, dst[1]);
				if(istonextstage == ThisIsGoTONextStage){
					continuityIndex = KnockMtransFiletrans(thrdPtr->conn_fd, OurBuf, dest[1], 
							twoArguement, continuityIndex, acc, &islogin, &istonextstage);
				}else{
					continuityIndex = dealwithLoginout( thrdPtr->conn_fd, OurBuf, dest[1], 
							twoArguement, continuityIndex, acc, &islogin, &istonextstage);
				}
				printf("%d: ousside, islogin = %d; ; OurBuf = %s %d, dst[1] = %s!\n",
						pthread_self(), islogin, OurBuf, debug, dst[1]);
				//WhatShouldBeThis = continuityIndex;
			}
			else
			{
				printf("Error: fragment! the left context won't be parsed! WTF...\n");
				break;
			}
			debug ++;
		}
	}
	pthread_exit((void*)1);
}

int main(int argc, char** argv) {
	// Parse args.
	if (argc != 2) {
		fprintf(stderr, "usage: %s [port]\n", argv[0]);
		exit(1);
	}

	int 	i, err;
	int 	clilen;
	int 	conn_fd;  		// fd for a new connection with client
	struct 	sockaddr_in cliaddr;  	// used by accept()
	pthread_t tid1;

	// open svr.listen_fd
	init_server((unsigned short) atoi(argv[1]));

	//TODO: Get file descriptor table size and initize request table
	maxfd 	 = TableSize;							  
	requestP = (request*) malloc(sizeof(request) * maxfd);
	if (requestP == NULL) {
		ERR_EXIT("out of memory allocating all requests");
	}
	for (i = 0; i < maxfd; i++) {
		init_request(&requestP[i]);
	}
	requestP[svr.listen_fd].conn_fd = svr.listen_fd;
	strcpy(requestP[svr.listen_fd].host, svr.hostname);

	//TODO: loop for handling connections
	fprintf(stderr, "starting on %.80s, port %d, fd %d, maxconn %d...\n", svr.hostname, svr.port, svr.listen_fd, maxfd);
	fd_set readset;
	TD data[TableSize];
	int conNum = 0;
	while (1) {
		//check whether new connection or command in
		while(1)
		{
			FD_ZERO(&readset);		
			FD_SET(svr.listen_fd, &readset);
			select(svr.listen_fd + 1, &readset, NULL, NULL, NULL);
			if(FD_ISSET(svr.listen_fd, &readset))	//有連線
			{
				// TODO: establish new connection
				clilen = sizeof(cliaddr);
				conn_fd = accept(svr.listen_fd, (struct sockaddr*)&cliaddr, (socklen_t*)&clilen);
				if (conn_fd < 0) {
					if (errno == EINTR || errno == EAGAIN) { continue; } // try again
					if (errno == ENFILE) {
						(void) fprintf(stderr, "out of file descriptor table ... (maxconn %d)\n", maxfd);
						continue;
					}
					ERR_EXIT("accept")
				}
				printf("parent thrd: new conn_fd == %d\n", conn_fd);
				requestP[conn_fd].conn_fd = conn_fd;
				strcpy(requestP[conn_fd].host, inet_ntoa(cliaddr.sin_addr));
				fprintf(stderr, "parent thrd: getting a new request... fd %d from %s\n", conn_fd, requestP[conn_fd].host);
				// TODO: thrd
				data[conNum].conn_fd 	= conn_fd;
				data[conNum].listen_fd 	= svr.listen_fd;
				//printf("data->conn_fd = %d; data->listen_fd = %d\n", data->conn_fd, data->listen_fd);
				err = pthread_create(&tid1, NULL, &threadAnothSrv, &(data[conNum]));
				if(err != 0) { 	printf("GG, cant create thrd\n"); }
				conNum ++;
			}
		}
	}
	free(requestP);
	return 0;
}


// ======================================================================================================
// You don't need to know how the following codes are working
#include <fcntl.h>

static void init_request(request* reqP) {
	reqP->conn_fd = -1;
	reqP->buf_len = 0;
	reqP->account = 0;
	reqP->wait_for_write = 0;
}

// basically, handle_read is used to read in req(i.e. context that client want U to know) from the client
// return 0: socket ended, request done.
// return 1: success, message (without header) got this time is in reqP->buf with reqP->buf_len bytes. read more until got <= 0.
// It's guaranteed that the header would be correctly set after the first read.
// error code:
// -1: client connection error
int handle_read(request* reqP) {
	int r, errNum;
	char buf[512] = {0};

	// Read in request from client
	r = read(reqP->conn_fd, buf, sizeof(buf));
	if (r < 0){
		errNum = -1;
	}else if (r == 0){
		errNum = 0;
	}else{
		memset(reqP->buf, 0, sizeof(reqP->buf)); //Warning!! unparsed strings in reqP's buf would be clear 
		memmove(reqP->buf, buf, strlen(buf));
		errNum = 1;
	}
	return errNum;
}

static void init_server(unsigned short port) {
	struct 	sockaddr_in servaddr;
	int 	tmp;

	gethostname(svr.hostname, sizeof(svr.hostname));
	svr.port = port;

	svr.listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (svr.listen_fd < 0) ERR_EXIT("socket");

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family 	 = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port 	 = htons(port);
	tmp = 1;
	if (setsockopt(svr.listen_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&tmp, sizeof(tmp)) < 0) {
		ERR_EXIT("setsockopt");
	}
	if (bind(svr.listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		ERR_EXIT("bind");
	}
	if (listen(svr.listen_fd, 1024) < 0) {
		ERR_EXIT("listen");
	}
}
