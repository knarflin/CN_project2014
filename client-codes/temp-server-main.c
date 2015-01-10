#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEFAULT_SERVER_BACKLOG	20
#define SOCK_BUFFER_SIZE	1000

#ifndef	DEFAULT_SERVER_PORT
#define DEFAULT_SERVER_PORT	22233
#endif

#ifndef	DEFAULT_SERVER_IP 
#define	DEFAULT_SERVER_IP	"127.0.0.1" //linux17.csie.org
#endif

void set_server_ip_and_port( char* serv_ip, int* serv_port ){
	char reply;
	printf( "Do you want to set the server's IP and Port?(Y/N)\n" );
	scanf("%c", &reply);
	if( reply == 'Y' || reply == 'y' ){
		printf( "Enter IP: ");
		scanf( "%s", serv_ip ); //A string like "100.200.0.1"
		printf( "\nEnter Port: ");
		scanf( "%d", serv_port ); //A port number for listen
		printf( "\n" );
	}else if(reply == 'N' || reply == 'n' ){
		strncpy( serv_ip, DEFAULT_SERVER_IP, strlen(DEFAULT_SERVER_IP) );
		*serv_port = DEFAULT_SERVER_PORT;
		printf( "IP and Port set to default. (%s, %d)\n", DEFAULT_SERVER_IP, DEFAULT_SERVER_PORT );
	}else{
		printf( "Invalid reply.\n");
		exit(0);
	}
	return;
}


int main(void){

	char serv_ip[20];
	int serv_port;
	int serv_sockfd;
	int clie_sockfd;
	int clie_addr_size;
	struct sockaddr_in clie_addr;
	struct sockaddr_in serv_addr;
	char r_buf[ SOCK_BUFFER_SIZE ];
	char w_buf[ SOCK_BUFFER_SIZE ];

	/* set server ip and port */
	set_server_ip_and_port( serv_ip, &serv_port );


	/* open socket */
	if( (serv_sockfd = socket( PF_INET, SOCK_STREAM, 0 )) < 0 ){
		fprintf( stderr, "Fail to open socket, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		exit(0);
	}

	/* bind socket */
	memset( &serv_addr, 0, sizeof(serv_addr) );
	serv_addr.sin_family 	= AF_INET;
	serv_addr.sin_port	= htons(serv_port);
	assert( inet_aton( serv_ip, &(serv_addr.sin_addr) ) > 0 );
	if( bind(serv_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) < 0 ){
		fprintf( stderr, "Fail to bind socket, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		exit(0);
	}

	/* listen socket */
	if( listen( serv_sockfd, DEFAULT_SERVER_BACKLOG ) < 0){
		fprintf( stderr, "Fail to listen socket, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		exit(0);
	}

	/* accept socket */
	memset( &clie_addr, 0, sizeof(clie_addr) );
	clie_sockfd = accept( serv_sockfd, (struct sockaddr *)&clie_addr, &clie_addr_size );
	if( clie_sockfd < 0 ){
		fprintf( stderr, "Fail to accept socket, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		exit(0);
	}

	/* recieve and send msg through socket */
	while(1){
		memset( r_buf, 0, sizeof(r_buf) );
		memset( w_buf, 0, sizeof(w_buf) );

		/* recv msg */
		fprintf( stdout, "Server recieving...\n");
		if( recv( clie_sockfd, r_buf, sizeof(r_buf), 0 ) <= 0 ){
			fprintf( stderr, "Fail at recv(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
			exit(0);
		}
		fprintf( stdout, "Server recieved msg:[%s]\n", r_buf );
		
		
		/* send msg */
		fprintf( stdout, "What do you want to send to client?\n");
		fscanf( stdin, "%s", w_buf );
		if( send( clie_sockfd, w_buf, strlen(w_buf), 0 ) < 0 ){
			fprintf( stderr, "Fail at send(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		}

	}

	/* close sockets */
	close(clie_sockfd);
	close(serv_sockfd);

	return 0;
}
