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

char *read_line(char *buf, size_t sz);
void split(char *buf, char *split[], size_t max);

int main(){
	char str[LEN];
	char **arr;
	char *path="/bin/";
	int i=0, pid, ret,status;
	char *buf, *prg;

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

//			PRINT TEST
//			for(i=0;i<MAX_ARG && arr[i]!=NULL;i++){
//				printf("%s ",arr[i]);
//			}
			if(!strcmp(arr[0],"exit")){
				for(i=0;i<MAX_ARG;i++){
					free(arr[i]);
				}
				free(arr);
				break;
			}

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
				wait(&status);
				//printf("child exit code: %d\n", WEXITSTATUS(status));
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
	printf("> ");
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
