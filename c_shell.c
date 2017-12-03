#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define LEN 80
#define MAX_ARG 10
#define LEN_ARG 10
#define N_FUNC 2

typedef struct{
	char *name;
	void (*f_builtin)();
}func_dict;

char *read_line(char *buf, size_t sz);
void split(char *buf, char *split[], size_t max);
int builtin_f(func_dict func_d[], char *str);
void exit_builtin(char **arr);
void cd_builtin(char **arr);
//void cd_builtin(char *path);
int background(char **arr);

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
						waitpid(pid, &status, WNOHANG);
					}else{
						wait(&status);
					}
					//for & -> waitpid(pid, &status, WNOHANG);
					//waitpid(0, &status, WNOHANG);
					//waitpid - WNOHANG

					//printf("child exit code: %d\n", WEXITSTATUS(status));
				}
			}
			// ERRORE FREE
			for(i=0;i<MAX_ARG;i++){
				free(arr[i]);
			}
			free(arr);
		}
	}

	printf("Exiting..\n");

	return 0;
}

char *read_line(char *buf, size_t sz){
	char *curDir = getcwd(NULL,0);
	if(!curDir){
		fprintf(stderr, "getcwd: %s\n", strerror(errno));
		exit(1);
	}
	printf("%s> ", curDir);
	free(curDir);
	return fgets(buf, sz, stdin);
}

void split(char *buf, char *split[], size_t max){
	const char *s = " \n";
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

//void cd_builtin(char *path){
void cd_builtin(char **arr){
	char *path = arr[1];
	int ret =chdir(path);
	if(ret==-1){
		fprintf(stderr, "%s: %s\n", path, strerror(errno));
	}
}


int background(char **arr){
	int i;
	for(i=1;arr[i]!=NULL && i<MAX_ARG;i++){
		if(strstr(arr[i],"&")!=NULL){
			arr[i][strlen(arr[i])-1]='\0';
			printf("%s\n",arr[i]);
			return i;
		}else if(!strcmp(arr[i],"&")){
			arr[i] = NULL;
			return i;
		}
	}
	return -1;
}
