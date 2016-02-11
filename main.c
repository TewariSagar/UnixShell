#include "global.h"

int main (void)
{
	counter = 1;
	aliasPtr = createAliasList();
	waiter = TRUE;
	mainLoop = TRUE;
	aliasLock = FALSE;
	argArray = NULL;
	inRedir = NULL;
	outRedir = NULL;
	errRedir = NULL;

	while(mainLoop)
	{
		printf("[ %d ]: ", counter);
		yyparse();
		counter++;
	}
}



void yyerror (char *s)
{
	fprintf (stderr, "%s\n", s);
} 

char* strTrim(char *str, int fr, int bk)
{
	int len = strlen(str);
	char* tmp = (char *)malloc(sizeof(char) * len - fr - bk); 
	strncpy(tmp, str+fr, len - fr - bk);
	tmp[len - fr - bk + 1] = '\0';
	//printf("DEBUG strTrim: %s\n",tmp);
	return tmp;
}


void cdCall(int command, char *dir)
{
	if(dir == NULL)
	{
		switch(command) //No argument
		{
			//Root
			case(0): {
				chdir(getenv("HOME"));
				setenv("PWD", getenv("HOME"), 1);
			} break;
			
			//Previous Directory
			case(1): {
				dir = getenv("PWD");
				int len = strlen(dir) - 1;	
				while(dir[len] != '/') len--;
				size_t allocSize = sizeof(char) * (len + 1);
				char * temp = (char *)malloc(allocSize);
				strncpy(temp,dir,len);
				int debug = chdir(temp);
				//printf("DEBUG VALUE: %d\n",debug);
				setenv("PWD", temp, 1);
				free(temp);
			} break;
		}	
	}
	else
	{
		switch(command) //Argument
		{
			//Start from current/root
			case(0): {
				int debug = -2;
				if(dir[0] == '/') 
				{
					debug = chdir(dir);
					if(debug == 0) setenv("PWD", dir, 1);
				}
				else {
					if(dir[0] == '.' && dir[1] == '/')
						dir = strTrim(dir, 2, 0);
					char * temp = getenv("PWD");
					strcat(temp, "/");
					strcat(temp, dir);
					//printf("DEBUG: %s\n", temp);
					debug = chdir(temp);
					if(debug == 0) setenv("PWD", temp, 1);
				}
				//printf("0 - DEBUG:%d - PWD: %s\n", debug, getenv("PWD"));
			} break;

			//Start from previous
			case(1):
			{
				char * cur = getenv("PWD");
				int len = strlen(cur) - 1;	
				while(cur[len] != '/') len--;
				size_t allocSize = sizeof(char) * (len + 1);
				char * prev = (char *)malloc(allocSize);
				strncpy(prev,cur,len);
				strcat(prev, dir);
				int debug = chdir(prev);
				//printf("DEBUG VALUE: %d\n",debug);
				if(debug == 0) setenv("PWD", prev, 1);
				free(prev);
			} break;

			//Start from current
			case(2):
			{
				int debug = -2;
				char * temp = getenv("PWD");
				strcat(temp, dir);
				//printf("DEBUG: %s\n", temp);
				debug = chdir(temp);
				if(debug == 0) setenv("PWD", temp, 1);
				//printf("2 - DEBUG:%d - PWD: %s\n", debug, getenv("PWD"));
			}
			default: break;
		}
	}
	//free(dir);
}



void sysCall(char *name)
{
	char * command = NULL;
	if(name[0] == '.' && name[1] == '/')
	{
		size_t allocSize = sizeof(char) * (strlen(getenv("PWD")) + strlen(name));
		command = (char *)malloc(allocSize);
		strcpy(command, getenv("PWD"));
		name = strTrim(name, 1, 0);
		strcat(command, name);
		printf("DEBUG sysCall: command %s \n",command);
		if(access(command, F_OK) == -1)
		{
			free(command);
			command = NULL;
		}
	}
	else
	{
		char backSlash[] = "/";
		strcat(backSlash, name);
		command = backSlash;
		printf("DEBUG sysCall: command %s \n",command);
		
		//Make copy of PATH string to prevent strtok-related corruption
		size_t allocSize = sizeof(char) * (strlen(getenv("PATH")) + 1);
		char* fullParse = (char *)malloc(allocSize);
		char* parse = strtok(strcpy(fullParse, getenv("PATH")),":");
		char fullCommand[50];
		
		while(parse != NULL)
		{
			printf("DEBUG sysCall: parse %s \n",parse);
			strcpy(fullCommand,parse);
			strcat(fullCommand,command);
			if(access(fullCommand, F_OK) != -1)
			{
				printf("DEBUG sysCall: Found %s\n", name);
				break;
			}
			parse = strtok(NULL,":");
		}
		free(fullParse);
		if(parse != NULL)
			command = fullCommand;
		else
			command = NULL;
	}

	if(argArray != NULL)
		argArray[0] = name;

	if(command == NULL)
	{
		printf("Invalid Command\n");		
	}
	else if(fork() == 0)
	{
		//Child
		IORedir();
		if(argArray == NULL)
		{
			execl (command, name, (char *) NULL); 
		}
		else
		{
			int temp = 0;
			while(argArray[temp] != (char *) NULL)
			{
				printf("DEBUG sysCall: argArray %d %s \n", temp, argArray[temp]);
				temp++;
			}
			execv (command, argArray); 
		}
		exit(0);
	}
	else
	{
		//Parent
		if(waiter)
		{
			wait ((int *) 0);
			printf("DEBUG: Wait complete\n");
		}
		else
		{
			waiter = TRUE;
			printf("DEBUG: Wait aborted\n");
		}
	}	
	//int iii = 0;
	//while(argArray != NULL && (argArray[iii] != (char *) NULL)) //segfaults for some weird reason
	//{
	//	free(argArray[iii]);
	//	argArray[iii] = NULL;
	//	iii++;
	//}
	free(argArray); 
	argArray = NULL;
	free(inRedir);
	inRedir = NULL;
	free(outRedir);
	outRedir = NULL;
	free(errRedir);
	errRedir = NULL;
	return;
}


void argJoin(char *arg)
{
	if(argArray != NULL)
	{
		int size = 0;
		while(argArray[size] != NULL) size++;
		size_t allocSize = sizeof(char*) * (size + 2);
		//printf("DEBUG argJoin allocSize %d \n", allocSize);
		char** buffer;
		buffer = realloc(argArray, allocSize);
		if(buffer != NULL)
		{	
			argArray = buffer;
		}
		else
		{
			printf("REALLOC FAIL\n");
			return;
		}
		argArray[size] = arg;
		argArray[size + 1] = (char *) NULL;

		int temp = 0;
		while(argArray[temp] != (char *) NULL)
		{
			//printf("DEBUG argJoin: Add %d %s \n", temp, argArray[temp]);
			temp++;
		}

		//printf("DEBUG argJoin: Add: NULL \n");
	}
	else
	{
		size_t allocSize = sizeof(char*) * 3;
		argArray = malloc(allocSize);
		argArray[0] = arg;
		argArray[1] = arg;
		argArray[2] = (char *) NULL;
		
		int temp = 0;
		while(argArray[temp] != (char *) NULL)
		{
			//printf("DEBUG argJoin: New %d %s \n", temp, argArray[temp]);
			temp++;
		}

		//printf("DEBUG argJoin: New NULL \n");
	}
}

char* argString()
{
	if(argArray != NULL)
	{
		char *argStr = NULL;
		int iii, jjj, totLen;
		iii = 1; totLen = 0;
		while(argArray[iii] != (char *) NULL)
		{
			totLen += strlen(argArray[iii]);
			//printf("DEBUG argString: totLen %d\n", totLen);
			iii++;
		}
		size_t allocSize = sizeof(char) * totLen;
		argStr = malloc(allocSize);
		jjj = iii - 1;
		while(jjj > 0)
		{
			strcat(argStr, argArray[jjj]);	
			//printf("DEBUG argString: strcat %s\n", argStr);		
			jjj--;
		}
		//printf("DEBUG argString: %s\n", argStr);
		return argStr;
	}
}

void IORedir()
{
	if(inRedir != NULL)
	{
		//printf("DEBUG IORedir in: %s \n", inRedir);
		int temp = open(inRedir, O_RDONLY, S_IREAD | S_IWRITE );
		if (temp == -1)
           	{
           		perror(inRedir);
           		return;
           	}
		close(0);
		dup(temp);
		close(temp);
	}

	if(outRedir != NULL)
	{
		//printf("DEBUG IORedir out: %s \n", outRedir);
		int temp = open(outRedir, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE);
				if (temp == -1)
           	{
           		perror(outRedir);
           		return;
           	}
		close(1);
		dup(temp);
		close(temp);
	}

	if(errRedir != NULL)
	{
		//printf("DEBUG IORedir err: %s \n", errRedir);
		int temp = open(errRedir, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE);
		if (temp == -1)
           	{
           		perror(errRedir);
           		return;
           	}
		close(2);
		dup(temp);
		close(temp);
	}
}

char* aliasChk(char* str, int recur)
{
	recur++;
	if(aliasLock)
	{
		aliasLock = FALSE;
		return str;
	}
	if(recur > 1000)
	{
		printf("ERROR - Recursive alias\n");
		return "";
	}
	struct node* alias = aliasGet(aliasPtr, str);
	if(alias != NULL)
	{
		if(strchr(alias->data, ' ') != NULL)
		{
			free(str);
			str = strdup(alias->data);
			int buf = yy_scan_string(str);
			yyparse();
			yyrestart(stdin);

			mainLoop = TRUE;
			counter++;
			printf("[ %d ]: ", counter);
			free(str);
			str = NULL;
		}
		else
		{
			free(str);
			str = aliasChk(strdup(alias->data), recur);
		}
	}
	return str;
}
