#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "parse.h"
#include "filter.h"
#include "account.h"
#include "cqueue.h"
#include <pthread.h>

#define ERR_EXIT(a) { perror(a); exit(1); }
const char ImDebug[] = "***\n";
const int bufSize = 1000;
const int ThisIsASignUp = ('s' - '0');
const int ThisIsAnAccount = ('a' - '0');
const int ThisIsALogIn = ('l' - '0');
const int ThisIsAUserName = ('u' - '0');
const int ThisIsAPassword = ('p' - '0');
const int ThisIsALogOut = -('l' - '0');
//used for WriteAcc and ReadAcc
typedef struct {
    int fd;
	int returnValue;
} FdAndReturn;

typedef struct {
    char hostname[512];  // server's hostname
    unsigned short port;  // port to listen
    int listen_fd;  // fd to wait for a new connection!!!
} server;

typedef struct {
    char host[512];  // client's host
    int conn_fd;  // fd to talk with client!!!
    char buf[512];  // data sent by/to client
    size_t buf_len;  // bytes used by buf
    // you don't need to change this.
	int account;
    int wait_for_write;  // used by handle_read to know if the header is read or not.
} request;

server svr;  // server
request* requestP = NULL;  // point to a list of requests!!!
int maxfd;  //即server.c本身有的fd table大小 size of open file descriptor table, size of request list
const int TableSize = 10;
const char* accept_read_header = "ACCEPT_FROM_READ";
const char* accept_write_header = "ACCEPT_FROM_WRITE";
const char* reject_header = "REJECT\n";
const char* Balance = "Balance:";
const char* OperationFail = "Operation fail.";
const char* ThisOccupied = "This account is occupied.";
const char* ThisAvailable = "This account is available.";
const char SIGNUP[] = "signup";


// Forwards
static void init_server(unsigned short port);
// initailize a server, exit for error

static void init_request(request* reqP);
// initailize a request instance

static int handle_read(request* reqP);
// return 0: socket ended, request done.
// return 1: success, message (without header) got this time is in reqP->buf with reqP->buf_len bytes. read more until got <= 0.
// It's guaranteed that the header would be correctly set after the first read.
// error code:
// -1: client connection error
// TODO: turn char #s to in #s

int SetReadLK(int fd, int offset, int len)
{
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = offset;
	lock.l_len = len;
	lock.l_pid = getpid();
	
	return(fcntl(fd, F_SETLK, &lock));
}
int SetWriteLK(int fd, int offset, int len)
{
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = offset;
	lock.l_len = len;
	lock.l_pid = getpid();
	
	return(fcntl(fd, F_SETLK, &lock));
}
typedef struct thrdData
{
	int conn_fd;
	int listen_fd;
}TD;
/*
C:<login>
C:<username>littleJohn<\>
S:<login-U-OK>, <login-FAIL>
C:<password>???<\>
S:<login-P-OK>, <login-FAIL>
C:<logout>
S:<logout-confirmed>
*/
/*
<signup>
<username>Fridays<\>
S: <signup-U-OK>, <signup-FAIL>
<password>yamatata<\>
S: <signup-P-OK>
*/
int dealwith(int conn_fd, char OurBuf[], char* dst1, int twoArguement, int continuityIndex, char acc[], int islogin)
{
	//printf("dst0 = %s\n", OurBuf);
	if(continuityIndex == ThisIsAUserName)
	{
		if(twoArguement == 0 || OurBuf[0] != 'u')
		{
			printf("Hey, U should do: <username> + enter\n");
			return ThisIsAPassword;
		}
		else
		{
			printf("there's a username\n");
			//TODO: change name
			sprintf(acc, "%s", dst1);
			return ThisIsAPassword;
		}
	}
	else if(continuityIndex == ThisIsAPassword)
	{
		if(twoArguement == 0 || OurBuf[0] != 'p')
		{
			printf("Hey, U should do: <password> + enter\n");
			return 0;
		}
		else
		{
			printf("there's a password\n");
			//TODO: validate
			if(islogin == 1)
			{	
				int i = authenticate(acc, dst1);
				printf("Logging in account...\n");
				if(i==0) 
					printf("Correct password, Bravo!\n");
				else if(i==1) 
					printf("Wrong password!\n");
				else if(i==2) 
					printf("Username not exist!\n");
				printf("\n");
				islogin = 0;
				return 0;
			}
			else if(islogin == 0)
			{
				int i=create_account(acc, dst1);
				printf("Creating account~~~\n");
				if(i==0) 
					printf("Successful!\n");
				else if(i==1) 
					printf("Username already exists!\n");
				printf("\n");
				return 0;
			}
		}
	}
	else if(continuityIndex == 0)
	{
		if(OurBuf[0] == 'l' && OurBuf[3] == 'i')
		{
			printf("there's a login\n");
			islogin = 1;
			return ThisIsAUserName;
		}
		else if(OurBuf[0] == 's')
		{
			printf("there's a signup\n");
			islogin = 0;
			return ThisIsAUserName;
		}
		else if(OurBuf[0] == 'l' && OurBuf[3] == 'o')
		{
			printf("there's an logout\n");
			islogin = 1126;
			return 0;
		}
		else
		{
			printf("for this version of server, there's nothing i can do\n");
			islogin = 1126;
			return 0;
		}
	}
	return -1126;
}
void* threadAnothSrv(void* arg)
{
	char OurBuf[bufSize];
	TD *thrdPtr = arg;
	fd_set readset;
	int continuityIndex = 0;
	while(1)
	{
		FD_ZERO(&readset);
		FD_SET(thrdPtr->conn_fd, &readset);
		
		select(thrdPtr->conn_fd + 1, &readset, NULL, NULL, NULL);
		if(FD_ISSET(thrdPtr->conn_fd, &readset) <= 0)
		{
			printf("err cant get message\n");
			exit(1);
		}
		
		// TODO: parse messages; given messages parsed, gonna act
		int ret = handle_read(&requestP[thrdPtr->conn_fd]); 
		if (ret < 0) {
			fprintf(stderr, "bad request from %s\n", requestP[thrdPtr->conn_fd].host);
			continue;
		}
		
		char* pos = requestP[thrdPtr->conn_fd].buf;
		char dst[2][32];
		char* dest[2]={dst[0],dst[1]};
		char acc[] = "thisisnotrue";
		int twoArguement;
		int islogin = 0;
		//int WhatShouldBeThis = 0;
		while(1)
		{
			//printf("msg = %s\n", pos);
			if(parse(&pos,dest,NULL,NULL,NULL)) // return 1 for succeed
			{
				//printf("%s: %s\n",dst[0],dest[1]);
				// TODO: we must act
				if(strlen(dest[1]) == 0)
					twoArguement = 0;
				else
					twoArguement = 1;
				sprintf(OurBuf, dst[0], strlen(dst[0]));
				continuityIndex = dealwith(thrdPtr->conn_fd, OurBuf, dest[1], twoArguement, continuityIndex, acc, islogin);
				//WhatShouldBeThis = continuityIndex;
			}
			else
			{
				//printf("Error: fragment! the left context won't be parsed! WTF...\n");
				break;
			}
		}
	}
}

int main(int argc, char** argv) {
    // Parse args.
    if (argc != 2) {
        fprintf(stderr, "usage: %s [port]\n", argv[0]);
        exit(1);
    }
	
    int i, err;
	//int PassPort[65536] = {0};
    struct sockaddr_in cliaddr;  // used by accept()
    int clilen;
    int conn_fd;  // fd for a new connection with client
    //char buf[512];
	pthread_t tid1;

    // Initialize server	(open svr.listen_fd)
    init_server((unsigned short) atoi(argv[1]));

    //TODO: Get file descriptor table size and initize request table
    maxfd = TableSize;							  
    requestP = (request*) malloc(sizeof(request) * maxfd);
    if (requestP == NULL) {
        ERR_EXIT("out of memory allocating all requests");
    }
    for (i = 0; i < maxfd; i++) {
        init_request(&requestP[i]);
    }
    requestP[svr.listen_fd].conn_fd = svr.listen_fd;
    strcpy(requestP[svr.listen_fd].host, svr.hostname);
	
    // Loop for handling connections
    fprintf(stderr, "starting on %.80s, port %d, fd %d, maxconn %d...\n", svr.hostname, svr.port, svr.listen_fd, maxfd);
	fd_set readset;
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
					if (errno == EINTR || errno == EAGAIN) continue;  // try again
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
				TD data;
				data.conn_fd = conn_fd;
				data.listen_fd = svr.listen_fd;
				//printf("data->conn_fd = %d; data->listen_fd = %d\n", data->conn_fd, data->listen_fd);
				err = pthread_create(&tid1, NULL, &threadAnothSrv, &data);
				if(err != 0)
					printf("GG, cant create thrd\n");
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
    char buf[512];
    // Read in request from client
    r = read(reqP->conn_fd, buf, sizeof(buf));
	//printf("buf inside handle_read = %s\n", buf);
    if (r < 0) 
		errNum = -1;
    else if (r == 0)
		errNum = 0;
	/*char* p1 = strstr(buf, "\015\012");
	be careful that in Windows, line ends with \015\012, Oh man, WTF is that!!!
	if (p1 == NULL) {
		p1 = strstr(buf, "\012");
		//newline_len = 1;
		if (p1 == NULL) {
			ERR_EXIT("this really should not happen...");
		}
	}
	size_t len = p1 - buf + 1;
	memmove(reqP->buf, buf, len);
	reqP->buf[len - 1] = '\0';
	reqP->buf_len = len-1;*/
	else
	{
		memmove(reqP->buf, buf, strlen(buf));
		errNum = 1;
	}
    return errNum;
}

static void init_server(unsigned short port) {
    struct sockaddr_in servaddr;
    int tmp;

    gethostname(svr.hostname, sizeof(svr.hostname));
    svr.port = port;

    svr.listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (svr.listen_fd < 0) ERR_EXIT("socket");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
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


