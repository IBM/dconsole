/*
 ***************************************************************************
 *
 *       COPYRIGHT 2015 IBM Corp
 *  
 *  File   : dconsole.l
 *  Author : Jim Entenmann
 *  Date   : 6/23/2015
 *
 *  Description : lex specification for pfs console.
 *
 *  change history:
 * 
 *        6/23/2015  jentenma   initial coding
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * Example of case sensitive rule ( no -i option to flex )
 *    {E}{X}       { return EXIT; }
 *    {E}{X}{I}    { return EXIT; }
 *    {E}{X}{I}{T} { return EXIT; }
 *
 **************************************************************************
 */

/* this increases the stack size */
%e 3000


%{

/* this is an alternate input function that lex (not flex) could use */
/*
#define input() (((((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)),output(yytchar),yytchar)
*/

#include "dconsole.h"
#include <stdio.h>
#include "ddiag.h"
#include "dcnsl.h"

int junk;
int tnum;
char *tident;
char *tstring;

/*
 * tracks current line number
 */
int pfsdbglineno ;

/*************************************************/
/*                                               */
/*  undefine the functions we need to so         */
/*  we can make the output of lex                */
/*  palatable to the platform services console   */
/*  interface.                                   */
/*                                               */
/*************************************************/


static char *glbuf;

/* the yyget_string is a modified version of the get_string function
   that is more palatable to lex */
#define YY_NO_INPUT
#ifndef YY_INPUT
#undef YY_INPUT

#define HISTORY_LIST_ADD(...)\
	do { } while(0)
#define HISTORY_GET_HANDLE(...)\
	do { } while(0)

#define YY_INPUT(buf,result,max_size) \
	do \
		{ \
		        glbuf = malloc(max_size);		       \
			if (glbuf == NULL){			       \
				pfsp_error("Error Allocating Buffer");   \
				break;				       \
			}					       \
			result = dconsole_readline(glbuf,max_size,"dconsole> "); \
			if ( result != -1 ) { \
				strncpy((char *)buf, (const char *)glbuf, max_size); \
			} \
			free(glbuf); \
		} \
	while ( 0 )

#endif /* ifndef YY_INPUT */

/* the abort function will reset the box. This is probably
   not a good thing but I can deal with this later if it becomes a problem
   which I don't think it will. */
#ifdef YY_FATAL_ERROR
#undef YY_FATAL_ERROR
#define YY_FATAL_ERROR(msg) \
	do \
		{ \
		(void) pfsp_critical( "%s\n", msg ); \
		_abort( 1 ); \
		} \
	while ( 0 )
#endif

/* the builtin echo function uses a fputs. We don't need to do 
   this. */

#ifdef ECHO
#undef ECHO
#define ECHO (void) pfsprint("%s",yytext )
#endif

%}

digit		[0-9]
alpha		[A-Za-z]
alphanum	[A-Za-z0-9_]
hexdigit        [A-Fa-f0-9]
zero            0

A	[Aa]
B	[Bb]
C	[Cc]
D	[Dd]
E	[Ee]
F	[Ff]
G	[Gg]
H	[Hh]
I	[Ii]
J	[Jj]
K	[Kk]
L	[Ll]
M	[Mm]
N	[Nn]
O	[Oo]
P	[Pp]
Q	[Qq]
R	[Rr]
S	[Ss]
T	[Tt]
U	[Uu]
V	[Vv]
W	[Ww]
X	[Xx]
Y	[Yy]
Z	[Zz]

eol	\n

%%

"exec"     { return EXEC; }
"ex"       { return EXIT; }
"exi"      { return EXIT; }
"exit"     { return EXIT; }
"history"  { return HISTORY; }
"histor"   { return HISTORY; }
"histo"    { return HISTORY; }
"hist"     { return HISTORY; }
"his"      { return HISTORY; }
"hi"       { return HISTORY; }
"h"        { return HISTORY; }
"help"     { return HELP; }
"hd"       { return H2D; }
"dh"       { return D2H; }
"load"     { return LOAD; }
"loop"     { return LOOP; }
"run"      { return RUN; }
"list"     { return LIST; }
"random"   { return RANDOM; }
"rando"    { return RANDOM; }
"rand"     { return RANDOM; }
"set"      { return SET; }
"unset"    { return UNSET; }
"u32"      { return U32; }
"u64"      { return U64; }
"address"  { return ADDRESS; }
"addres"   { return ADDRESS; }
"addre"    { return ADDRESS; }
"addr"     { return ADDRESS; }
"on"       { return ON; }
"off"      { return OFF; }
"disable"  { return DISABLE; }
"clear"    { return CLEAR; }
"enable"   { return ENABLE; }
"show"     { return SHOW; }
"flag"     { return FLAG; }
"mask"     { return MASK; }
"fail"     { return CNSLFAIL; }
"stop"     { return STOP; }
"cont"     { return CONTINUE; }
"conti"    { return CONTINUE; }
"contin"   { return CONTINUE; }
"continu"  { return CONTINUE; }
"continue" { return CONTINUE; }
"summary"  { return SUMMARY; }
"get"      { return GET; }
"port"     { return PORT; }
"status"   { return STATUS; }
"jexec"    { return JEXEC; }
"verbosity" { return VERBOSITY; }
"verbose" { return VERBOSITY; }
"logging"  { return LOGGING; }
"loglevel" { return LOGGING; }
"debug"    { return DEBUG; }
"info"     { return INFO; }
"warn"     { return WARN; }
"error"    { return ERROR; }
"critical" { return CRITICAL; }

\!              { return BANG; }
\"              { return DOUBLEQUOTE; }
\'              { return SINGLEQUOTE; }
\=		{ return EQUALS; }
\:		{ return COLON; }
\;		{ return SEMICOLON; }
\,              { return LEXCOMMA; }
\+              { return PLUS;  }
\*              { return MULT;  }
\/              { return DIV;   }
\-              { return MINUS; }
\(              { return OPAREN; }
\)              { return CPAREN; }
\|              { return BITOR;  }
\&              { return BITAND; }
\^              { return BITXOR; }
\~              { return BITCOMP; }
\%              { return MODULUS; }
\@              { return ATSIGN; }

\<\<          { return LEFTSHIFT; }
\>\>          { return RIGHTSHIFT; }
\?            { return HELP; }

\224\72       { return UPARROW; }

{alpha}{alphanum}*  {
                tident= malloc(yyleng);
                strcpy(tident,yytext);
                return IDENT  ; 
              }

{digit}+	         { /* yylval.sval = atoi( yytext ); */
	                   sscanf(yytext,"%lld", &yylval.sval );
                           return NUMBER; }

{zero}{X}{hexdigit}+     { /*yylval.uval = atoh( yytext );*/
 	                   sscanf(yytext,"%llx", &yylval.uval );
                           return HEXNUMBER; }

\"([^\\\"]|\\.)*\" { 
                tstring= malloc(yyleng);
                strcpy(tstring,yytext);
                return STRING  ; 
              }

[ \t] ;

<<EOF>> return EOL;

{eol}	{ pfsdbglineno++; return EOL; }

[.\t]\n { return EOL ; }

.	return yytext[0];

%%

#ifdef yywrap
#undef yywrap
#endif

int yywrap(){
    unput('\n'); 
    unput('X');
    unput('E');
    return 0;
}



/*
{digit}+	         { sscanf( yytext,"%d",&yylval.sval ); 
                           return NUMBER; }

{zero}{X}{hexdigit}+     { sscanf( yytext,"%x",&yylval.uval );
                           return HEXNUMBER; }
*/

void lex_init()
{

	return;

	/* This stuff was causing a seg fault */
/* points to current character in buffer */
/*YY_CHAR *yy_c_buf_p = (YY_CHAR *) 0;*/
yy_init = 1;		/* whether we need to initialize */
yy_start = 0;	/* start state number */

}




