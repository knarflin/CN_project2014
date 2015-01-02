// parse.h

/*
parse() parses a command from a string *src.
If *src does not contain any complete command (<tag>content<\> or <tag>), 
like "ssword>crispy<\", then parse() will return 0.
Otherwise it will parse the first complete command from the beginning, 
making
dest[0]=tag
dest[1]=content,
set *src to the first character following the commanc, and return 1.

---------------------------------Sample 1---------------------------------
Input:
*src = "<account>Ruby<\\> <password>crispy<\\> <nam"

Output:
dest[0]="account";
dest[1]="Ruby";
return value: 1

Side effect:
*src points to the space on the left of "<password>"

---------------------------------Sample 2---------------------------------
Input:
*src = "ount>Ruby<\\> <password>crispy<\\> <nam"

Output:
dest[0]="password";
dest[1]="crispy";
return value: 1

Side effect:
*src points to the space on the left of "<nam"

---------------------------------Sample 3---------------------------------
Input:
*src = "ssword>crispy<\"

Output:;
return value: 0

*src is not changed

*/

///////////////////////////// Code begins here /////////////////////////////

#include<string.h>
#include<stdio.h>

const char* tag_only[]={
	"signup",
	"fail",
	"ok",
	"login",
	"logout"
};

const int tag_only_count=5;

// input:  src: command string
// output: dest[0]:tag, dest[1]:content
// return value: 0 for failed, 1 for succeeded
int parse(char** src, char** dest)
{
	char tmpchar;
	int ptr=0;
	int i;
	int tagonly;

	char* readpos=*src;

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

	*src=readpos;
	return 1;
}

