#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <pthread.h>

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
				if( s_offline_signup( clie_sockfd ) < 0 ) return -1;
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

	char username	[ USERNAME_LIMIT_LEN 	] = {0};
	char password	[ PASSWORD_LIMIT_LEN 	] = {0};
	char r_buf	[ BUFFER_SIZE		] = {0};
	char w_buf	[ BUFFER_SIZE 		] = {0};

	printf( "Enter your username:\n" );
	scanf( "%s", username);
	
	sprintf( w_buf, "<login>" "<username>%s<\\>", username );
	if( send( clie_sockfd, w_buf, strlen(w_buf), 0) < 0 ){
		fprintf( stderr, "Fail at send(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}
	
	if( recv( clie_sockfd, r_buf, strlen("<login-U-OK>"), MSG_WAITALL ) != strlen("<login-U-OK>") ){
		fprintf( stderr, "Fail at recv(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}else if( strcasecmp( r_buf, "<login-FAIL>" ) == 0 ){
		printf( "Username not exist\n" );
		return 0;
	}else if( strcasecmp( r_buf, "<login-U-OK>" ) != 0 ){
		fprintf( stderr, "Bad reply from server, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}
	
	printf( "Enter your password:\n" );
	scanf( "%s", password );

	sprintf( w_buf, "<password>%s<\\>", password );
	if( send( clie_sockfd, w_buf, strlen(w_buf), 0) < 0 ){
		fprintf( stderr, "Fail at send(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}

	if( recv( clie_sockfd, r_buf, strlen("<login-P-OK>"), MSG_WAITALL ) != strlen("<login-P-OK>") ){
		fprintf( stderr, "Fail at recv(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}else if( strcasecmp( r_buf, "<login-FAIL>" ) == 0 ){
		printf( "Wrong password\n" );
		return 0;
	}else if( strcasecmp( r_buf, "<login-P-OK>" ) != 0 ){
		fprintf( stderr, "Bad reply from server, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}
	
	return s_online_general( clie_sockfd );
}

int s_offline_signup( int clie_sockfd ){
	
	char username	[ USERNAME_LIMIT_LEN 	] = {0};
	char password	[ PASSWORD_LIMIT_LEN 	] = {0};
	char r_buf	[ BUFFER_SIZE		] = {0};
	char w_buf	[ BUFFER_SIZE 		] = {0};

	printf( "New username:\n" );
	scanf( "%s", username);
	
	sprintf( w_buf, "<signup>" "<username>%s<\\>", username );
	if( send( clie_sockfd, w_buf, strlen(w_buf), 0) < 0 ){
		fprintf( stderr, "Fail at send(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}
	
	if( recv( clie_sockfd, r_buf, strlen("<signup-U-OK>"), MSG_WAITALL ) != strlen("<signup-U-OK>") ){
		fprintf( stderr, "Fail at recv(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}else if( strcasecmp( r_buf, "<signup-FAIL>" ) == 0 ){
		printf( "Username alrealy exists\n" );
		return 0;
	}else if( strcasecmp( r_buf, "<signup-U-OK>" ) != 0 ){
		fprintf( stderr, "Bad reply from server, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}
	
	printf( "New password:\n" );
	scanf( "%s", password );

	sprintf( w_buf, "<password>%s<\\>", password );
	if( send( clie_sockfd, w_buf, strlen(w_buf), 0) < 0 ){
		fprintf( stderr, "Fail at send(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}

	if( recv( clie_sockfd, r_buf, strlen("<signup-P-OK>"), MSG_WAITALL ) != strlen("<signup-P-OK>") ){
		fprintf( stderr, "Fail at recv(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}else if( strcasecmp( r_buf, "<signup-P-OK>" ) == 0 ){
		printf( "Sign up successful\n" );
	}else{
		fprintf( stderr, "Bad reply from server, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}
	
	return 0;
};


int s_online_general( int clie_sockfd ){
	
	pthread_t recv_tid; //create a thread for recieving
	int err;
	int logout_flag = 0;
	char reply;

	err = pthread_create( &recv_tid, NULL, &s_online_recv, &clie_sockfd );
	if( err != 0 ){
		errno = (err == errno)? errno: err;
		fprintf( stderr, "Fail to create thread, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}

	while( !logout_flag ){
		if( reply != ' ' && reply != '\n' ){
			printf( "What do you want to do? (F)File Transfer, (M)Message Transfer (L)Log out\n" );
		}
		scanf( "%c", &reply );

		switch( reply ){
			case 'F':
			case 'f':
				if( s_online_ftp( clie_sockfd ) < 0 ){ return -1; }
				break;
			case 'M':
			case 'm':
				if( s_online_msg( clie_sockfd ) < 0 ){ return -1; }
				break;
			case 'L':
			case 'l':
				if( s_online_end( clie_sockfd ) < 0 ){ return -1; }
				logout_flag = 1;
				break;
			case ' ':
			case '\n':
				break;
			default:
				fprintf( stderr, "Invalid reply\n" );
				break;
		}
	}
	
	int retval;
	err = pthread_join( recv_tid, (void **)&retval ); 
	if( err != 0 ){
		errno = (err == errno)? errno: err;
		fprintf( stderr, "Fail to join thread, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}else if( retval != 0 ){
		fprintf( stderr, "Bad return form thread, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) ); //debug point
		//unknown error, only recv_thread knows
		return -1;
	}

	return 0;
}


static struct it_signal* _itsig_head = NULL;
static struct it_signal* _itsig_tail = NULL;

int s_online_recv( void* clie_sockfd_ptr ){
	int clie_sockfd = *(int *)clie_sockfd_ptr;
	printf( "Enter s_online_recv()\n" );	//debug point
	/*
	char* ptr = NULL;
	char r_buf[ DEFAULT_BUFFER_SIZE ] = {0};
	
	while(1){
		memset( r_buf, 0, sizeof(r_buf) );
		recv( clie_sockfd, r_buf, sizeof(r_buf), 0 );
		//TODO
	}
	*/
	return 0;
}


//send a logout message to server
int s_online_end( int clie_sockfd ){
	printf( "Enter s_online_end()\n" );	//debug point
	if( send( clie_sockfd, "<logout>", strlen("<logout>"), 0 ) <= 0 ){
		fprintf( stderr, "Fail at send <logout>, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}
	return 0;
};

int s_online_ftp( int clie_sockfd ){
	printf( "Enter s_online_ftp()\n" );	//debug point
	//TODO
	return 0;
}

int s_online_msg( int clie_sockfd ){
	printf( "Enter s_online_msg()\n" );	//debug point
	/*TODO
	char w_buf[ DEAULT_BUFFER_SIZE ] = {0};

	printf( "Who do you want to send\n" );
	fscanf( stdin, "%s", w_buf );

	if( send( clie_sockfd, w_buf, strlen(w_buf), 0) <= 0 ){
		fprintf( stderr, "Fail to send dst_usr, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}

	if( recv( clie_sockfd, r_buf, sizeof(r_buf), 0 ) <= 0 ){
		fprintf( stderr, "Fail at recv(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}

	printf( "Client recieved msg:[%s]\n", r_buf );
	*/
	return 0;
}
