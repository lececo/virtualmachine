/*TypeDef Deklarationen*/
typedef struct {
	int isObjRef; 		   /* slot used for object reference ? */
	union {
	    ObjRef objRef;     /* used if isObjRef =TRUE */
	    int number;        /* used if isObjRef =FALSE */
	 } u;
} StackSlot;
