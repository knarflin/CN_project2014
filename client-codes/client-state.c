#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include "client-main.h"
#include "client-state.h"

int s_offline_general( int clie_sockfd ){
	char reply = 0;
	int quit_flag = 0;

	while( !quit_flag ){
		if(reply != ' ' && reply != '\n' ){
			printf( "What do you want to do? (L)Log in, (C)Create an account, (Q)Quit\n" );
		}
		scanf(  "%c", &reply );

		switch( reply ){
			case 'L':
			case 'l':
				if( s_offline_login( clie_sockfd ) < 0 ) return -1;
				break;
			case 'C':
			case 'c':
				if( s_offline_create( clie_sockfd ) < 0 ) return -1;
				break;
			case 'Q':
			case 'q':
				quit_flag = 1;
				break;
			case ' ':
			case '\n':
				break;
			default:
				fprintf( stderr, "Invalid reply\n" );
				break;
		}
	}
	return 0;
}

int s_offline_login( int clie_sockfd ){

	char account	[ ACCOUNT_LIMIT_LEN 	] = {0};
	char password	[ PASSWORD_LIMIT_LEN 	] = {0};
	char r_buf	[ BUFFER_SIZE		] = {0};
	char w_buf	[ BUFFER_SIZE 		] = {0};

	printf( "Enter your Account:\n" );
	scanf( "%s", account);
	
	sprintf( w_buf, "<login>" "<account>%s<\\>", account );
	if( send( clie_sockfd, w_buf, strlen(w_buf), 0) < 0 ){
		fprintf( stderr, "Fail at send(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}
	
	memset( r_buf, 0, sizeof(r_buf) );
	if( recv( clie_sockfd, r_buf, strlen("<login-A-OK>"), MSG_WAITALL ) != strlen("<login-A-OK>") ){
		fprintf( stderr, "Fail at recv(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}else if( strcmp( r_buf, "<login-FAIL>" ) == 0 ){
		printf( "No account\n" );
		return 0;
	}else if( strcmp( r_buf, "<login-A-OK>" ) != 0 ){
		fprintf( stderr, "Bad reply from server, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}
	
	printf( "Enter your Password:\n" );
	scanf( "%s", password );

	sprintf( w_buf, "<password>%s<\\>", password );
	if( send( clie_sockfd, w_buf, strlen(w_buf), 0) < 0 ){
		fprintf( stderr, "Fail at send(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}

	if( recv( clie_sockfd, r_buf, strlen("<login-P-OK>"), MSG_WAITALL ) != strlen("<login-P-OK>") ){
		fprintf( stderr, "Fail at recv(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}else if( strcmp( r_buf, "<login-FAIL>" ) == 0 ){
		printf( "Wrong password\n" );
		return 0;
	}else if( strcmp( r_buf, "<login-P-OK>" ) != 0 ){
		fprintf( stderr, "Bad reply from server, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}
	
	//TODO
	//s_online_general( clie_sock );

	return 0;
}

int s_offline_create( int clie_sockfd ){
	return 0;
};
int s_online_general();
int s_online_ftp();
/*
int s_online_msg(){
	// send and recv
	while(1){
		memset( &r_buf, 0, sizeof(r_buf) );
		memset( &w_buf, 0, sizeof(w_buf) );

		// send msg to server
		printf( "What do you want to send to server\n" );
		fscanf( stdin, "%s", w_buf );
		if( send( clie_sockfd, w_buf, strlen(w_buf), 0) < 0 ){
			fprintf( stderr, "Fail at send(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
			exit(0);
		}

		// recieve msg from server
		printf( "Client recieving...\n");
		if( recv( clie_sockfd, r_buf, sizeof(r_buf), 0 ) < 0 ){
			fprintf( stderr, "Fail at recv(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
			exit(0);
		}
		printf( "Client recieved msg:[%s]\n", r_buf );
	}
	return 0;
}
*/
