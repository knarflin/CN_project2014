// job.h

#ifndef JOB
#define JOB

#include<string.h>
#include<assert.h>

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

// assign the attributes of a job
void job_assign(struct job* jb, char* _src_usr, char _jobtype, int _seg_count, char* _filename, char* _content){
	assert(strlen(_src_usr)<=USERNAME_LIMIT_LEN);
	strcpy(jb->src_usr,_src_usr);
	jb->jobtype=_jobtype;
	jb->seg_count=_seg_count;
	jb->filename=_filename;
	jb->content=_content;
}

void print_job(struct job* jb){ // just for debugging
	printf("src_usr: %s\n",jb->src_usr);
	printf("jobtype: %c\n",jb->jobtype);
	printf("seg_count: %d\n",jb->seg_count);
	printf("filename: %s\n",jb->filename);
	printf("content: %s\n",jb->content);
}

#endif