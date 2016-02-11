#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "linklist.h"

#ifndef global_h
#define global_h
void yyerror (char *s);
char* strTrim(char *str, int fr, int bk);
void sysCall(char *name);
void cdCall(int command, char *dir);
void argJoin(char *arg);
//char* argString();
void IORedir();
void setEnv(char* var1, char* var2);
void printEnv(char* name);
void unsetEnv(char *name);
char* aliasChk(char *str, int recur);

typedef enum { FALSE, TRUE } bool;
char **argArray;
char *inRedir;
char *outRedir;
char *errRedir;
bool waiter;
bool aliasLock;
bool mainLoop;
int counter;
int buffer;
extern char** environ;
struct node* aliasPtr;
#endif
