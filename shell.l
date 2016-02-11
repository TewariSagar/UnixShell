%{
#include "y.tab.h"
#include "global.h"
%}

%%
"cd"					{return cd_command;}
"bye"					{return exit_command;}
"setenv"				{return setenv_command;}
"printenv"				{return getenv_command;}
"unsetenv"				{return unsetenv_command;}
"alias"					{return alias_command;}
"unalias" 				{aliasLock = TRUE; return unalias_command;}
".."					{return ddot;}
\"[ \.\-/0-9a-zA-z<>|&\\]*\"		{yylval.word = strTrim(yytext, 1, 1); return text;}
\$\{[\.\-/0-9a-zA-z]*\}			{yylval.word = strdup(getenv(strTrim(yytext, 2, 1))); return text;}
[\.\-/0-9a-zA-z]*			{yylval.word = aliasChk(strdup(yytext), 0); if(yylval.word != NULL) return text;}
[<>|\&]					{return yytext[0];}
[ \t]					{;}
"\n"					{return execute;}
<<eof>>					{mainLoop = FALSE; return execute;}
.					{yyerror("unexpected character");}
%%
int yywrap(void) {return 1;}
