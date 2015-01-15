// job.h

#ifndef JOB
#define JOB

#include<string.h>
#include<assert.h>
#include<stdlib.h>

#ifndef USERNAME_LIMIT_LEN
#define USERNAME_LIMIT_LEN	100
#endif

struct job {
	char src_usr[ USERNAME_LIMIT_LEN ];
	char jobtype; 		// ¡¯f¡¯ , ¡®m¡¯
	int seg_count; 		// 1, 2, 3, 4, ¡­¡­ 19, 0
	char* filename;	// ¡°bgm.txt¡±
	char* content;		// ¡°........¡± (no more than 1000)
};

void print_job(struct job* jb){ // just for debugging
	printf("src_usr: %s\n",jb->src_usr);
	printf("jobtype: %c\n",jb->jobtype);
	printf("seg_count: %d\n",jb->seg_count);
	printf("filename: %s\n",jb->filename);
	printf("content: %s\n",jb->content);
}

#endif