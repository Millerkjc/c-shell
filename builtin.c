#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "builtin.h"

#define MAX_ARG 10
#define N_FUNC 2

func_dict func_d[N_FUNC] = {{"exit",&exit_builtin},{"cd",&cd_builtin}};

int builtin_f(func_dict func_d[], char *str){
	int i;
	for(i=0;i<N_FUNC;i++){
		if(!strcmp(func_d[i].name,str)){
			return i;
		}
	}
	return -1;
}


void cd_builtin(char **arr){
	char *path = arr[1];
	int ret;

	if (path == NULL){
		path = getenv("HOME");
	}

	ret = chdir(path);

	if(ret==-1){
		fprintf(stderr, "%s: %s\n", path, strerror(errno));
	}   
}

void exit_builtin(char **arr){
	int i;
	for(i=0;i<MAX_ARG;i++){
		free(arr[i]);
	}   
	free(arr);
	printf("Exiting..\n");
	exit(0);
}
