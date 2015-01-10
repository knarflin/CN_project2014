/* client-state.h */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define FILENAME_LIMIT_LEN	256;
#define TAG_LIMIT_LEN		20;
#define MAX_WRITE_FD_COUNT	100;


int adjust_buffer( char* buffer, char** offset ){
	int buf_size = sizeof( buffer );
	int invalid_len = *offset - buffer;
	int valid_len = buf_size - invalid_len;
	memmove( buffer, *offset, valid_len );
	memset( buffer[valid_len], 0, invalid_len );
	*offset = buffer;
	return valid_len;
};

struct wfile {
	int    fd;
	char   filename[ FILENAME_LIMIT_LEN ];
	struct wfile* next;
};

/*TODO
write_file( fd-filename-map, filename, datagram_cnt, ){
	printf( "Enter write_file()\n");
	return 0;
}
*/

/* for inter-thread communication ----------------------------------------------------*/

#define IT_SIGNAL_UNKNOWN		0;
#define IT_SIGNAL_DSTUSER_OFFLINE	1;
#define IT_SIGNAL_DSTUSER_ONLINE	2;
#define IT_SIGNAL_LOGOUT_CONFIRMED	3;

struct it_signal { //inter-thread signal
	int value;
	struct it_signal* next;
};

int itsig_enqueue( struct it_signal** head, struct it_signal** tail, int siganl ){
	struct it_signal* tmp = (struct it_signal *)malloc( sizeof(struct it_signal) )
	tmp -> value = signal;
	tmp -> next = NULL;
	//TODO mutex lock
	if( *tail == NULL && *head == NULL ){
		*tail = tmp;
		*head = tmp;
	}else( *tail != NULL && *head != NULL ){
		tail -> next = tmp;
		*tail = tmp;
	}else{
		fprintf( stderr, "Fail at itsig_enqueue(), strange queue\n" );
	}
	//TODO mutex unlock
	return 0;
}

int itsig_dequeue( struct it_signali** head, struct it_signal* tail, int siganl ){
	if( *head == NULL ){
		fprintf( stderr, "Fail at itsig_dequeue(), queue already empty\n" );
		return -1;
	}
	
	//TODO mutex lock
	//TODO mutex unlock
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

