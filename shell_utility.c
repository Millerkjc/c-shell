#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "shell_utility.h"

#define MAX_ARG 10

char *read_line(char *buf, size_t sz){
	char *curDir = getcwd(NULL,0);
	if(!curDir){
		fprintf(stderr, "getcwd: %s\n", strerror(errno));
		exit(1);
	}
	//printf("%s> ", curDir);
	printf("Too> ");
	free(curDir);
	return fgets(buf, sz, stdin);
}

void split(char *buf, char *split[], size_t max){
	const char *s = "& \n";
	size_t i=0;

	buf = strtok(buf,s);

	while(buf != NULL && i<max-1) {
		*(buf + strlen(buf)) = '\0';
		strcpy(split[i], buf);
		i+=1;
		buf = strtok(NULL, s);
	}
	split[i]=NULL;
}


int background(char **arr){
	int i;
	for(i=1;arr[i]!=NULL && i<MAX_ARG;i++){
		if(strstr(arr[i],"&")!=NULL){
			arr[i][strlen(arr[i])-1]='\0';
			return i;
		}else if(!strcmp(arr[i],"&")){
			arr[i] = NULL;
			return i;
		}
	}   
	return -1; 
}


void free_arr(char **arr){
	int i;
	for(i=0;i<MAX_ARG;i++){
		free(arr[i]);
	}   
	free(arr);
}

