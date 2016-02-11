void sysCall(char *name)
{
	char * command = NULL;
	if(name[0] == '.' && name[1] == '/')
	{
		size_t allocSize = sizeof(char) * (strlen(getenv("PWD")) + strlen(name));
		char * command = (char *)malloc(allocSize);
		strcpy(command, getenv("PWD");
		name = strTrim(name, 1, 0);
		strcat(command, name);
	}
	else
	{
		char backSlash[] = "/";
		strcat(backSlash, name);
		command = backSlash;
		//printf("DEBUG sysCall: command %s \n",command);
		
		//Make copy of PATH string to prevent strtok-related corruption
		size_t allocSize = sizeof(char) * (strlen(getenv("PATH")) + 1);
		char * parse = (char *)malloc(allocSize);
	
		parse = strtok(strcpy(parse, getenv("PATH")),":");
		char fullCommand[50];
		
		while(parse != NULL)
		{
			//printf("DEBUG sysCall: parse %s \n",parse);
			strcpy(fullCommand,parse);
			strcat(fullCommand,command);
			if(access(fullCommand, F_OK) != -1)
			{
				//printf("DEBUG sysCall: Found %s\n", name);
				break;
			}
			parse = strtok(NULL,":");
		}
		//free(command);
		command = fullCommand;
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
				//printf("DEBUG sysCall: argArray %d %s \n", temp, argArray[temp]);
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
			//printf("DEBUG: Wait complete\n");
		}
		else
		{
			waiter = TRUE;
			//printf("DEBUG: Wait aborted\n");
		}

		//int iii = 0;
		//while(argArray != NULL && (argArray[iii] != (char *) NULL))
		//{
		//	free(argArray[iii]);
		//	argArray[iii] = NULL;
		//	iii++;
		//}

	}	
	free(argArray); 
	argArray = NULL;
	free(inRedir);
	inRedir = NULL;
	free(outRedir);
	outRedir = NULL;
	free(errRedir);
	errRedir = NULL;
	//free(parse); //Throws a munmap_chunk() error for some reason
	return;
}
