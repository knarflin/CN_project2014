// parse.h

/*
parse() parses a command from a string *src.
If *src does not contain any complete command (<tag>content<\> or <tag>), 
like "ssword>crispy<\", then parse() will return 0.
Otherwise it will parse the first complete command from the beginning, 
making
dest[0]=tag
dest[1]=content,
set *src to the first character following the command, and return 1.

Input:
char** src: input string
char** dest: output strings
int* intptr1: output file descriptor
int* intptr2: output datagramcount
int* isfiledata: whether the tag starts with "filedata"

---------------------------------Sample 1---------------------------------
Input:
*src = "<account>Ruby<\\> <password>crispy<\\> <nam"

Output:
dest[0]="account";
dest[1]="Ruby";
*isfiledata = 0
return value: 1

Side effect:
*src points to the space on the left of "<password>"

---------------------------------Sample 2---------------------------------
Input:
*src = "ount>Ruby<\\> <password>crispy<\\> <nam"

Output:
dest[0]="password";
dest[1]="crispy";
*isfiledata = 0
return value: 1

Side effect:
*src points to the space on the left of "<nam"

---------------------------------Sample 3---------------------------------
Input:
*src = "ssword>crispy<\"

Output:;
*isfiledata = 0
return value: 0

*src is not changed

*/

/*

---------------------------------Sample 4---------------------------------
Input:
*src = "<filedata,2,15>Hello world!<\\>"

Output:;
dest[0]="filedata,2,15";
dest[1]="Hello world!";
*intptr1 = 2
*intptr2 = 15
*isfiledata = 1
return value: 1

Side effect:
*src points to the character '0' at the end of the string

*/

/*

Sample messege:

<signup>
<fail>
<ok>
<login>
<logout>

*/

///////////////////////////// Code begins here /////////////////////////////

#ifndef PARSE
#define PARSE

#include<string.h>
#include<stdio.h>
#include"mystring.h"
#include<assert.h>

// When changing tag_only words, two code segments needs to be modified:

const char* tag_only[]={ // modifiable 1/2
	"signup",
	"fail",
	"ok",
	"login",
	"logout"
};

const int tag_only_count=5; // modifiable 2/2

// input: src: command string
// output: dest[0]:tag, dest[1]:content
// return value: 0 for failed, 1 for succeeded
int parse(char** src, char** dest, char* filename, int* datagram_cnt, int* isfiledata)
{
	char tmpchar;
	int ptr=0;
	int i;
	int tagonly;

	char* readpos=*src;
	char* tempptr;

	if(isfiledata!=NULL)
		*isfiledata=0;

//--------------------------- Step 1: parse tag ---------------------------

	// ignore the characters before the first '<'
	while(1){
		tmpchar=readpos[0];
		readpos++;
		if(tmpchar=='<'){break;}
		else if(tmpchar==0){return 0;}
	}

	// get tag
	
	while(1) {
		dest[0][ptr] = readpos[0];
		readpos++;
		if(dest[0][ptr] == '>') {dest[0][ptr] = 0; break;}
		else if(dest[0][ptr]==0) {return 0;}
		else{ptr++;}
	}
	
//--------------------------- Step 2: process the tag ---------------------------

	// decide whether the tag is "filedata"
	tempptr=dest[0];
	if(startwith(tempptr,"filedata")){
		assert(filename!=NULL);
		assert(datagram_cnt!=NULL);
		assert(isfiledata!=NULL);
		*isfiledata=1;
		tempptr+=9;
		readuntil(&tempptr,filename,',');
		*datagram_cnt=myatoi(&tempptr,0);
	}

	// decide whether the tag is "tagonly" without content
	tagonly=0;
	for(i=0;i<tag_only_count;i++){
		if(strcmp(dest[0],tag_only[i])==0){
			tagonly=1;
			break;
		}
	}

	if(tagonly){
		dest[1][0]=0;
		*src=readpos;
		return 1;
	}

//--------------------------- Step 3: parse content ---------------------------

	// get content
	ptr = 0;
	while(1) {
		dest[1][ptr] = readpos[0];
		readpos++;
		if(dest[1][ptr]=='<' && readpos[0]=='\\' && readpos[1]=='>'){
			dest[1][ptr]=0;
			break;
		}
		else if(dest[1][ptr]=='\r'||dest[1][ptr]=='\n'){
			dest[1][ptr]='\r';
			dest[1][++ptr]='\n';
		}
		else if(dest[1][ptr]==0){return 0;}
		else{ptr++;}
	}

	*src=readpos+2;
	return 1;
}

#endif