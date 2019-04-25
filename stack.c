
#include "stackHeader.h"
/* stack.c */

/* Declaration of variables for stack*/
int stack_size = 5000;
StackSlot stack_njvm[5000];
int stack_counter = 0;
int frame_counter = 0;
int *pointer = NULL;

/* Declaration of functions for stack*/
int push(ObjRef adresse);
int pushNumber(int number);
int pushComplexObj(int anzahl);
int putFieldInComplexObject(int field);
int getValueOfFieldComplexObject(int field);
StackSlot pop();
int stack_isEmpty();
int stack_isFull();
int addFrame(int stackGrowth);
int releaseFrame();
int pushFrameValue(int pos);
int popStackSaveValueInFrame(int pos);
int getStackCounter();
int getFramCounter();

/*  push() function returns 0, if it pushed the element successful. If the stack
    is already full, it returns 1*/
int push(ObjRef adresse){

    if(stack_isFull()){
        return 1;
    }

    stack_njvm[stack_counter].isObjRef = 1;
    stack_njvm[stack_counter].u.objRef = adresse;

    stack_counter = stack_counter + 1;

    return 0;
}

int pushNumber(int number){

    if(stack_isFull()){
        return 1;
    }

    stack_njvm[stack_counter].isObjRef = 0;
    stack_njvm[stack_counter].u.number = number;

    stack_counter = stack_counter + 1;
   
    return 0;
}

int pushComplexObj(int anzahl){

    ObjRef objRef;

    if(stack_isFull()){
        return 1;
    }

    objRef = malloc(sizeof(unsigned int) +
                  anzahl * sizeof(ObjRef));
    
    if (objRef == NULL) {
        printf("Fatal error with memory reservation!\n");
        exit(99);
    }
    
    stack_njvm[stack_counter].isObjRef = 1;
    //the 2147483648 is in binary 100000000... with 31 zeros! (so basically set MSB)
    objRef-> size = MSB | anzahl;
    stack_njvm[stack_counter].u.objRef = objRef;
    
    /* all value initializeing with null*/
    for(int i = 0; i < (int) GET_SIZE(objRef); i++ ){
        GET_REFS(objRef)[i] = NULL;
    }

    stack_counter = stack_counter + 1;
   
    return 0;
}

/* set the field value in a complex object to the TOS element*/
int putFieldInComplexObject(int field){

    StackSlot element1;
    StackSlot element2;

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

    // checking if everything is alright
    if( element2.u.objRef == NULL ){
        printf("FATAL ERROR: object is null! \n");
  		exit(99);
    }
    if( !(element1.isObjRef && element2.isObjRef) ) {
        printf("FATAL ERROR: stack should have an Objref here not a number! \n");
  		exit(99);
    }
    if(IS_PRIM(element2.u.objRef)){
        printf("FATAL ERROR: stack should have an Complex Objref here not a Primitive Type! \n");
  		exit(99);
    }
    if( (int)GET_SIZE(element2.u.objRef) <= field || field < 0){
        printf("FATAL ERROR: index out of bounds for Complex Objref! \n");
  		exit(99);
    }

    GET_REFS(element2.u.objRef)[field] = element1.u.objRef;

    return 0;
}

/* put field value of the complex object (which is on TOS) on TOS*/
int getValueOfFieldComplexObject(int field){

    StackSlot element1;

    if(stack_isEmpty()){
	    printf("Stack is empty! ERROR! \n");
		exit(99);
    }

    element1 = pop();

    // checking if everything is alright
    if( element1.u.objRef == NULL ){
        printf("FATAL ERROR: object is null! \n");
  		exit(99);
    }
    if( !(element1.isObjRef) ) {
        printf("FATAL ERROR: stack should have an Objref here not a number! \n");
  		exit(99);
    }
    if(IS_PRIM(element1.u.objRef)){
        printf("FATAL ERROR: stack should have an Complex Objref here not a Primitive Type! \n");
  		exit(99);
    }
    if( (int)GET_SIZE(element1.u.objRef) <= field || field < 0){
        printf("FATAL ERROR: index out of bounds for Complex Objref! \n");
  		exit(99);
    }

    if( push( GET_REFS(element1.u.objRef)[field] ) ){
        printf("Stack is full! ERROR! \n");
		exit(99);
    }

    return 0;
}

/*Before using pop(), you need to check, if Stack is empty.*/
StackSlot pop(){

    stack_counter = stack_counter - 1;

    return stack_njvm[stack_counter];
}


/*Returns 1, if the stack is empty, otherwise returns 0*/
int stack_isEmpty(){

    if(stack_counter <= 0){
        return 1;  
    }

    return 0;
}


/*Returns 1, if the stack is full, otherwise returns 0*/
int stack_isFull(){

    if(stack_counter >= stack_size){
        return 1;
    } 

    return 0;

}

/*Returns 1, if stack_counter is pointing beyond the stack.*/
int addFrame(int stackGrowth){

    if(pushNumber(frame_counter)){
        printf("Fatal error: Stack is Full!");
        exit(99);
    }

    frame_counter = stack_counter;
    stack_counter = stack_counter + stackGrowth;

    /* initializing local variables with NULl*/
    for(int i = 0; i < stackGrowth; i++){
        stack_njvm[frame_counter + i].isObjRef = 1;
        stack_njvm[frame_counter + i].u.objRef = NULL;
    }

    if(stack_isFull()){
        return 1;
    }

    return 0;
}

int releaseFrame(){

    StackSlot element;

    stack_counter = frame_counter;

    
    if(stack_isEmpty()){
	    printf("Stack is empty! ERROR! \n");
		exit(99);
    }
        
    element = pop();

    if(element.isObjRef){
        printf("FATAL ERROR: stack should have a number here not an Objref! \n");
  		exit(99);
    }

    frame_counter = element.u.number;

    return 0;
}

int pushFrameValue(int pos){

    /*check if pos is not going beyond the current frame boundaries*/
    if(frame_counter + pos >= stack_counter){
        printf("ERROR! You want to access values beyond the current frame.\n");
        exit(99);
    }

    StackSlot pushValue = stack_njvm[frame_counter + pos];
        
    if(stack_isFull()){
        return 1;
    }

    stack_njvm[stack_counter] = pushValue;

    /*
    if(pushValue.isObjRef){
        stack_njvm[stack_counter].isObjRef = 1;

        Geht das oder brauche ich einen neuen Pointer, um eine neue Adresse zu generieren. fuer???
        stack_njvm[stack_counter].u.objRef = pushValue.u.objRef;     
    }else{
        stack_njvm[stack_counter].isObjRef = 0;
        stack_njvm[stack_counter].u.number = pushValue.u.number;        
    }  
    */

    stack_counter = stack_counter + 1;

    return 0;
}

int popStackSaveValueInFrame(int pos){

    /*check if pos is not going beyond the current frame boundaries*/
    if(frame_counter + pos >= stack_counter){
        printf("ERROR! You want to access values beyond the current frame.\n");
        exit(99);
    }

    StackSlot popValue;

    if(stack_isEmpty()){
	    printf("Stack is empty! ERROR! \n");
		exit(99);
    }
    
    popValue = pop();

    stack_njvm[frame_counter + pos] = popValue;
    
    return 0;
}

int getStackCounter(){
    return stack_counter;
}

int getFramCounter(){
    return frame_counter;
}
