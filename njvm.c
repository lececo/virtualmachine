#include "header.h"

int main(int argc, char *argv[]){

	/*Function Declaration		*/
	int terminal_commands(int anzahl, char *argumente[]);
	int startDebugger(void);
	/*	Variable Declaration	*/
	int kommandozeile_auswertung = 0;
	
	/* Prints all the Terminal commands!

	int i = 0;
	for(; i < argc; i++){
		    
		 printf("argv[%d] = %s !\n", i, argv[i]);	
		 

		}
	*/

	kommandozeile_auswertung = terminal_commands(argc, argv);

	if(kommandozeile_auswertung == 0){
		printf("Ninja Virtual Machine started\n");
		printf("Ninja Virtual Machine stopped\n");		
	}else if (kommandozeile_auswertung == 1){
		printf("Ninja Virtual Machine started\n");
		programmspeicher_execute();
		printf("Ninja Virtual Machine stopped\n");	
	}else if (kommandozeile_auswertung == 2){
		
		startDebugger();

	}else if(kommandozeile_auswertung == 7){
		printf("no code file specified. Please specifiy a file\n");		
	}

	free(programmspeicher);
	
	/*
	free(staticDataVariables);
	*/

	return 0;
}


/*starts debugger*/
int startDebugger(void){
	
	int halt = 0;
	int temp = 0;
	int userInput = 1;
	int breakpoint = -1;
	char word[15];
	IR = 0;
	PC = 0;

	printf("DEBUG: file '%s' loaded (code size = %d, data size = %d) \n", debugProgrammName, programmspeicherSize, staticDataAreaSize);
	printf("Ninja Virtual Machine started\n");
	
	while(!halt){


		IR = programmspeicher[PC];
		
		/*list one instruction*/
		interpreter_lists(IR);

		printf("DEBUG: inspect, list, breakpoint, step, run, quit? \n");
			
		while(userInput){
			
			/*get user input	*/
			fgets(word, sizeof(word), stdin);

			/*evaluate input from user*/
			if(!strncmp("step", word, 1)){
				
				userInput = 0;

			}else if(!strncmp("quit", word, 1)){
				printf("Ninja Virtual Machine stopped\n");
				exit(99);
			}else if(!strncmp("run", word, 1)){
				
				if(breakpoint == -1){
					/*execute the program and quit*/
					debugger_programmspeicher_execute(-1);
					printf("Ninja Virtual Machine stopped\n");
					exit(99);
				}else{
					
					debugger_programmspeicher_execute(breakpoint);
					breakpoint = -1;

					interpreter_lists(IR);
					printf("DEBUG: inspect, list, breakpoint, step, run, quit? \n");
				}

			}else if(!strncmp("list", word, 1)){
				
				/*lists the whole program*/
				int PC_OLD = PC;
				int IR_OLD = IR;

				programmspeicher_auslese();
				PC = PC_OLD;
				IR = IR_OLD;
				printf(" --- end of code --- \n");

				interpreter_lists(IR);
				printf("DEBUG: inspect, list, breakpoint, step, run, quit? \n");

			}else if(!strncmp("inspect", word, 1)){
				
				printf("DEBUG [inspect]: stack, data, object? \n");
				fgets(word, sizeof(word), stdin);

				if(!strncmp("data", word, 1)){
					
					int i = 0;

					for( ; i < staticDataAreaSize; i++){
						// check for null pointer
						if((void *)staticDataVariables[i] != NULL){
							printf("\t (objref) \tdata[00%d]:\t%p \n", i,  (void *)staticDataVariables[i]);
						}
						else{
							printf("\t (objref) \tdata[00%d]:\t(nil) \n", i);
						}
					}
					
					printf("\t\t --- end of data --- \n");

					interpreter_lists(IR);
					printf("DEBUG: inspect, list, breakpoint, step, run, quit? \n");

				}else if(!strncmp("stack", word, 1)){
					
					int stack_counter = getStackCounter();
					int frame_counter = getFramCounter();
					int counter = stack_counter;

					if(frame_counter == stack_counter){
						printf("sp, fp \t---> \t 00%d: \t(xxxx) xxxx \n", counter);
						counter = counter - 1;
					}else{
						printf("sp \t---> \t 00%d: \t(xxxx) xxxx \n", counter);
						counter = counter - 1;
					}
					
					while(counter >= 0){
						
						if(stack_njvm[counter].isObjRef){
							if(counter == frame_counter){
								printf("fp \t---> \t 00%d: \t(objref) %p \n", counter, (void *)stack_njvm[counter].u.objRef);
								counter = counter - 1;
							}
							else{
								printf(" \t \t 00%d: \t(objref) %p \n", counter, (void *)stack_njvm[counter].u.objRef);
								counter = counter - 1;	
							}
						}
						else{
							if(counter == frame_counter){
								printf("fp \t---> \t 00%d: \t(number) %d \n", counter, stack_njvm[counter].u.number);
								counter = counter - 1;
							}
							else{
								printf(" \t \t 00%d: \t(number) %d \n", counter, stack_njvm[counter].u.number);
								counter = counter - 1;	
							}
						}						
					}

					printf("\t\t --- bottom of the stack --- \n");
					
					interpreter_lists(IR);
					printf("DEBUG: inspect, list, breakpoint, step, run, quit? \n");
				}else if(!strncmp("object", word, 1)){

					void *input;

					printf("object reference? \n");

					scanf("%p", &input);

					bip.op1 = input;

					if( IS_PRIM(bip.op1) ){
						printf("<primitive object> \n");
						printf("value = ");
						bigPrint(stdout);
						printf("\n");
					}else{
						printf("<compound object> \n");
						for(int i = 0; i < GET_SIZE( bip.op1 ); i++){
							printf("field[00%d]: \t ", i);
						
							if(GET_REFS( bip.op1 )[i] == NULL){
								printf("(objref) nil \n");
							}else{
								
								bip.op2 = bip.op1;
								bip.op1 = GET_REFS( bip.op1 )[i];

								printf("(objref) %p", (void *)bip.op1);
								printf("\n");

								bip.op1 = bip.op2;
							}
						}
					}

					printf(" \t --- end of object --- \n");

					interpreter_lists(IR);
					printf("DEBUG: inspect, list, breakpoint, step, run, quit? \n");
				}
				else{

					interpreter_lists(IR);
					printf("DEBUG: inspect, list, breakpoint, step, run, quit? \n");
				}
			}else if(!strncmp("breakpoint", word, 1)){
				
				int input;

				/*display user current breakpoint*/
				if(breakpoint == -1){
					printf("DEBUG [breakpoint]: cleared \n");
				}else{
					printf("DEBUG [breakpoint]: set at %d \n", breakpoint);
				}

				/*get user input*/
				printf("DEBUG [breakpoint]: address to set, -1 to clear, <ret> for no change? \n");
				scanf("%d", &input);			
				

				/*display user new breakpoint*/
				if(input == -1){
					breakpoint = -1;
					printf("DEBUG [breakpoint]: now cleared \n");
				}else if(input < programmspeicherSize && input > -1){
					breakpoint = input;
					printf("DEBUG [breakpoint]: now set at %d \n", breakpoint); 
				}else{

				}

				printf("DEBUG: inspect, list, breakpoint, step, run, quit? \n");
			
			}
		}

		PC = PC + 1;
		/*execute one instruction*/
		temp = interpreter_execute(IR);

		userInput = 1;

		if(temp == 1){
			halt = 1;
		}	
	}	

	/*
	DEBUG: inspect, list, breakpoint, step, run, quit?
	*/
	return 0;
} 


int debugger_programmspeicher_execute(int breakpoint){

	int halt = 0;

	while(!halt){

		IR = programmspeicher[PC];

		if(PC == breakpoint){
			return 0;
		}

		PC = PC + 1;

		if(interpreter_execute(IR)){
			halt = 1;
		}
	
	}

	return 0;
}

/*	Reads the whole program memory and lists every instruction in the terminal.
	Returns 1, if there are any unknown he assembler code instructions.
*/
int programmspeicher_auslese(void){
	
	IR = 0;
	PC = 0;

	while(programmspeicherSize > PC){
		
		IR = programmspeicher[PC];

		interpreter_lists(IR);

		PC = PC + 1;

	}
	
	return 0;
}



/*	Executes the whole program memory, executes every instruction.

*/
int programmspeicher_execute(void){

	int halt = 0;
	IR = 0;;
	PC = 0;

	while(!halt){

		IR = programmspeicher[PC];

		PC = PC + 1;

		if(interpreter_execute(IR)){
			halt = 1;
		}

	}

	return 0;

}


/*	Interpreter lists all the instructions for the given program
	Return 2, if it doesn't know the instruction!
*/
int interpreter_lists(int IR){

	int value;

	switch( (OP_CODE(IR)) ){

		case HALT:
			
			printf("%d:\t HALT \n", PC);
		
			return 1;
			
			break;

		case PUSHC:
			
			value = IMMEDIATE(IR);
			value = SIGN_EXTEND(value);

			printf("%d:\t PUSHC %d \n", PC, value);
			
			break;
		
		case ADD:

			printf("%d:\t ADD \n", PC);
		
			break;
		
		case SUB:
		
			printf("%d:\t SUB \n", PC);
			
			break;

		case MUL:

			printf("%d:\t MUL \n", PC);
			
			break;
		
		case DIV:

			printf("%d:\t DIV \n", PC);
			
			break;
		
		case MOD:

			printf("%d:\t MOD \n", PC);
			
			break;
		
		case RDINT:

			printf("%d:\t RDINT \n", PC);

			break;
		
		case WRINT:

			printf("%d:\t WRINT \n", PC);
			
			break;
		
		case RDCHR:

			printf("%d:\t RDCHR \n", PC);
			
			break;
		
		case WRCHR:

			printf("%d:\t WRCHR \n", PC);
			
			break;

		case PUSHG:

			value = IMMEDIATE(IR);

			printf("%d:\t PUSHG %d \n", PC, value);

			break;

		case POPG:

			value = IMMEDIATE(IR);
			
			printf("%d:\t POPG %d \n", PC, value);

			break;
		
		case ASF:

			value = IMMEDIATE(IR);
			
			printf("%d:\t ASF %d \n", PC, value);

			break;
		
		case RSF:

			printf("%d:\t RSF \n", PC);

			break;
		
		case PUSHL:

			value = IMMEDIATE(IR);
			value = SIGN_EXTEND(value);

			printf("%d:\t PUSHL %d \n", PC, value);

			break;

		case POPL:
			
			value = IMMEDIATE(IR);
			value = SIGN_EXTEND(value);

			printf("%d:\t POPL %d \n", PC, value);

			break;

		case EQ:

			printf("%d:\t EQ \n", PC);

			break;

		case NE:

			printf("%d:\t NE \n", PC);

			break;

		case LT:

			printf("%d:\t LT \n", PC);

			break;

		case LE:

			printf("%d:\t LE \n", PC);

			break;

		case GT:

			printf("%d:\t GT \n", PC);

			break;

		case GE:

			printf("%d:\t GE \n", PC);

			break;

		case JMP:
			
			value = IMMEDIATE(IR);
			
			printf("%d:\t JMP %d \n", PC, value);

			break;

		case BRF:

			value = IMMEDIATE(IR);

			printf("%d:\t BRF %d \n", PC, value);

			break;

		case BRT:

			value = IMMEDIATE(IR);

			printf("%d:\t BRT %d \n", PC, value);

			break;

		case CALL:

			value = IMMEDIATE(IR);

			printf("%d:\t CALL %d \n", PC, value);

			break;

		case RET:

			printf("%d:\t RET \n", PC);

			break;

		case DROP:

			value = IMMEDIATE(IR);

			printf("%d:\t DROP %d \n", PC, value);

			break;

		case PUSHR:

			printf("%d:\t PUSHR \n", PC);

			break;

		case POPR:

			printf("%d:\t POPR \n", PC);

			break;

		case DUP:

			printf("%d:\t DUP \n", PC);

			break;

		case NEW:

			value = IMMEDIATE(IR);

			printf("%d:\t NEW %d \n", PC, value);

			break;

		case GETF:
		
			value = IMMEDIATE(IR);

			printf("%d:\t GETF %d \n", PC, value);

			break;

		case PUTF:
			
			value = IMMEDIATE(IR);

			printf("%d:\t PUTF %d \n", PC, value);

			break;

		case NEWA:
			
			printf("%d:\t NEWA \n", PC);

			break;

		case GETFA:
			
			printf("%d:\t GETFA \n", PC);

			break;

		case PUTFA:
			
			printf("%d:\t PUTFA \n", PC);

			break;

		case GETSZ:
			
			printf("%d:\t GETSZ \n", PC);

			break;

		case PUSHN:
			
			printf("%d:\t PUSHN \n", PC);

			break;

		case REFEQ:
			
			printf("%d:\t REFEQ \n", PC);

			break;

		case REFNE:
			
			printf("%d:\t REFNE \n", PC);

			break;

		default:
			printf("ERROR, wrong instruction! Check your assembler code again!");
			exit(99);
	}

	return 0;

}

/*	Interpreter executes all the instructions for the given program.
*/
int interpreter_execute(int IR){

	int value = 0;
	int counterVar = 0;
	char zeichen = 0;
	StackSlot element1;
	StackSlot element2;
	ObjRef objRef = NULL;	

	switch( (OP_CODE(IR)) ){

		case HALT:
			
			return 1;
			
			break;

		case PUSHC:
			
			value = IMMEDIATE(IR);
			value = SIGN_EXTEND(value);
			
			bigFromInt(value);
			
			if(push(bip.res)){
				printf("Stack is full! ERROR! \n");
				exit(99);
			}

			break;

		case ADD:

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				element1 = pop();
				
				if(!element1.isObjRef){
    			    printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  					exit(99);
  				}

				if(stack_isEmpty()){
					printf("Stack is empty! ERROR! \n");
					exit(99);
				}else{

					element2 = pop();
					
					if(!element2.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  					}

					bip.op1 = element1.u.objRef;
					bip.op2 = element2.u.objRef;

					bigAdd();

					push(bip.res);
				}

			}			
		
			break;
		
		case SUB:

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				element1 = pop();
				
				if(!element1.isObjRef){
    				printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  					exit(99);
  				}

				if(stack_isEmpty()){
					printf("Stack is empty! ERROR! \n");
					exit(99);
				}else{
 
					element2 = pop();

					if(!element2.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  					}

					bip.op1 = element2.u.objRef;
					bip.op2 = element1.u.objRef;

					bigSub();

					push(bip.res);
				}

			}
			
			break;

		case MUL:

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				element1 = pop();

				if(!element1.isObjRef){
    				printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  					exit(99);
  				}

				if(stack_isEmpty()){
					printf("Stack is empty! ERROR! \n");
					exit(99);
				}else{

					element2 = pop();

					if(!element2.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  					}

					bip.op1 = element2.u.objRef;
					bip.op2 = element1.u.objRef;

					bigMul();

					push(bip.res);
				}

			}

			break;
		
		case DIV:

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				element1 = pop();

				if(!element1.isObjRef){
    			   	printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  					exit(99);
  				}

				if(stack_isEmpty()){
					printf("Stack is empty! ERROR! \n");
					exit(99);
				}else{

					element2 = pop();

					if(!element2.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  					}
			
					/* check for division by zero*/
					bip.op1 = element1.u.objRef;
					value = bigToInt();

					if(value == 0){
						printf("Division with zero is not possible! \n");
						exit(99);
					}

					bip.op1 = element2.u.objRef;
					bip.op2 = element1.u.objRef;

					bigDiv();

					push(bip.res);
				}

			}
			
			break;
		
		case MOD:
			
			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				element1 = pop();

				if(!element1.isObjRef){
    			   	printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  					exit(99);
  				}

				if(stack_isEmpty()){
					printf("Stack is empty! ERROR! \n");
					exit(99);
				}else{

					element2 = pop();

					if(!element2.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  					}

					/* check for division by zero*/
					bip.op1 = element1.u.objRef;
					value = bigToInt();

					if( value == 0){
						printf("Division with zero is not possible! \n");
						exit(99);
					}

					bip.op1 = element2.u.objRef;
					bip.op2 = element1.u.objRef;

					bigDiv();

					push(bip.rem);
				}

			}
			
			break;
		
		case RDINT:

			bigRead(stdin);

			if(push(bip.res)){
				printf("Stack is full! ERROR! \n");
				exit(99);
			}

			break;
		
		case WRINT:

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				element1 = pop();

				if(element1.isObjRef){
					bip.op1 = element1.u.objRef;
					bigPrint(stdout);
				}else{
					printf("%d", element1.u.number);
				}
			}
			
			break;
		
		case RDCHR:

			scanf("%c", &zeichen);

			value = IMMEDIATE(zeichen);

			bigFromInt(value);

			if(push(bip.res)){
				printf("Stack is full! ERROR! \n");
				exit(99);
			}
			
			break;
		
		case WRCHR:

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{
				element1 = pop();

				// do we need to check? Should we allow to print frame_counter or jump adresses?
				if(element1.isObjRef){
					bip.op1 = element1.u.objRef;
					value = bigToInt();
				}else{
					value = element1.u.number;
				}

				printf("%c", value);
			}

			break;
		
		case PUSHG:

			value = IMMEDIATE(IR);
			
			if(value >= staticDataAreaSize){
					printf("FATAL error: You are exceeding the limits of the global Data Area.");
					exit(99);
			}

			if(push(staticDataVariables[value])){
				printf("Stack is full! ERROR! \n");
				exit(99);
			}
 
			break;

		case POPG:

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				element1 = pop();
				value = IMMEDIATE(IR);
				
				if(value >= staticDataAreaSize){
					printf("FATAL error: You are exceeding the limits of the global Data Area.");
					exit(99);
				}
				
				staticDataVariables[value] = element1.u.objRef;
			}
			
			break;
		
		case ASF:

			value = IMMEDIATE(IR);

			if(addFrame(value)){
				printf("Your are exceeding the limits of the stack. Stack is full! \n");
				exit(99);
			}

			break;
		
		case RSF:
			
			releaseFrame();

			break;
		
		case PUSHL:
			value = IMMEDIATE(IR);
			value = SIGN_EXTEND(value);
			
			pushFrameValue(value);

			break;

		case POPL:
			/*pop value from stack and save it in frame*/
			value = IMMEDIATE(IR);
			value = SIGN_EXTEND(value);

			popStackSaveValueInFrame(value);

			break;

		case EQ:

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				element1 = pop();

				if(!element1.isObjRef){
    			   	printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  					exit(99);
  				}

				if(stack_isEmpty()){
					printf("Stack is empty! ERROR! \n");
					exit(99);
				}else{

					element2 = pop();
					
					if(!element2.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  					}

					bip.op1 = element1.u.objRef;
					bip.op2 = element2.u.objRef;

					if( ! bigCmp() ){	
						bigFromInt(1);				
						if(push( bip.res )){
							printf("Stack is full! ERROR! \n");
							exit(99);
						}
					}else{
						bigFromInt(0);
						if( push( bip.res )){
							printf("Stack is full! ERROR! \n");
							exit(99);
						}
					}
				}
			}

			break;

		case NE:

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				element1 = pop();

				if(!element1.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  				}

				if(stack_isEmpty()){
					printf("Stack is empty! ERROR! \n");
					exit(99);
				}else{

					element2 = pop();

					if(!element2.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  					}

					bip.op1 = element1.u.objRef;
					bip.op2 = element2.u.objRef;

					if( ! bigCmp() ){	
						bigFromInt(0);				
						if(push( bip.res )){
							printf("Stack is full! ERROR! \n");
							exit(99);
						}
					}else{
						bigFromInt(1);
						if( push( bip.res )){
							printf("Stack is full! ERROR! \n");
							exit(99);
						}
					}
				}
			}

			break;

		case LT:

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				element1 = pop();

				if(!element1.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  				}
				
				if(stack_isEmpty()){
					printf("Stack is empty! ERROR! \n");
					exit(99);
				}else{

					element2 = pop();

					if(!element2.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  					}

					bip.op1 = element1.u.objRef;
					bip.op2 = element2.u.objRef;
					
					if( bigCmp() >= 1){
						bigFromInt(1);
						if(push( bip.res )){
							printf("Stack is full! ERROR! \n");
							exit(99);
						}
					}else{
						bigFromInt(0);
						if(push( bip.res )){
							printf("Stack is full! ERROR! \n");
							exit(99);
						}
					}
				}
			}

			break;

		case LE:

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				element1 = pop();

				if(!element1.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  				}

				if(stack_isEmpty()){
					printf("Stack is empty! ERROR! \n");
					exit(99);
				}else{

					element2 = pop();

					if(!element2.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  					}

					bip.op1 = element1.u.objRef;
					bip.op2 = element2.u.objRef;

					if( bigCmp() >= 0){
						bigFromInt(1);
						if(push( bip.res )){
							printf("Stack is full! ERROR! \n");
							exit(99);
						}
					}else{
						bigFromInt(0);
						if(push( bip.res )){
							printf("Stack is full! ERROR! \n");
							exit(99);
						}
					}
				}
			}

			break;

		case GT:

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				element1 = pop();

				if(!element1.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  				}

				if(stack_isEmpty()){
					printf("Stack is empty! ERROR! \n");
					exit(99);
				}else{

					element2 = pop();

					if(!element2.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  					}

					bip.op1 = element2.u.objRef;
					bip.op2 = element1.u.objRef;

					if( bigCmp() >= 1){
						bigFromInt(1);
						if(push( bip.res )){
							printf("Stack is full! ERROR! \n");
							exit(99);
						}
					}else{
						bigFromInt(0);
						if(push( bip.res )){
							printf("Stack is full! ERROR! \n");
							exit(99);
						}
					}
				}
			}

			break;

		case GE:

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				element1 = pop();

				if(!element1.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  				}

				if(stack_isEmpty()){
					printf("Stack is empty! ERROR! \n");
					exit(99);
				}else{

					element2 = pop();

					if(!element2.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  					}

					bip.op1 = element2.u.objRef;
					bip.op2 = element1.u.objRef;

					if( bigCmp() >= 0){
						bigFromInt(1);
						if(push( bip.res )){
							printf("Stack is full! ERROR! \n");
							exit(99);
						}
					}else{
						bigFromInt(0);
						if(push( bip.res )){
							printf("Stack is full! ERROR! \n");
							exit(99);
						}
					}
				}
			}

			break;

		case JMP:
			/*jump to the immediate value*/
			value = IMMEDIATE(IR);

			PC = value;

			break;

		case BRF:
			/*jump if top element on stack is false*/
			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				element1 = pop();
				
				if(!element1.isObjRef){
    			   	printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  					exit(99);
  				}

				bip.op1 = element1.u.objRef;

				if(bigToInt() == 0){
					value = IMMEDIATE(IR);
					PC = value;			
				}
			}

			break;

		case BRT:
			/*jump if top element on stack is true*/
			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				element1 = pop();

				if(!element1.isObjRef){
    			   	printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  					exit(99);
  				}

				bip.op1 = element1.u.objRef;

				if(bigToInt() == 1){
					value = IMMEDIATE(IR);
					PC = value;			
				}
			}
		
			break;


		case CALL:

			/*push old programm counter adress on stack*/
			if(pushNumber(PC)){
				printf("Stack is full! ERROR! \n");
				exit(99);
			}

			value = IMMEDIATE(IR);
			PC = value;

			break;

		case RET:

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{

				/*get the adress, that was pushed in CALL back*/
				element1 = pop();
				
				if(element1.isObjRef){
    			   	printf("FATAL ERROR: stack should have a number not an objref! \n");
  					exit(99);
  				}
				
				PC = element1.u.number;
			}

			break;

		case DROP:

			/*delete n values from the stack*/

			value = IMMEDIATE(IR);

			for( ; counterVar < value; counterVar++){
				if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
				}else{
					pop();
				}
			}

			counterVar = 0;

			break;

		case PUSHR:

			/*push valueRegister value on stack*/

			if(push(valueRegister)){
				printf("Stack is full! ERROR! \n");
				exit(99);
			}

			break;

		case POPR:

			/*pop from stack and save value in valueRegister*/

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
				}else{
					element1 = pop();

					if(!element1.isObjRef){
    			   		printf("FATAL ERROR: stack should have a ObjRef here not a number! \n");
  						exit(99);
  					}

					valueRegister = element1.u.objRef;
				}

			break;

		case DUP:

			/*duplicate the top element on stack*/

			if(stack_isEmpty()){
				printf("Stack is empty! ERROR! \n");
				exit(99);
			}else{
					element1 = pop();
					
					if(element1.isObjRef){
						push(element1.u.objRef);
						if(push(element1.u.objRef)){
							printf("Stack is full! ERROR! \n");
							exit(99);
						}
					}else{
						pushNumber(element1.u.number);
						if(pushNumber(element1.u.number)){
							printf("Stack is full! ERROR! \n");
							exit(99);
						}
					}
			}

			break;

		case NEW:

			value = IMMEDIATE(IR);

			if(pushComplexObj(value)){
				printf("Stack is full! ERROR! \n");
				exit(99);
			}	

			break;

		case GETF:
			
			value = IMMEDIATE(IR);

			getValueOfFieldComplexObject(value);

			break;

		case PUTF:
			
			value  = IMMEDIATE(IR);
			
			if(putFieldInComplexObject(value)){
				printf("Fatal Error: Prior object in stack was not a complex object!\n");
				exit(99);
			}

			break;

		case NEWA:
			    
   			if(stack_isEmpty()){
	    		printf("Stack is empty! ERROR! \n");
				exit(99);
   			}

		    StackSlot element1;

		    element1 = pop();

   			if( !(element1.isObjRef) ) {
        		printf("FATAL ERROR: stack should have an Objref here not a number! \n");
  				exit(99);
    		}

		    bip.op1 = element1.u.objRef;
			value = bigToInt();
			
			if(pushComplexObj(value)){
				printf("Stack is full! ERROR! \n");
				exit(99);
			}

			break;

		case GETFA:
			
			/* exactly the same as GETF, but index value is on TOS not an immediate value*/

			if(stack_isEmpty()){
	    		printf("Stack is empty! ERROR! \n");
				exit(99);
   			}

		    element1 = pop();

   			if( !(element1.isObjRef) ) {
        		printf("FATAL ERROR: stack should have an Objref here not a number! \n");
  				exit(99);
    		}

		    bip.op1 = element1.u.objRef;
			value = bigToInt();

			getValueOfFieldComplexObject(value);

			break;

		case PUTFA:
			
			/* we pop value and index from the stack, just to push value again on the stack and use 
			   putFieldInComplexObject(indexValue)*/

			if(stack_isEmpty()){
	    		printf("Stack is empty! ERROR! \n");
				exit(99);
   			}

		    element1 = pop();

			if(stack_isEmpty()){
	    		printf("Stack is empty! ERROR! \n");
				exit(99);
			}

		    element2 = pop();

   			if( !(element1.isObjRef && element2.isObjRef) ) {
        		printf("FATAL ERROR: stack should have an Objref here not a number! \n");
  				exit(99);
    		}

		    bip.op1 = element2.u.objRef;
			value = bigToInt();

			push(element1.u.objRef);

			putFieldInComplexObject(value);

			break;

		case GETSZ:
			
   			if(stack_isEmpty()){
	    		printf("Stack is empty! ERROR! \n");
				exit(99);
    		}

		    element1 = pop();

			if( !(element1.isObjRef) ) {
        		printf("FATAL ERROR: stack should have an Objref here not a number! \n");
  				exit(99);
    		}

			if( IS_PRIM(element1.u.objRef) ){
				value = -1;
				bigFromInt(value);
				push(bip.res);
			}else{
				bigFromInt(GET_SIZE(element1.u.objRef));
				push(bip.res);
			}

			break;

		case PUSHN:
			
			/* pushing the null pointer on to the stack*/
			objRef = NULL;

			if( push(objRef) ){
				printf("Stack is full! ERROR! \n");
				exit(99);
			}
			
			break;

		case REFEQ:
			
			if(stack_isEmpty()){
	    		printf("Stack is empty! ERROR! \n");
				exit(99);
    		}

		    element1 = pop();

			if(stack_isEmpty()){
	    		printf("Stack is empty! ERROR! \n");
				exit(99);
    		}

		    element2 = pop();
			
			if( !(element1.isObjRef && element2.isObjRef) ) {
        		printf("FATAL ERROR: stack should have an Objref here not a number! \n");
  				exit(99);
    		}

			if(element1.u.objRef == element2.u.objRef){
				bigFromInt(1);
				if( push( bip.res )){
					printf("Stack is full! ERROR! \n");
					exit(99);
				}
			}else{
				bigFromInt(0);
				if( push( bip.res )){
					printf("Stack is full! ERROR! \n");
					exit(99);
				}
			}

			break;

		case REFNE:
			
			if(stack_isEmpty()){
	    		printf("Stack is empty! ERROR! \n");
				exit(99);
    		}

		    element1 = pop();

			if(stack_isEmpty()){
	    		printf("Stack is empty! ERROR! \n");
				exit(99);
    		}

		    element2 = pop();
			
			if( !(element1.isObjRef && element2.isObjRef) ) {
        		printf("FATAL ERROR: stack should have an Objref here not a number! \n");
  				exit(99);
    		}

			if(element1.u.objRef != element2.u.objRef){
				bigFromInt(1);
				if( push( bip.res )){
					printf("Stack is full! ERROR! \n");
					exit(99);
				}
			}else{
				bigFromInt(0);
				if( push( bip.res )){
					printf("Stack is full! ERROR! \n");
					exit(99);
				}
			}

			break;

		/*	Should never get here, because interpreter_lists catches all wrong instructions!	*/
		default:
			printf("ERROR, wrong instruction! Check your assembler code again! \n");
			exit(99);
	}

	return 0;

}


/*terminal_commands function checks for additional arguments.
  If there are arguments to start programs, they will be executed and there will be a return value of 2.
  If there are no arguments, the function will return 3.
 */
int terminal_commands(int anzahl, char *argumente[]){

	int i = 1;
	int returnWert = 0;

	for(; i < anzahl; i++){
		
		if (!strcmp(argumente[i], "--version")){
			printf("Ninja Virtual Machine version 3 (compiled Mai 15 2018, 17:35:57) \n");
			exit(99);
		}
		else if(!strcmp(argumente[i], "--help")){
			printf("usage: ./njvm [option] [option] ...\n");
			printf("	--version show version and exit\n");
			printf("	--help show this help and exit\n");
			printf("	--debug start debugger\n");

			exit(99);
		}
		else if(!strcmp(argumente[i], "--debug")){

			/*start debugger*/
			if(returnWert == 0){
				returnWert = 6;
			}

			returnWert = returnWert + 1;

		}
		else{
			
			FILE * fp;

			fp = fopen(argumente[i], "r");

			checkAndLoadFile(fp);

			debugProgrammName = argumente[i];

			/*check if debugger was started first*/
			if(returnWert == 7){
				returnWert = 1;
			}
			
			returnWert = returnWert + 1;
		}
	}

	return returnWert;
}


void checkAndLoadFile(FILE *fp){

	unsigned int arrayTest[4];

	if(fp == NULL){
				
		printf("ERROR, there is an error with opening the file. \n");

		exit(99);
	}			
			
	if(fread(&arrayTest[0], sizeof(unsigned int), 1, fp) != 1){
			
		printf("ERROR, there is an error with reading the file. \n");
				
		exit(99);
	}
			
	/*Check the format identifier*/
	if(arrayTest[0] != 0x46424A4E){
				
		printf("ERROR, there is a wrong format with the file. \n");

		exit(99);
	}

	if(fread(&arrayTest[0], sizeof(unsigned int), 1, fp) != 1){
			
		printf("ERROR, there is an error with reading the file. \n");
				
		exit(99);
	}

	/*Check the version*/
	if(arrayTest[0] != VERSION){
			
		printf("ERROR, the file has the wrong version. \n");

		exit(99);
	}

	/*Read the number of instructions	*/
	if(fread(&arrayTest[0], sizeof(unsigned int), 1, fp) != 1){
			
		printf("ERROR, there is an error with reading the file. \n");
				
		exit(99);
	}

	/*Allocate memory for the instructions in programmspeicher*/
	programmspeicherSize = arrayTest[0];
	programmspeicher = malloc(programmspeicherSize * sizeof(unsigned int));

	if(programmspeicher == NULL){
		printf("Speicherzugriff Reservierung fehlgeschlagen! \n");
		exit(99);
	}

	/*Read the number of static data variables	*/
	if(fread(&arrayTest[0], sizeof(unsigned int), 1, fp) != 1){
				
		printf("ERROR, there is an error with reading the file. \n");
				
		exit(99);
	}

	staticDataAreaSize = arrayTest[0];

	/*Allocate memory for the instructions in staticDataVariables*/
	staticDataVariables = malloc(staticDataAreaSize * sizeof(ObjRef));
	
	if(staticDataVariables == NULL){
		printf("Speicherzugriff Reservierung fehlgeschlagen! \n");
		exit(99);
	}

	/* initializing the global variables with NULL*/
	for(int i = 0; i < staticDataAreaSize; i++){
		staticDataVariables[i] = NULL;
	}

	/*Read the instructions into programmspeicher */
	if(fread(programmspeicher, sizeof(unsigned int), programmspeicherSize, fp) != programmspeicherSize){
				
		printf("ERROR, there is an error with reading the file. \n");
				
		exit(99);
	}

	if(fclose(fp) != 0){
		
		printf("ERROR, there is an error with closing the file. \n");
				
		exit(99);
	}
}

/*
 * This routine is called in case a fatal error has occurred.
 * It should print the error message and terminate the program.
 */
void fatalError(char *msg) {
  printf("Fatal error: %s\n", msg);
  exit(1);
}

/*
 * This function is called whenever a new primitive object with
 * a certain amount of internal memory is needed. It should return
 * an object reference to a regular object, which contains a freely
 * usable memory area of at least the requested size (measured in
 * bytes). The memory area need not be initialized in any way.
 *
 * Note that this function may move all objects in memory at will
 * (due to, e.g., garbage collection), as long as the pointers in
 * the global "bip" structure point to the correct objects when
 * the function returns.
 */
ObjRef newPrimObject(int dataSize) {
  ObjRef objRef;

  objRef = malloc(sizeof(unsigned int) +
                  dataSize * sizeof(unsigned char));
  if (objRef == NULL) {
    fatalError("newPrimObject() got no memory");
  }
  objRef->size = dataSize;
  return objRef;
}
