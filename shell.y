%{
#include "global.h"
%}

%union {int num; char *word; char** wordArr;}
%start start
%token execute

%token exit_command
%token cd_command
%token setenv_command
%token getenv_command
%token unsetenv_command
%token alias_command
%token unalias_command

%token ddot
%token <word> text
%type <word> syscall
%type <word> customCall
%type <wordArr> arg
%type <word> IO_Pipe

%%
start:	execute				{counter--; return;}
|	customCall execute		{return;}
|	exit_command execute		{printf("Leaving the shell... \n"); exit(0);}
|	syscall execute			{return;} //syscall
|	start execute			{return;}
|	start customCall execute	{return;}
|	start exit_command execute	{printf("Leaving the shell... \n"); exit(0);}
|	start syscall execute		{return;} //syscall
;

customCall: 			{;}
| cd_command			{cdCall(0, NULL);} //Home '
| cd_command ddot 		{cdCall(1, NULL);} //Previous Directory '
| cd_command ddot text		{cdCall(1, $3); free($3);}  //Previous Directory '
| cd_command text		{cdCall(0, $2); free($2);}  //Root - Current '
//| cd_command "." text		{cdCall(2, $3); free($3);}  //Current '

| setenv_command text text	{setEnv($2, $3);}
| getenv_command		{printEnv(NULL);}
| getenv_command text		{printEnv($2);}
| unsetenv_command text		{unsetEnv($2);}

| alias_command			{aliasPrint(aliasPtr);}
| alias_command text text	{aliasInsert(&aliasPtr, $3, $2);}
| unalias_command text		{aliasDelete(&aliasPtr, $2);}
;

syscall: text			{sysCall($1); free($1);} //syscall
|	text IO_Pipe		{sysCall($1); free($1);}
|	text arg		{sysCall($1); free($1);} //syscall + arguments
|	text arg IO_Pipe	{sysCall($1); free($1);}
|	text '&'		{waiter = FALSE; sysCall($1); free($1);} //syscall
|	text IO_Pipe '&'	{waiter = FALSE; sysCall($1); free($1);}
|	text arg '&'		{waiter = FALSE; sysCall($1); free($1);} //syscall + arguments
|	text arg IO_Pipe '&'	{waiter = FALSE; sysCall($1); free($1);}
;

arg:  text		{argJoin($1); } 
|	text arg	{argJoin($1); }
;

IO_Pipe: '<' text 			{inRedir = $2;}		//in
|	'>' text 			{outRedir = $2;}	//out
|	'<' text '>' text 		{inRedir = $2; outRedir = $4;}	//in out	
|	'>' '&' text			{errRedir = $3;}		//err
|	'<' text '>' '&' text		{inRedir = $2; errRedir = $5;}	//in err
|	'>' text '>' '&' text		{outRedir = $2; errRedir = $5;}	//out err
|	'<' text '>' text '>' '&' text	{inRedir = $2; outRedir = $4;errRedir = $7;}	//in out err
;

%%


