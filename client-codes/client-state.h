/* client-state.h */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MESSAGE_LIMIT_LEN	1000
#define FILESEG_LIMIT_LEN	1000
#define FILENAME_LIMIT_LEN	256
#define TAG_LIMIT_LEN		20


static int adjust_buffer( char* buffer, int buf_size, char** offset ){
	fprintf( stderr, "%c", **offset); //debug point
	int invalid_len = *offset - buffer;
	int valid_len = strlen( *offset );
	fprintf( stderr, "buffer size = %d, invalid = %d, valid = %d\n", buf_size, invalid_len, valid_len ); //debug point
	memmove( buffer, *offset, valid_len );
	memset( &buffer[valid_len], 0, invalid_len );
	*offset = buffer;
	return valid_len;
};

static struct wfile {
	int    fd;
	char   filename[ FILENAME_LIMIT_LEN ];
	struct wfile* next;
};

//debug point
static int print_wf( struct wfile* ptr ){
	printf(" -> ");
	if( ptr == NULL ){
		printf( "NULL\n" );
		return 0;
	}
	printf( "(%d, %s)", ptr->fd, ptr->filename );
	return print_wf( ptr->next );
}

static struct wfile* write_file( struct wfile* wf_list, char* filename, int datagram_cnt, char* content ){
	printf( "Enter write_file()\n");
	struct wfile* tmp;

	if( wf_list == NULL ){
		tmp = (struct wfile *)malloc( sizeof(struct wfile) );
		tmp -> fd = open( filename , O_CREAT | O_WRONLY | O_TRUNC, 00644 );
		if( tmp -> fd < 0 ){
			fprintf( stderr, "Fail at open(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
			return NULL;
		}
		if( write( tmp -> fd, content, strlen(content) ) != strlen(content) ){
			fprintf( stderr, "Fail at write(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
			return NULL;
		}
		strncpy( tmp -> filename, filename, strlen(filename) );
		tmp -> next = NULL;
		if( datagram_cnt == 0 ){ //last segment of the file
			fprintf( stderr, "write last file segment\n" );//debug point
			close( tmp -> fd );
			free( tmp );
			return NULL;
		}
		return tmp;
	}

	if( strcmp( wf_list -> filename, filename ) == 0 ){
		if( write( wf_list -> fd, content, strlen(content) ) != strlen(content) ){
			fprintf( stderr, "Fail at write(), %s, %d. ERROR_MSG: %s\n", __FILE__, __LINE__, strerror(errno) );
			return wf_list;
		}
		if( datagram_cnt == 0 ){ //last segment of the file
			fprintf( stderr, "write last file segment\n" );//debug point
			tmp = wf_list -> next;
			close( wf_list -> fd );
			free( wf_list );
			return tmp;
		}
		return wf_list;
	}
	
	wf_list -> next = write_file( wf_list -> next, filename, datagram_cnt, content );
	return wf_list;
}

/* for inter-thread communication ----------------------------------------------------*/

#define IT_SIGNAL_UNKNOWN		0
#define IT_SIGNAL_DSTUSER_OFFLINE	1
#define IT_SIGNAL_DSTUSER_ONLINE	2

struct it_signal { //inter-thread signal
	int value;
	struct it_signal* next;
};

//debug function
static int _itsig_printqueue( struct it_signal* ptr ){
	printf( " -> " );
	if( ptr == NULL ){
		printf("NULL\n");
		return 0;
	}
	switch( ptr->value ){
		case IT_SIGNAL_DSTUSER_OFFLINE	:
			printf( "offline" );
			break;
		case IT_SIGNAL_DSTUSER_ONLINE	:
			printf( "online" );
			break;
		default:
			break;
	}
	return _itsig_printqueue( ptr->next );
}

static int _itsig_enqueue( struct it_signal** head, struct it_signal** tail, int signal ){
	struct it_signal* tmp = (struct it_signal *)malloc( sizeof(struct it_signal) );
	tmp -> value = signal;
	tmp -> next = NULL;
	if( *tail == NULL && *head == NULL ){
		*tail = tmp;
		*head = tmp;
	}else if( *tail != NULL && *head != NULL ){
		(*tail) -> next = tmp;
		*tail = tmp;
	}else{
		fprintf( stderr, "Fail at itsig_enqueue(), strange queue\n" );
		return -1;
	}
	_itsig_printqueue( *head ); //debug point
	return 0;
}

static int _itsig_dequeue( struct it_signal** head, struct it_signal** tail ){
	if( *head == NULL ){
		fprintf( stderr, "Fail at itsig_dequeue(), queue already empty\n" );
		return -1;
	}
	struct it_signal* tmp = *head;
	*head = (*head) -> next;
	free( tmp );
	if( *head == NULL ){
		*tail = NULL;
	}
	_itsig_printqueue( *head ); //debug point
	return 0;
}


/* state functions ------------------------------------------------------------------*/

/* three offline states */
int s_offline_general();
int s_offline_login();
int s_offline_signup();

/* three online states */
int s_online_general();
int s_online_recv();
int s_online_ftp();
int s_online_msg();
int s_online_end();
