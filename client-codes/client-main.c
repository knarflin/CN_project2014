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

#include "client-main.h"
#include "client-state.h"


int get_server_ip_and_port( char* serv_ip, int* serv_port ){
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
		printf( "IP and Port set to default. (%s, %d)\n", DEFAULT_SERVER_IP, DEFAULT_SERVER_PORT );
	}else{
		printf( "Invalid reply.\n");
		exit(0);
	}
	return 0;
}

int main(void){
	
	char serv_ip[20];
	int serv_port;
	int clie_sockfd;
	struct sockaddr_in serv_addr;

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

	s_offline_general( clie_sockfd );

	/* close socket*/
	close( clie_sockfd );
	return 0;
}
