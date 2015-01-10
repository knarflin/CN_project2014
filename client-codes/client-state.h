/* client-state.h */
#include <stdio.h>


#define IT_SIGNAL_UNKNOWN		0;
#define IT_SIGNAL_DSTUSER_OFFLINE	1;
#define IT_SIGNAL_DSTUSER_ONLINE	2;
#define IT_SIGNAL_LOGOUT_CONFIRMED	3;

struct it_signal { //inter-thread signal
	int value;
	struct it_signal* next;
};

/* for inter-thread communication */
#ifndef IT_SIGNAL
#define IT_SIGNAL
//static struct it_signal* _itsig_head = NULL;
//static struct it_signal* _itsig_tail = NULL;
#endif

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

