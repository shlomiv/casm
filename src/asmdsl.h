/*
 * asdDsl.h
 *
 *  Created on: Mar 8, 2012
 *      Author: shlomi
 */

#ifndef ASMDSL_H_
#define ASMDSL_H_

#include "commons.h"
#include "parser.h"
#include "DoubleLinkedList.h"
#include "lists.h"
#include "gendsl.h"

#define DO_(c) \
		int count=c;		 \
		Operand a[c] ;		    \
		int pi=0,i;			\
		Operand val;			\
		val.kind = none; \
		for (i=0;i<count;i++) { a[i].kind=none;}

#define FIN_OR(code) \
		for (i=0;val.kind==none&&i<count;i++) {val=a[i];} \
		if (val.kind != none) { \
			code;			\
		};

#define REMOVE_WARNS pi=val.get.constant;pi++;pi=a[0].get.constant; /* a silly line just to remove annoying "unused" warnings*/

#define OR2(A,B)   \
    /*OR2*/ 		    \
    DO_(2)		        \
    l = NULL;			\
	A;B;				\
	l = NULL;

#define OR3(A,B,C) \
    /*OR3*/ 		    \
    DO_(3)		        \
    l = NULL;			\
	A;B;C;				\
	l = NULL;

#define OR4(A,B,C,D)   \
    /*OR3*/ 		    \
    DO_(4)		        \
    l = NULL;			\
	A;B;C;D;			\
	l = NULL;

#define OR5(A,B,C,D,E)   \
    /*OR3*/ 		    \
    DO_(5)		        \
    l = NULL;			\
	A;B;C;D;E;			\
	l = NULL;

#define VAL(i) theVals[i-1]

#define DIRECT \
	if (l==NULL) /*DIRECT*/ { \
		l = strip(getAllAlphas(strip(p," "), a[pi].get.direct), " "); \
		l = oneOf(l,",\n"); \
		if (l!=NULL && l!=strip(p," ")) { \
				a[pi].kind = direct; \
				val = a[pi]; \
				p=l;\
		}\
	}\
	pi++;

#define LABEL1 \
	if (l==NULL) /*LABEL1*/ { \
		l = charIs(getAllAlphasDigits(charIs(charIs(getAllAlphas(strip(p," "), a[pi].get.oneIndex.label), '[') ,'%'), a[pi].get.oneIndex.index), ']'); \
		l = oneOf(l,",\n"); \
		if (l!=NULL && l!=strip(p," ")) { \
				a[pi].kind = label_with_index; \
				val = a[pi]; \
				p=l;\
		}\
	}\
	pi++;

#define LABEL2 \
	if (l==NULL) /*LABEL2*/ { \
		char r; \
		l = charIs(getAllAlphasDigits(charIs(strip(p, " "), '['), a[pi].get.twoIndice.index), ']'); \
		l = getAllAlphasDigits(l, a[pi].get.twoIndice.label); \
		l = charIs(getDigit(charIs(charIs(l, '['), 'r'), &r), ']'); \
		l = oneOf(l,",\n"); \
		if (l!=NULL && l!=strip(p," ")) { \
				a[pi].kind = label_with_two_indices; \
				a[pi].get.twoIndice.reg = r - '0'; \
				val = a[pi]; \
				p=l;\
		}\
	}\
	pi++;

#define REG  \
	if (l==NULL) /*REG*/ { \
		char* n; \
		l = charIs(strip(p," "), 'r'); \
			if (l!=NULL) { \
				if ((n=oneOf(strip(isDigit(l), " "),",\n"))) { \
					a[pi].kind = reg; \
					a[pi].get.reg = 0; \
					a[pi].get.reg = (char)*(l)-'0'; \
					val = a[pi]; \
					p=n;\
				} \
				else l = NULL; \
		}\
	}\
	pi++;


#define CONST  \
	if (l==NULL) /*CONST*/ { \
		l = charIs(strip(p," "),'#');\
		if (l!=NULL) { \
			l = oneOf(getInteger(strip(l," "),&a[pi].get.constant, err), ",\n"); \
			if (l!=NULL) { \
				a[pi].kind = constant; \
				val = a[pi];\
				p=l; \
			} \
		}\
	}\
    pi++;

#define TWO(A,B,code) { \
	Operand theVals[2],val; /*TWO*/	\
	Operand a[2];\
	theVals[0].kind=none;theVals[1].kind=none;\
	int pi=0;\
	char* l=NULL;	\
	if (p==NULL) return NULL; \
	{A;theVals[0]=val;} \
	l=NULL;				\
	if (theVals[0].kind==none) { \
		*err = FIRST_ARG_IS_INVALID;\
		return NULL;\
	} \
	{B;theVals[1]=val;}	\
	if (theVals[1].kind==none) { \
		*err = SECOND_ARG_IS_INVALID;\
		return NULL;\
	} \
	code;		\
	REMOVE_WARNS		\
	return p; \
}

#define ONE(A, code) { \
		Operand theVals[1],val; /*ONE*/	\
		Operand a[1];\
		int pi=0;\
		char* l=NULL;	\
		if (p==NULL) return NULL; \
		{A;theVals[0]=val;} \
		l=NULL;				\
		if (theVals[0].kind==none) { \
			*err = FIRST_ARG_IS_INVALID;\
			return NULL;\
		} \
		code;		\
		REMOVE_WARNS		\
		return p; \
}

#define NONE(code) {if (p==NULL) return NULL; code; return p;}

#define MANY_NUMBERS(code) { \
	char* l; \
	int i,count=0;\
	if (p==NULL) return NULL; \
	l = p = strip(p," ");\
	while ((l=oneOf(getInteger(l, &i, err), ",\n"))!=((void *)0)) {count++;}\
	int* nums = (int*)malloc(sizeof(int)*count);\
	i=0;\
	l=p;\
	while ((l=oneOf(getInteger(l, &nums[i++], err), ",\n"))!=((void *)0));\
	code; \
	return p; \
}

#define STRING(code) { \
	char* text,*l; \
	if (p==NULL) return NULL; \
	p = strip(p," "); \
	l = charIs(allString(charIs(p, '"')), '"') ; \
	text = malloc((int)l-(int)p); \
	l = charIs(getAllString(charIs(p, '"'), text), '"') ; \
	code; \
	return p; \
}

#define GET_LINE_LABEL { \
	char* o=line; \
	if(!(isAlpha(line) && (l=charIs(getAllAlphasDigits(line, label), ':')))) { label[0]='\0'; l=o;} \
}

#define PARSE(line)  { \
	int err=0; \
	Label label;\
	char*l;\
	lineCounter++;\
	GET_LINE_LABEL;\
	l=strip(l, " ");\
	if(0) {}

char* trimNewline(char* line);

#define TRY(cmd)      else if (cmd (matchWordD(l, #cmd) , &context, &err, label, lineCounter, line)) {}
#define TRY_DOT(cmd)  else if (cmd (matchWordD(charIs(l, '.'), or(charIs(#cmd,'_'),#cmd)), &context, &err, label,lineCounter, line)) {}
#define ELSE(msg)     else if (!err){ printf("%s\n",msg);}\
					  else if (err) { \
						  switch(err) { \
						  	  case FIRST_ARG_IS_INVALID: printf("error in line %d: first argument of line '%s' is not valid\n", lineCounter, trimNewline(line));break; \
						  	  case SECOND_ARG_IS_INVALID:printf("error in line %d: second argument of line '%s' is not valid\n",lineCounter,  trimNewline(line));break; \
						  } \
					  }}

void func(Operand oper1, Operand oper2, Label label);
void debugPrint(Operand oper);

#define CMD(name) char* name(char* p, Context* context, int* err, char* label, int lineNumber, char* originalLine)

CMD(data);

CMD(cmp);
CMD(mov);
CMD(add);
CMD(sub);
CMD(lea);

CMD(prm);
CMD(not);
CMD(clr);
CMD(inc);
CMD(dec);
CMD(jmp);
CMD(bne);
CMD(red);
CMD(jsr);
CMD(rts);
CMD(stop);

CMD(_extern);
CMD(entry);
CMD(data);
CMD(string);

#endif /* ASMDSL_H_ */
