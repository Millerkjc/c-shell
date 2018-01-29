#include <stdio.h>
#include <errno.h>
#include "builtin.h"

void cd_builtin(char **arr){
	char *path = arr[1];
	int ret =chdir(path);
	if(ret==-1){
		fprintf(stderr, "%s: %s\n", path, strerror(errno));
	}   
}
