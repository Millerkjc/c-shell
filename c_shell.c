#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

//My library
#include "builtin.h"
#include "shell_utility.h"

#define LEN 80
#define LEN_ARG 10
#define N_FUNC 2

typedef struct{
	char *name;
	void (*f_builtin)();
}func_dict;

int builtin_f(func_dict func_d[], char *str);
void exit_builtin(char **arr);

int main(){
	char str[LEN];
	char **arr;
	char *path="/bin/";
	int i=0, pid, ret,status;
	int built_ret;
	char *buf, *prg;
	int bkgnd;

	//char builtin = {&f1, &f2,...}
	//char builtin = {&}
	//void (*f_builtin)() = NULL;
	func_dict func_d[N_FUNC] = {{"exit",&exit_builtin},{"cd",&cd_builtin}};

	while(read_line(str,LEN)!=NULL){
		if(strcmp(str,"\n")){
			arr = malloc(MAX_ARG*sizeof(char*));
			if(arr == NULL){
				exit(1);
			}

			for(i=0;i<MAX_ARG;i++){
				arr[i] = malloc(LEN_ARG);
				if(arr[i] == NULL){
					exit(1);
				}
			}

			split(str,arr,MAX_ARG);
			bkgnd = background(arr);

//			PRINT TEST
//			for(i=0;i<MAX_ARG && arr[i]!=NULL;i++){
//				printf("%s ",arr[i]);
//			}

			built_ret =builtin_f(func_d,arr[0]);
			
			if(built_ret!=-1){
				func_d[built_ret].f_builtin(arr);
			}else{

				pid = fork();
				if(pid<0){
					printf("Cannot fork\n");
					exit(1);
				}

				if(pid == 0){
					prg = malloc(strlen(arr[0]) + 1);
					if(prg == NULL){
						fprintf(stderr,"2 prg malloc fails\n");
						exit(1);
					}

					if(!strstr(arr[0],path)){
						//buf = malloc(strlen(path) + strlen(arr[0]) + 1);
						// error in strcat
						buf = calloc(1, strlen(path) + strlen(arr[0]) + 1);
						if(buf == NULL){
							fprintf(stderr,"2 buf malloc fails\n");
							free(prg);
							exit(1);
						}
						strcpy(prg,arr[0]);
						strcat(buf, path);
						strcat(buf,arr[0]);
						strcpy(arr[0],buf);
						free(buf);
					}
					free(prg);

					ret = execv(arr[0], arr);
					if(ret == -1){
						fprintf(stderr, "%s: %s\n", prg, strerror(errno));
						exit(1);
					}

				}else{
					if(bkgnd!=-1){
						//waitpid(pid, &status, WNOHANG);
						waitpid(pid, NULL, WNOHANG);
						free_arr(arr);
					}else{
						wait(&status);
						free_arr(arr);
					}
					//for & -> waitpid(pid, &status, WNOHANG);
					//waitpid(0, &status, WNOHANG);
					//waitpid - WNOHANG

					//printf("child exit code: %d\n", WEXITSTATUS(status));
				}
			}
			// ERRORE FREE
			//free_arr(arr);
		}
	}

	printf("Exiting..\n");

	return 0;
}

int builtin_f(func_dict func_d[], char *str){
	int i;
	for(i=0;i<N_FUNC;i++){
		if(!strcmp(func_d[i].name,str)){
			return i;
		}
	}
	return -1;
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
