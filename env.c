#include "global.h"

void setEnv(char* var1, char* var2)
{
	int ret = setenv(var1, var2, 1);
	if( ret == -1 )
		printf("environment variable not set");
	else
		printf("environment variable of %s set to: %s\n" , var1 , var2);
}

void printEnv(char *name)
{
	if(name == NULL)
	{
		int i = 0;
		while(environ[i])
 			printf("%s\n", environ[i++]);
	}
	else
	{
		char *val;
		val = getenv(name);
		printf("the variable is %s\n",val);
	}
}

void unsetEnv(char *name)
{
	int ret = 0;
	ret = unsetenv(name);
	if(ret == -1)
		printf("unset failed");
}
