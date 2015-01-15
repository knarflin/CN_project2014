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
#include "client-parse.h"

//inter-thread communication, between s_online_general() and s_online_recv()
static struct it_signal* _itsig_head = NULL;
static struct it_signal* _itsig_tail = NULL;
static pthread_mutex_t _itsig_lock;

/* three offline states */
int s_offline_general();
int s_offline_login();
int s_offline_signup();

/* three online states */
int s_online_general();
int s_online_knock();
int s_online_recv();
int s_online_ftp();
int s_online_msg();
int s_online_end();



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
	printf( "Enter your password:\n" );
	scanf( "%s", password );
	
	sprintf( w_buf, "<login>" "<username>%s<\\>" "<password>%s<\\>", username, password );

	if( send( clie_sockfd, w_buf, strlen(w_buf), 0) < 0 ){
		fprintf( stderr, "Fail at send(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}
	
	if( recv( clie_sockfd, r_buf, strlen("<login-good>"), MSG_WAITALL ) != strlen("<login-good>") ){
		fprintf( stderr, "Fail at recv(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}else if( strcasecmp( r_buf, "<login-badU>" ) == 0 ){
		printf( "Username not exist\n" );
		return 0;
	}else if( strcasecmp( r_buf, "<login-badP>" ) == 0 ){
		printf( "Wrong password\n" );
		return 0;
	}else if( strcasecmp( r_buf, "<login-good>" ) != 0 ){
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
	printf( "New password:\n" );
	scanf( "%s", password );
	
	sprintf( w_buf, "<signup>" "<username>%s<\\>" "<password>%s<\\>", username, password );
	if( send( clie_sockfd, w_buf, strlen(w_buf), 0) < 0 ){
		fprintf( stderr, "Fail at send(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}
	
	if( recv( clie_sockfd, r_buf, strlen("<signup-good>"), MSG_WAITALL ) != strlen("<signup-good>") ){
		fprintf( stderr, "Fail at recv(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}else if( strcasecmp( r_buf, "<signup-badU>" ) == 0 ){
		printf( "Username alrealy exists\n" );
	}else if( strcasecmp( r_buf, "<signup-good>" ) == 0 ){
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
	
	//init _itsig mutex
	pthread_mutex_init( &_itsig_lock, NULL );
	
	//Create thread for recieving
	err = pthread_create( &recv_tid, NULL, &s_online_recv, &clie_sockfd );
	if( err != 0 ){
		errno = (err == errno)? errno: err;
		fprintf( stderr, "Fail to create thread, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}

	//Ask user and provide service through function call.
	while( !logout_flag ){
		if( reply != ' ' && reply != '\n' ){
			printf( "What do you want to do? (F)File Transfer, (M)Message Transfer (L)Log out\n" );
		}
		scanf( "%c", &reply );

		switch( reply ){
			case 'F':
			case 'f':
				//file transfer
				if( s_online_ftp( clie_sockfd ) < 0 ){ return -1; }
				break;
			case 'M':
			case 'm':
				//message sending
				if( s_online_msg( clie_sockfd ) < 0 ){ return -1; }
				break;
			case 'L':
			case 'l':
				//log out
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
	
	//Wait for recieving-thread to terminate
	int retval;
	err = pthread_join( recv_tid, (void **)&retval ); 
	if( err != 0 ){
		errno = (err == errno)? errno: err;
		fprintf( stderr, "Fail to join thread, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}else if( retval != 0 ){
		fprintf( stderr, "Bad return form thread, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) ); //debug point
		return -1;
	}

	return 0;
}


int s_online_recv( void* clie_sockfd_ptr ){
	int clie_sockfd = *(int *)clie_sockfd_ptr;
	printf( "Enter s_online_recv()\n" );	//debug point
	
	char  r_buf[ BUFFER_SIZE ] = {0};
	int   buf_valid_len = 0;

	char  filename[ FILENAME_LIMIT_LEN ];
	int   datagram_cnt;
	int   isfiledata;
	char  tag[ TAG_LIMIT_LEN ], content[ FILESEG_LIMIT_LEN ];
	char* dest[2] = { tag, content };
	char* pos = r_buf;

	char  src_usr[ USERNAME_LIMIT_LEN ] = {0};
	struct file* wf_list = NULL;
	int logout_flag = 0;

	while( !logout_flag ){
		if( recv( clie_sockfd, &r_buf[buf_valid_len], sizeof(r_buf) - buf_valid_len - 1, 0 ) <= 0 ){
			fprintf( stderr, "Fail at recv(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
			return -1;
		}
		while( parse( &pos, dest, filename, &datagram_cnt, &isfiledata ) != 0 ){

			if( isfiledata ){
				wf_list = write_file( wf_list, filename, datagram_cnt, dest[1] );
				print_fl( wf_list ); //debug point
			}else if( strcasecmp( dest[0], "logout-confirmed" ) == 0 ){
				logout_flag = 1;
			}else if( strcasecmp( dest[0], "user-online" ) == 0 ){
				if( pthread_mutex_lock( &_itsig_lock ) != 0 ){
					fprintf( stderr, "Fail locking, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
					return -1;
				}
				_itsig_enqueue( &_itsig_head, &_itsig_tail, IT_SIGNAL_DSTUSER_ONLINE );
				if( pthread_mutex_unlock( &_itsig_lock ) != 0 ){
					fprintf( stderr, "Fail unlocking, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
					return -1;
				}
			}else if( strcasecmp( dest[0], "user-offline" ) == 0 ){
				if( pthread_mutex_lock( &_itsig_lock ) != 0 ){
					fprintf( stderr, "Fail locking, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
					return -1;
				}
				_itsig_enqueue( &_itsig_head, &_itsig_tail, IT_SIGNAL_DSTUSER_OFFLINE );
				if( pthread_mutex_unlock( &_itsig_lock ) != 0 ){
					fprintf( stderr, "Fail unlocking, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
					return -1;
				}
			}else if( strcasecmp( dest[0], "name" ) == 0 ){
				sprintf( src_usr, "%s", dest[1] );
			}else if( strcasecmp( dest[0], "message") == 0 ){
				printf( "MSG FROM %s: [%s]\n", src_usr, dest[1] );
			}
		}
		buf_valid_len = adjust_buffer( r_buf, sizeof(r_buf), &pos );
		//fprintf( stderr, "s_online_recv(): buf_valid_len = %d\n", buf_valid_len ); //debug point
	}
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

	int  file_count = 0;
	int  read_len	= 0;
	int  i;
	char reply 	= ' ';
	char r_buf	[ BUFFER_SIZE		]	= {0};
	char w_buf	[ BUFFER_SIZE 		] 	= {0};
	char filepath	[ FILENAME_LIMIT_LEN	]	= {0};
	char dst_usr	[ USERNAME_LIMIT_LEN 	] 	= {0};
	struct file* rf_list = NULL;
	struct file* rf_ptr  = NULL; //for traversing
	struct file* rf_tmp  = NULL; //for freeing
	
	//Start knocking.
	printf( "Who do you want the files be transfered to? (We will check if he/she is online)\n" );
	scanf( "%s", dst_usr );
	if( s_online_knock( clie_sockfd, dst_usr ) == -1 ){
		return -1;
	}

	printf( "How many files do you want to send?\n" );
	scanf( "%d", &file_count );
	for( i=0; i<file_count; i++ ){
		printf( "Enter the filename under cwd or absolute path of file #%d : ", i ); //absolute path not yet supported, pathtoname()
		scanf( "%s", filepath );
		rf_list = insert_fl( rf_list, filepath );
	}
	
	printf( "Transfering files to %s ...\n",  dst_usr );

	while( rf_list != NULL ){
		for( rf_ptr = rf_list; rf_ptr != NULL; ){
			read_len = read( rf_ptr->fd, r_buf, FILESEG_LIMIT_LEN );
			if( read_len < 0 ){
				//read() error
				fprintf( stderr, "Fail at read(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
				return -1;
			}else if( read_len == 0 ){
				//EOF
				sprintf( w_buf, "<username>%s<\\>" "<filedata,%s,%d><\\>", dst_usr, rf_ptr->filename, 0 );
				if( send( clie_sockfd, w_buf, strlen(w_buf), 0 ) <= 0 ){
					fprintf( stderr, "Fail transfering, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
				}
				rf_tmp = rf_ptr;
				rf_ptr = rf_ptr->next;
				printf( "Complete transmition of file '%s'\n", rf_tmp->filepath );
				rf_list = remove_fl( rf_list, rf_tmp->filepath );
			}else{
				//not EOF
				sprintf( w_buf, "<username>%s<\\>" "<filedata,%s,%d>%s<\\>", 
						dst_usr, rf_ptr->filename, rf_ptr->datagram_cnt, r_buf );
				if( send( clie_sockfd, w_buf, strlen(w_buf), 0 ) <= 0 ){
					fprintf( stderr, "Fail transfering, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
				}
				rf_ptr->datagram_cnt ++;
				rf_ptr = rf_ptr->next;
			}
		}
	}
	printf( "ALL files tranfered.\n" );
	//TODO debug!!
	return 0;
}

int s_online_knock( int clie_sockfd, char* dst_usr ){

	int  tmp_itsig 	= IT_SIGNAL_UNKNOWN;
	char w_buf	[ BUFFER_SIZE 		] 	= {0};
	
	//Send knocking.
	sprintf( w_buf, "<knock>%s<\\>", dst_usr );
	if( send( clie_sockfd, w_buf, strlen(w_buf), 0) <= 0 ){
		fprintf( stderr, "Fail to send dst_usr, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
		return -1;
	}
	
	//Take _itsig from recieving-thread to check friend is online or not.
	//Busy looping!! Ineffective!!!
	while( tmp_itsig == IT_SIGNAL_UNKNOWN ){
		pthread_mutex_lock( &_itsig_lock );
		if( _itsig_head != NULL){
			tmp_itsig = _itsig_head -> value;
			_itsig_dequeue( &_itsig_head, &_itsig_tail );
		}
		pthread_mutex_unlock( &_itsig_lock );
	}

	//Show friend's online status.
	switch( tmp_itsig ){
		case IT_SIGNAL_DSTUSER_OFFLINE	:
			printf( "Your friend is currently offline.\n" );
			break;
		case IT_SIGNAL_DSTUSER_ONLINE	:
			printf( "Your friend is currently online.\n" );
			break;
		default:
			fprintf( stderr, "Invalid tmp_itsig '%d', %s, %d. ERROR_MSG: %s\n", tmp_itsig, __FILE__, __LINE__, strerror(errno));
			return -1;
	}
	return 0;
}

int s_online_msg( int clie_sockfd ){

	int  input_len	= 0;
	char reply 	= ' ';
	char w_buf	[ BUFFER_SIZE 		] 	= {0};
	char input	[ MESSAGE_LIMIT_LEN+1	] 	= {0};
	char dst_usr	[ USERNAME_LIMIT_LEN 	] 	= {0};
	
	//Start knocking.
	printf( "Who do you want to send? (We will check if he/she is online)\n" );
	scanf( "%s", dst_usr );
	if( s_online_knock( clie_sockfd, dst_usr ) == -1 ){
		return -1;
	}
	
	//Check if user want to send msg. Return if not.
	printf( "Send msg or not? (Y/N)\n" );
	while(1){
		reply = fgetc( stdin );
		if( reply == ' ' || reply == '\n' ){
			continue;
		}else if( reply == 'N' || reply == 'n' ) {
			return 0;
		}else if( reply == 'Y' || reply == 'y' ){
			printf( "Please type the message: (Terminate when EOF is encountered.)\n" );
			break;
		}else{
			fprintf( stderr, "Invalid reply, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno));
			return 0;
		}
	}
		
	//Sending msg (Press EOF to stop sending)
	while( fgets( input, sizeof(input), stdin ) != NULL ){
		input_len = strlen(input);
		if(( input_len == 0 ) || ( input_len == 1 && input[0] == '\n')){ continue; }
		input[ input_len-1 ] = '\0'; //replace '\n' with '\0'
		sprintf( w_buf, "<username>%s<\\><message>%s<\\>", dst_usr, input );
		if( send( clie_sockfd, w_buf, strlen(w_buf), 0) <= 0 ){
			fprintf( stderr, "Fail to send dst_usr, %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
			return -1;
		}
	}
	
	//Send ends.
	fprintf( stderr, "Fail at getline() , %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) ); //debug point
	return 0;
}
