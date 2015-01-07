#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#ifndef	DEFAULT_SERVER_PORT
#define DEFAULT_SERVER_PORT	28976
#endif

#ifndef	DEFAULT_SERVER_IP
#define	DEFAULT_SERVER_IP	"140.112.30.50" //linux17.csie.org
#endif

#define BUFFER_SIZE	1000


void get_server_ip_and_port( char* serv_ip, int* serv_port ){
	char reply;
	printf( "What's remote server's IP and Port, hand-set or default?(H/D)\n" );
	scanf("%c", &reply);
	if( reply == 'H' || reply == 'h' ){
		printf( "Enter IP: ");
		scanf( "%s", serv_ip ); //A string like "100.200.0.1"
		printf( "\nEnter Port: ");
		scanf( "%d", serv_port ); //A port number for listen
		printf( "\n" );
	}else if(reply == 'D' || reply == 'd' ){
		strncpy( serv_ip, DEFAULT_SERVER_IP, strlen(DEFAULT_SERVER_IP) );
		*serv_port = DEFAULT_SERVER_PORT;
		printf( "IP and Port set to default. (140.112.30.50, 28976)\n");
	}else{
		printf( "Invalid reply.\n");
		exit(0);
	}
	return;
}

int main(void){
	
	char serv_ip[20];
	int serv_port;
	int clie_sockfd;
	struct sockaddr_in serv_addr;
	char r_buf[ BUFFER_SIZE ];
	char w_buf[ BUFFER_SIZE ];

	/* set server ip and port */
	get_server_ip_and_port( serv_ip, &serv_port );

	/* open socket */
	if( (clie_sockfd = socket( PF_INET, SOCK_STREAM, 0 )) < 0 ){
		fprintf( stderr, "Fail to open socket, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		exit(0);
	}

	/* connect socket */
	memset( &serv_addr, 0, sizeof(serv_addr) );
	serv_addr.sin_family	= AF_INET;
	serv_addr.sin_port	= htons( serv_port );
	assert( inet_aton( serv_ip, &(serv_addr.sin_addr) ) > 0 );
	if( connect( clie_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) < 0 ){
		fprintf( stderr, "Fail to connect socket, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		exit(0);
	}

	/* send and recv */
	while(1){
		memset( &r_buf, 0, sizeof(r_buf) );
		memset( &w_buf, 0, sizeof(w_buf) );

		/* send msg to server*/
		printf( "What do you want to send to server\n" );
		fscanf( stdin, "%s", w_buf );
		if( send( clie_sockfd, w_buf, strlen(w_buf), 0) < 0 ){
			fprintf( stderr, "Fail at send(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
			exit(0);
		}

		/* recieve msg from server*/
		printf( "Client recieving...\n");
		if( recv( clie_sockfd, r_buf, sizeof(r_buf), 0 ) < 0 ){
			fprintf( stderr, "Fail at recv(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
			exit(0);
		}
		printf( "Client recieved msg:[%s]\n", r_buf );
	}
	
	/* close socket*/
	close( clie_sockfd );
	return 0;
}


