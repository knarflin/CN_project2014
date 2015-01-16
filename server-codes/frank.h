#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>


int readyforread( int fd ){ //To test if the socket has something to read, Write by Frank
	struct timeval tv;
	int ready;
	fd_set readset;
	tv.tv_usec = 5000; 
	FD_ZERO( &readset ); 
	FD_SET( fd, &readset );
	
	ready = select( fd+1, &readset, NULL, NULL, tv );
	if( ready < 0 ){
		fprintf( stderr, "Fail at select(), %s, %d. ERROR_MSG :%s\n", __FILE__, __LINE__, strerror(errno) );
	}else if( ready == 0 ){
		return 0;
	}else if( ready == 1 ){
		return 1;
	}else{
		fprintf( stderr, "Bad return from select(), %s, %d.\n", __FILE__, __LINE__ );
	}
	return -1;
};

int handle_job( int conn_fd, char* dst_usr ){
	struct job* jptr;
	int ret = job_get( dst_usr,  );

	return 0;
}

