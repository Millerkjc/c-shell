#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "builtin.h"

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
