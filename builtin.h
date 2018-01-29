//Builting shell function

typedef struct{
	char *name;
	void (*f_builtin)();
}func_dict;

extern func_dict func_d[];

int builtin_f(func_dict func_d[], char *str);

void cd_builtin(char **arr);
void exit_builtin(char **arr);
