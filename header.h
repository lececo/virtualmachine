#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "support.h"
#include "bigint.h"

#define HALT 0
#define PUSHC 1
#define ADD 2 
#define SUB 3
#define MUL 4
#define DIV 5
#define MOD 6
#define RDINT 7
#define WRINT 8
#define RDCHR 9
#define WRCHR 10
#define PUSHG 11
#define POPG 12
#define ASF 13
#define RSF 14
#define PUSHL 15
#define POPL 16
#define EQ 17
#define NE 18
#define LT 19
#define LE 20
#define GT 21
#define GE 22
#define JMP 23
#define BRF 24
#define BRT 25
#define CALL 26
#define RET 27
#define DROP 28
#define PUSHR 29
#define POPR 30
#define DUP 31
#define NEW 32
#define GETF 33
#define PUTF 34
#define NEWA 35
#define GETFA 36
#define PUTFA 37
#define GETSZ 38
#define PUSHN 39
#define REFEQ 40
#define REFNE 41

/*Makro declaration*/
#define IMMEDIATE(x) ((x) & 0x00FFFFFF)
#define SIGN_EXTEND(x) ((x) & 0x00800000 ? (x) | 0xFF000000 : (x))
#define OP_CODE(x) ((x >> 24))
#define MSB (1 << (8 * sizeof ( unsigned int) - 1))
#define IS_PRIM(objRef) ((( objRef )->size & MSB) == 0)
#define GET_SIZE(objRef) ((objRef)->size & ~MSB)
#define GET_REFS(objRef) ((ObjRef *)(objRef)->data)

/* include stack after makro declaration*/
#include "stack.c"

/*Defining current version*/
#define VERSION 8

/*Global Stack, global Programmcounter and global staticDataArea Declaration	*/
extern StackSlot stack_njvm[];
int PC = 0;
unsigned int *programmspeicher = NULL;   
ObjRef *staticDataVariables = NULL;

/* value register*/
ObjRef valueRegister = NULL;

/*Debugger information variables*/
char *debugProgrammName;
int programmspeicherSize = 0;
int staticDataAreaSize = 0;
unsigned int IR = 0; 

/* Typ Deklarationen 
typedef int integerVar;
typedef char charVar;*/

/*Function Declaration		*/
int interpreter_lists(int IR);
int interpreter_execute(int IR);
int programmspeicher_auslese(void);
int programmspeicher_execute(void);
void checkAndLoadFile(FILE *fp);
int debugger_programmspeicher_execute(int breakpoint);
int handleStackReturn(int counter);

/*Stack Functions           */
extern int putFieldInComplexObject(int field);
extern int getValueOfFieldComplexObject(int value);
extern int pushNumber(int number);
extern int pushComplexObj(int anzahl);
extern int push(ObjRef adresse);
extern StackSlot pop(void);
extern int stack_isEmpty(void);
extern int addFrame(int stackGrowth);
extern int releaseFrame(void);
extern int pushFrameValue(int position);
extern int popStackSaveValueInFrame(int position); 