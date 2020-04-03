/*
 ***************************************************************************
 *
 *   dconsole.y
 *  
 *   *  (c) COPYRIGHT 2018 IBM Corp.
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
 **************************************************************************
 */

%{


unsigned long ap_s_addr ;        /* start address                */
unsigned long ap_e_addr ;        /* end address                  */

/*
** Install the "C" code here so all variables will be declared and
** there will be no need to forward
** reference.
*/


#include <stdio.h>
#include <ctype.h>
/*#include <stdtype.h>*/
#include <setjmp.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdint.h>
#include "dhelp.h"
#include "dconsole.h"
#include "ddiag.h"
#include "dcnsl.h"
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <libudev.h>
#include <json-c/json.h>
#include <dlfcn.h>
#include <glib.h>

extern int yyparse (void);
extern void yyrestart (FILE *input_file  );
extern int yylex (void);

extern FILE *yyin, *yyout;
extern char *yytext;
extern char *tident;
extern char *tstring;
extern int dcnsldbglineno;
extern void lex_init(void);

#define TMPSTRBUFFSIZE 120

char *history_buff;
FILE *apdiagin;
FILE *apdiagout;

int   history_state;        /* last input to parser was history command */
int   history_command;      /* history command number                   */

#if defined DCNSL_CONSOLE_HISTORY
static int history_lineno_enable;
#endif

#define ENVSIZE 80

int gbl_rtn_status = -1;

jmp_buf              /* control-c handler     */
  jmpbuf;

jmp_buf       /* unexpected exceptions */
  exception_jmpbuf;

static struct dhandle *dhndl;

static int total_failures =0;

void ctlc_handler(int sig)
{
   longjmp(jmpbuf, sig);
}

struct dl_symbol_table st[] = {
	 {
		 (void *(*)(void *))cnsl_get_hndl,
		 "cnsl_get_hndl"
	 },
	 {
		 (void *(*)(void *))cnsl_get_random_test,
		 "cnsl_get_random_test"
	 },
	 {
		 (void *(*)(void *))dpts_run_script,
		 "dpts_run_script"
	 },
	 {
		 (void *(*)(void *))dlogger_debug,
		 "dlogger_debug"
	 },
	 {
		 (void *(*)(void *))dlogger_info,
		 "dlogger_info"
	 },
	 {
		 (void *(*)(void *))dlogger_warn,
		 "dlogger_warn"
	 },
	 {
		 (void *(*)(void *))dlogger_error,
		 "dlogger_error"
	 },
	 {
		 (void *(*)(void *))dlogger_critical,
		 "dlogger_critical"
	 }
};


int ddebugger(char *path, void *conf_ptr)
{
	int status = 0;
	int rc;
	int tnum;
	GKeyFile *gkf_ptr = (GKeyFile *)conf_ptr;
	GError *gerror;
	gsize  glength;

	 dcnsldbglineno = 1;
	 ap_s_addr = 0x0;        /* start address                */
	 ap_e_addr = 0x0;        /* end address                  */
	 yyin  = stdin ;
	 yyout = stdout;

	 signal(SIGINT, ctlc_handler);

	 dhndl = (struct dhandle *)malloc(sizeof(struct dhandle));
	 if ( dhndl == NULL ){
		 dcnslprint(" ** ERROR: allocating memory\n");
		 return -1;
	 }

	 bzero( dhndl, sizeof(struct dhandle));

	 /*
	 zlog_info((zlog_category_t *)(dhndl->zc), "hello, zlog");
	 zlog_critical((zlog_category_t *)(dhndl->zc), "xxxx, zlog");
	 */
         cnsl_set_hndl(dhndl);

	 /* test symbol table method of cnsl_get_hndl */
	 dhndl = (struct dhandle *)(*((void *(*)(void))st[0].p2f))();
	 tnum =  (*((int(*)(int, int))st[1].p2f))(5,10);
	 /* printf ("%d\n", tnum ); */

	 dprint_banner();
	 dcnslprint("\n");

	 dhndl->cnsl_path = path;
	 dhndl->table = dpts_get_table(0);

	 dhndl->num_tests = dpts_init(dhndl->table);
	 dhndl->loop_count=1;
	 dhndl->loop_decrement=1;
	 dhndl->flags = DCNSL_HNDL_FLAG_VERBOSITY_INFO | DCNSL_HNDL_FLAG_LOGGING_DEBUG;

	 dhndl->gkf_ptr = conf_ptr;

	 /* Get the drive information for SDC */
	 dhndl->sdc_drives = g_key_file_get_string_list (gkf_ptr,"sdc drives","drives", &glength, &gerror );
	
	if (dhndl->sdc_drives == NULL){
		fprintf (stderr, "Error reading group information for sdc drives");
		fprintf(stderr, "%s", gerror->message);
	}		

	 dhndl->sdc_host_drives = g_key_file_get_string_list (gkf_ptr,"sdc drives","host_drives", &glength, &gerror );

	if (dhndl->sdc_host_drives == NULL){
		fprintf (stderr, "Error reading group information for sdc host drives");
		fprintf(stderr, "%s", gerror->message);
	}		

#if defined __DEBUG_SDC_DRIVES__
	int i=0;
	while ( *(dhndl->sdc_drives+i) != NULL ){
		printf("drive=%s\n", *(dhndl->sdc_drives+i));
		i++;
	}

	i=0;
	while ( *(dhndl->sdc_host_drives+i) != NULL ){
		printf("drive=%s\n", *(dhndl->sdc_host_drives+i));
		i++;
	}
#endif

	 if(setjmp(jmpbuf) == 0){  /*save stuff for CTLC interrupt return*/
		 /* Nothing to see here */
	 }
	 else { /* longjump got us here */
		 dcnslp_debug(" Longjump got us here !!!\n");
		 /* Need to kill back ground processes ?? */
                 yyrestart(yyin);
	 }

#if 0    
	 dcnslp_info("Starting PFS Console\n");
	 dcnslp_debug("XStarting PFS Console\n");
	 dcnslp_warn("Ytarting PFS Console\n");
#endif

	 lex_init();

	 status = yyparse();

	 /* printf("yyparse exit status = %d\n", status); */

         free(dhndl);

	 return status;

 }


int yyerror ( s )
char  *s;
{
	dcnslprint("Error: %s\n",s );
	return 1;
}


/* extern int alloca(); */


%}


%union {
	/* unsigned long int uval; */
    unsigned long long uval;
    long long sval;
    char *cptr ;  
    char sgle_char;
 }

%token LEXCOMMA
%token EOL
%token COLON
%token EQUALS
%token OPAREN
%token CPAREN
%token IDENT
%token HELP
%token EXIT
%token H2D
%token D2H
%token SEMICOLON
%token ATSIGN
%token BANG
%token DOUBLEQUOTE
%token SINGLEQUOTE
%token EXEC
%token STRING
%token UPARROW
%token HISTORY
%token LOAD
%token RUN
%token LIST
%token LOOP
%token RANDOM
%token SET
%token UNSET
%token U32
%token U64
%token ADDRESS
%token ON
%token OFF
%token DISABLE
%token CLEAR
%token ENABLE
%token SHOW
%token FLAG
%token MASK
%token CNSLFAIL
%token STOP
%token CONTINUE
%token SUMMARY
%token GET
%token PORT
%token STATUS
%token JEXEC
%token VERBOSITY
%token LOGGING
%token DEBUG
%token INFO
%token WARN
%token ERROR
%token CRITICAL

%left BITOR
%left BITXOR
%left BITAND
%left LEFTSHIFT 
%left RIGHTSHIFT
%left MINUS PLUS
%left MULT DIV MODULUS
%right BITCOMP
%nonassoc UMINUS

%token <sval> NUMBER
%token <uval> HEXNUMBER

%type <uval> hex_num
%type <sval> num
/*			
%type <uval> address
%type <uval> locations
*/
%type <uval> expression
%type <cptr> text

%start session

%%

session      :  commandlist ;

commandlist  : command EOL { 
               if( setjmp( exception_jmpbuf ) != 0 ){
                 yyrestart(yyin);
		 PFSPRINT("type return to continue..");
		 YYERROR;
	       }
             }
             | commandlist command EOL { 
               if( setjmp( exception_jmpbuf ) != 0 ){
                 yyrestart(yyin);
		 PFSPRINT("type return to continue..");
		 YYERROR;
	       }
             }
             | error EOL { 
               yyerrok; 
               if( setjmp( exception_jmpbuf ) != 0 ){
                 yyrestart(yyin);
		 PFSPRINT("type return to continue..");
		 YYERROR;
	       }
             }
             ;

command :  commands {}
        |   command SEMICOLON command
        ;

commands :    /* nothing */    {
            dcnsldbglineno--;
        }
        |   hex_2_decimal_cmd
        |   decimal_2_hex_cmd
        |   calc
        |   help
	|   exec_cmd
	|   load_cmd
	|   list_cmd
	|   run_cmd
	|   exit_cmd
	|   history_cmd
	|   disable_cmd
	|   enable_cmd
	|   set_cmd
	|   unset_cmd
	|   summary_cmd
    	;


exec_cmd : EXEC IDENT {
              int ret;
	      ret = dpts_run_script((char *)tident);
	      /* ret = system((const char *)tident); */
              gbl_rtn_status = ret;
              if (tident)
                 free(tident);
	    }
	    | EXEC IDENT STRING {
              int ret;
              char tmpstr[256];
              char tmpstr1[1024];

              memset(tmpstr,0,256);
              memset(tmpstr1,0,1024);
              strncpy( tmpstr, tstring+1, strlen(tstring)-2);
              sprintf(tmpstr1,"%s %s", tident, tmpstr );

	      ret = dpts_run_script((char *)tmpstr1);
	      /* ret = system((const char *)tmpstr1); */
              gbl_rtn_status = ret;

              if (tstring)
                 free(tstring);
              if (tident)
                 free(tident);
	    }
	    | EXEC STRING {
              int ret;
              char tmpstr[256];

              memset(tmpstr,0,256);

              strncpy( tmpstr, tstring+1, strlen(tstring)-2);
	      ret = dpts_run_script((char *)tmpstr);
              /* ret = system((const char *)tmpstr); */

              gbl_rtn_status = ret;
              free(tstring);
	    }
	    | JEXEC STRING {
              int ret;
              char tmpstr[256];

	      /* JSON support */
	      char *jreply = NULL;
	      struct dcnslc_exec_reply *reply = malloc(sizeof(struct dcnslc_exec_reply));;

	      /* time & duration */
              struct timeval tv, tv_end;
	      time_t nowtime;
	      struct tm *nowtm;
	      char tmptbuf[64];
              unsigned long seconds;
              char duration_str[DCNSL_MAX_DURATION_STR];
	      char line[1024];

	      /* popen method */
	      FILE *jfp;
#if defined (JEXEC_USE_MALLOC)
	      char *pbuf= malloc(1024*1024);
	      memset(pbuf,0,1024*1024);
#else
	      char *pbuf= NULL;
#endif
              memset(tmpstr,0,256);

	      /* duration and timestamp start */
	      gettimeofday(&tv,NULL);
	      nowtime = tv.tv_sec;
	      nowtm = localtime(&nowtime);
	      strftime(tmptbuf, sizeof(tmptbuf),"%Y-%m-%dT%H:%M:%S", nowtm);

	      reply->context = "dcnslconsole";
	      reply->timestamp = tmptbuf;
	      reply->message = tmpstr;
	      reply->header = "na";

              strncpy( tmpstr, tstring+1, strlen(tstring)-2);
	      jfp = popen(tmpstr,"r");
	      if(jfp == NULL){
		      reply->payload = "FAIL";
		      ret = 1;
	      }
	      else {
#if defined (JEXEC_USE_MALLOC)
		      while(fgets(line,sizeof(line),jfp) != NULL){
			      strncat(pbuf,line,(size_t)(1024*1024));
		      }
		      reply->payload = pbuf;
		      pclose(jfp);
		      ret = 0;
#else
		      pbuf = NULL;
		      while(fgets(line,sizeof(line),jfp) != NULL){
			      /* printf("line size=%d\n", strlen(line)); */
			      if (pbuf != NULL){
				      pbuf = (char *)realloc(pbuf, strlen(pbuf) + (size_t)strlen(line)+1); 
			      }
			      else {
				      pbuf = (char *)malloc(strlen(line)+1);
				      memset(pbuf,0,sizeof(*pbuf));
			      }
			      strcat(pbuf,line);
		      }
		      if(pbuf != NULL)
			      reply->payload = pbuf;
		      else
			      reply->payload = "none";
		      pclose(jfp);
		      ret = 0;
#endif
	      }
              /* ret = system((const char *)tmpstr); */

	      /* duration */
	      gettimeofday(&tv_end,NULL);
	      seconds = tv_end.tv_sec - tv.tv_sec;
	      cnsl_duration( seconds, duration_str );

	      /* put together the JSON reply */
	      reply->duration = duration_str;

	      jreply = dconsole_get_json_str(reply);
	      if (jreply != NULL){
		      /* dcnslprint("%s\n",jreply); */
		      dcnslp_info("%s\n",jreply);
		      free(jreply);
	      }
              gbl_rtn_status = ret;
              free(tstring);
	      free(reply);
              if ( pbuf != NULL )
		      free(pbuf);
	    }
	    ;

history_cmd : HISTORY {
            }
	    |  BANG num {
#if defined DCNSL_CONSOLE_HISTORY
                    history_buff=NULL;
                    history_list_get_entry_num(history_get_handle(), &history_buff, $2);
                    /* this part will eventually tie into YY_INPUT to implement this 
                       part of the history command. It's a start */
                    if (history_buff){
		        dcnslprint("%d=%s\n", $2, history_buff);
                        free(history_buff);
                        history_buff=NULL;
                    }
                    else {
		        dcnslprint("%d not found!\n", $2);
                    }
#endif
            }
            ;

load_cmd : LOAD {
	      dplist_show();
	    }
            | LOAD IDENT {
              if (dplist_load((char *)tident) >= 0){
                     dhndl->table = dpts_get_table(0);
                     dhndl->num_tests = dpts_init(dhndl->table);
                     dcnslprint(" -- Test suite %s loaded. Number of tests=%d\n",(char *)tident,
                              dhndl->num_tests);
	             dpts_show(dhndl->table);
              }
              else {
                    dcnslprint(" Fail: table %s not found\n",(char *)tident);
              }
	    }
            ;

list_cmd : LIST {
	      dpts_show(dhndl->table);
	    }
            ;

summary_cmd : SUMMARY {
	      dpts_summary(dhndl->table);
	    }
	    | SUMMARY CLEAR {
	      dpts_summary_clear(dhndl->table);
            }
            ;

run_cmd : RUN run_list loop_spec {
	      int tnum=0;
	      int single_mode;
              struct timeval tv_start, tv_end;
              unsigned long seconds;
              char duration_str[DCNSL_MAX_DURATION_STR];
              int qyn_y = 1;
              unsigned int flags;

	      /*
	       * Some tests Should not be part of a loop so we need to know if 
	       * that mode was defined.
	       */
	      if ((dhndl->loop_count==1) && (dhndl->loop_decrement==1))
		      single_mode=1;
	      else
		      single_mode=0;
            
	      /*
	       * Cycle through everything that's enabled
	       */
              while ( dhndl->loop_count ){
                  tnum = 0;
  	          while ((dhndl->table+tnum)->p2f != NULL){
                      flags = (dhndl->table+tnum)->flags;
                      if ( dhndl->vars.flags & DCNSL_VARS_USE_MASK )
                         flags &= dhndl->vars.mask;
		      if ((dhndl->table+tnum)->run){
                              if (DCNSL_DF_RUN_QUERY_YN  & flags){
                                      qyn_y = cnsl_yn(" ** Destructive Test **");
                              }
			      if ( qyn_y  && (( !single_mode && ((DCNSL_DF_RUN_ONCE & flags)==0)) || single_mode) ){
				      dcnslp_info(" -- Running test [%4d] %s\n", tnum+1, (dhndl->table+tnum)->description);
                                      gettimeofday(&tv_start,NULL);
				      (dhndl->table+tnum)->status=(dhndl->table+tnum)->p2f(flags,(void *)st);
                                      gettimeofday(&tv_end,NULL);
                                      seconds = tv_end.tv_sec - tv_start.tv_sec;
                                      cnsl_duration( seconds, duration_str );
				      dcnslp_info(" -- Done status = %s, %s\n", 
                                             (dhndl->table+tnum)->status==DCNSL_DIAG_PASS?"PASS":"FAIL", duration_str);

                                      if ((dhndl->table+tnum)->status == DCNSL_DIAG_FAIL && ((dhndl->flags & DCNSL_HNDL_FLAG_FAIL_CONTINUE)==0)){
					      total_failures++;
                                             (dhndl->table+tnum)->summary.num_fail++;
				      }
                                      else
                                             (dhndl->table+tnum)->summary.num_pass++;
                                      if ((dhndl->table+tnum)->status == DCNSL_DIAG_FAIL && ((dhndl->flags & DCNSL_HNDL_FLAG_FAIL_CONTINUE)==0)){
                                          dhndl->loop_count =1;
                                          dhndl->loop_decrement =1;
                                          break;
                                      }   
			      }
			      else {
				      dcnslp_info(" -- Bypass test [%4d] %s\n", tnum+1, (dhndl->table+tnum)->description);
                              }
                              qyn_y = 1;
		      }
		      tnum++;
                  }
                  dhndl->loop_count -= dhndl->loop_decrement;
	      }	
	      /* clear the run flag */
	      dpts_clear_run(dhndl->table);
              /* set the default loop count */
              dhndl->loop_count=1;
              dhndl->loop_decrement=1;
              dhndl->flags &= ~DCNSL_HNDL_FLAG_RANDOM;
	    }
	    |  RUN RANDOM {
	            int i=0;
                    int tnum;
                    struct timeval tv_start, tv_end;
                    unsigned long seconds;
                    char duration_str[DCNSL_MAX_DURATION_STR];
                    int qyn_y = 1;
                    unsigned int flags;

		    for (i=0;i < dhndl->num_tests;i++){
			    if (!(dhndl->table+i)->disable)
				    (dhndl->table+i)->run =1;
			    (dhndl->table+i)->status =0;
		    }
                    while (1) {
                          tnum =  cnsl_get_random_test( 1, dhndl->num_tests );
                          if ( tnum < 0){
                                  dcnslp_error(" Error: Generating random test number\n");
                                  break;
                          }
                          if (!cnsl_validate_range(dhndl,tnum, tnum)){
                                  dcnslp_error(" Error: Invalid test number %d\n", tnum);
                                  break;
                          }
                          tnum--;  /* zero based here on out */
                          flags = (dhndl->table+tnum)->flags;
                          if ( dhndl->vars.flags & DCNSL_VARS_USE_MASK )
                                   flags &= dhndl->vars.mask;
 		          if ((dhndl->table+tnum)->run){
                                   if (DCNSL_DF_RUN_QUERY_YN  & flags){
                                      qyn_y = cnsl_yn(" ** Destructive Test **");
                                   }
                          }
                          if (!qyn_y)
                             break;

   		          dcnslp_info(" -- Running test [%4d] %s\n", tnum+1, (dhndl->table+tnum)->description);

                          gettimeofday(&tv_start,NULL);
			  (dhndl->table+tnum)->status=(dhndl->table+tnum)->p2f(flags,(void *)st);
                          gettimeofday(&tv_end,NULL);
                          seconds = tv_end.tv_sec - tv_start.tv_sec;
                          cnsl_duration( seconds, duration_str );
		          dcnslp_info(" -- Done status = %s, %s\n", 
                                     (dhndl->table+tnum)->status==DCNSL_DIAG_PASS?"PASS":"FAIL", duration_str);
                          if ((dhndl->table+tnum)->status == DCNSL_DIAG_FAIL && ((dhndl->flags & DCNSL_HNDL_FLAG_FAIL_CONTINUE)==0)){
				  total_failures++;
				  (dhndl->table+tnum)->summary.num_fail++;
			  }
                          else
				  (dhndl->table+tnum)->summary.num_pass++;

                          if ((dhndl->table+tnum)->status == DCNSL_DIAG_FAIL && ((dhndl->flags & DCNSL_HNDL_FLAG_FAIL_CONTINUE)==0)){
				  break;
                          }   

                   }
	           dhndl->flags |= DCNSL_HNDL_FLAG_RANDOM;
            }

            ;

loop_spec : /* nothing */ {
                dhndl->loop_count=1;
                dhndl->loop_decrement=1;
            }
  	    | LOOP {
                dhndl->loop_count=1;
                dhndl->loop_decrement=0;
            }
	    | LOOP num {
                dhndl->loop_count=$2;
                dhndl->loop_decrement=1;
            }                
            ;

run_list : run_spec {
            }
            | run_list LEXCOMMA run_spec {
            }
            ;

run_spec : /* nothing */ {
	            int i=0;
		    for (i=0;i < dhndl->num_tests;i++){
			    if (!(dhndl->table+i)->disable)
				    (dhndl->table+i)->run =1;
			    (dhndl->table+i)->status =0;
		    }
            }
            | num {
		    if (cnsl_validate_range(dhndl,$1, $1)){
			    if (!(dhndl->table+$1)->disable)
				    (dhndl->table+$1-1)->run =1;
			    (dhndl->table+$1-1)->status =0;
		    }
            }
            | num MINUS num {
		    int i;
		    if (cnsl_validate_range(dhndl,$1, $3))
			    for (i=$1-1;i < $3; i++)  {
				    if (!(dhndl->table+i)->disable)
					    (dhndl->table+i)->run =1;
				    (dhndl->table-+i)->status =0;
			    }
            }
            ;

disable_cmd : DISABLE disable_list {
             }
             | DISABLE CLEAR {
		     dpts_clear_disable(dhndl->table);
             }
             ;

disable_list : disable_spec {
            }
            | disable_list LEXCOMMA disable_spec {
            }
            ;

disable_spec : /* nothing */ {
	            int i=0;
		    for (i=0;i < dhndl->num_tests;i++){
			    (dhndl->table+i)->disable =1;
		    }
            }
            | num {
		    if (cnsl_validate_range(dhndl,$1, $1)){
			    (dhndl->table+$1-1)->disable =1;
		    }
            }
            | num MINUS num {
		    int i;
		    if (cnsl_validate_range(dhndl,$1, $3))
			    for (i=$1-1;i < $3; i++)  {
				    (dhndl->table+i)->disable =1;
			    }
            }
            ;

enable_cmd : ENABLE enable_list {
             }
             ;

enable_list : enable_spec {
            }
            | enable_list LEXCOMMA enable_spec {
            }
            ;

enable_spec : /* nothing */ {
	            int i=0;
		    for (i=0;i < dhndl->num_tests;i++){
			    (dhndl->table+i)->disable =0;
		    }
            }
            | num {
		    if (cnsl_validate_range(dhndl,$1, $1)){
			    (dhndl->table+$1-1)->disable =0;
		    }
            }
            | num MINUS num {
		    int i;
		    if (cnsl_validate_range(dhndl,$1, $3))
			    for (i=$1-1;i < $3; i++)  {
				    (dhndl->table+i)->disable =0;
			    }
            }
            ;

set_cmd : SET {
		    dcnslprint(" address   64 bit hex value\n");
		    dcnslprint(" mask      32 bit hex value\n");
		    dcnslprint(" U32       32 bit hex value\n");
		    dcnslprint(" U64       32 bit hex value\n");
		    dcnslprint(" fail      stop, continue\n");
		    dcnslprint(" verbosity debug, info, warn, error, critical\n"
			     "------------------------------------------------\n");
                    if ( dhndl->vars.flags & DCNSL_VARS_USE_ADDRESS )
			    dcnslprint(" address: 0x%jx\n", dhndl->vars.addr);
                    else
 			    dcnslprint(" address: not set\n");
                    if ( dhndl->vars.flags & DCNSL_VARS_USE_MASK )
			    dcnslprint(" mask:    0x%08x\n", dhndl->vars.mask);
                    else
 			    dcnslprint(" mask:    not set\n");
                    if ( dhndl->vars.flags & DCNSL_VARS_USE_U32 )
			    dcnslprint(" U32:     0x%08x\n", dhndl->vars.u32);
                    else
 			    dcnslprint(" U32:     not set\n");
                    if ( dhndl->vars.flags & DCNSL_VARS_USE_U64 )
			    dcnslprint(" U64:     0x%jx\n", dhndl->vars.u64);
                    else
 			    dcnslprint(" U64:     not set\n");
                    dcnslprint(" flags: 0x%08x\n fail:      %s\n", dhndl->flags, 
			     dhndl->flags & DCNSL_HNDL_FLAG_FAIL_CONTINUE?"continue":"stop");
                    dcnslprint(" verbose : %s\n", cnsl_get_verbosity_str(dhndl));
                    dcnslprint(" loglevel: %s\n", cnsl_get_logging_str(dhndl));

            }
            | SET ADDRESS hex_num {
                    dhndl->vars.flags |= DCNSL_VARS_USE_ADDRESS;
                    dhndl->vars.addr = $3;
		    dcnslprint(" address: 0x%jx\n", dhndl->vars.addr);

            }
            | SET MASK expression {
                    dhndl->vars.flags |= DCNSL_VARS_USE_MASK;
                    dhndl->vars.mask = $3;
		    dcnslprint(" mask: 0x%08x\n", dhndl->vars.mask);

            }
            | SET U64 hex_num {
                    dhndl->vars.flags |= DCNSL_VARS_USE_U64;
                    dhndl->vars.u64 = $3;
		    dcnslprint(" U64: 0x%jx\n", dhndl->vars.u64);

            }
            | SET U32 hex_num {
                    dhndl->vars.flags |= DCNSL_VARS_USE_U32;
                    dhndl->vars.u32 = (uint32_t)$3;
		    dcnslprint(" U32: 0x%08x\n", dhndl->vars.u32);

            }
            | SET CNSLFAIL STOP {
                    dhndl->flags &= ~DCNSL_HNDL_FLAG_FAIL_CONTINUE;
                    dcnslprint(" fail: %s\n", 
                              dhndl->flags & DCNSL_HNDL_FLAG_FAIL_CONTINUE?"Continue":"Stop");
            }
            | SET CNSLFAIL CONTINUE {
                    dhndl->flags |= DCNSL_HNDL_FLAG_FAIL_CONTINUE;
                    dcnslprint(" fail: %s\n", 
                             dhndl->flags & DCNSL_HNDL_FLAG_FAIL_CONTINUE?"Continue":"Stop");
            }
            | SET VERBOSITY DEBUG {
		    dhndl->flags &= ~DCNSL_HNDL_FLAG_VERBOSITY_MASK;
                    dhndl->flags |= DCNSL_HNDL_FLAG_VERBOSITY_DEBUG;
            }
            | SET VERBOSITY INFO {
		    dhndl->flags &= ~DCNSL_HNDL_FLAG_VERBOSITY_MASK;
                    dhndl->flags |= DCNSL_HNDL_FLAG_VERBOSITY_INFO;
            }
            | SET VERBOSITY WARN {
		    dhndl->flags &= ~DCNSL_HNDL_FLAG_VERBOSITY_MASK;
                    dhndl->flags |= DCNSL_HNDL_FLAG_VERBOSITY_WARN;
            }
            | SET VERBOSITY ERROR {
		    dhndl->flags &= ~DCNSL_HNDL_FLAG_VERBOSITY_MASK;
                    dhndl->flags |= DCNSL_HNDL_FLAG_VERBOSITY_ERROR;
            }
            | SET VERBOSITY CRITICAL {
		    dhndl->flags &= ~DCNSL_HNDL_FLAG_VERBOSITY_MASK;
                    dhndl->flags |= DCNSL_HNDL_FLAG_VERBOSITY_CRITICAL;
            }
            | SET LOGGING DEBUG {
		    dhndl->flags &= ~DCNSL_HNDL_FLAG_LOGGING_MASK;
                    dhndl->flags |= DCNSL_HNDL_FLAG_LOGGING_DEBUG;
            }
            | SET LOGGING INFO {
		    dhndl->flags &= ~DCNSL_HNDL_FLAG_LOGGING_MASK;
                    dhndl->flags |= DCNSL_HNDL_FLAG_LOGGING_INFO;
            }
            | SET LOGGING WARN {
		    dhndl->flags &= ~DCNSL_HNDL_FLAG_LOGGING_MASK;
                    dhndl->flags |= DCNSL_HNDL_FLAG_LOGGING_WARN;
            }
            | SET LOGGING ERROR {
		    dhndl->flags &= ~DCNSL_HNDL_FLAG_LOGGING_MASK;
                    dhndl->flags |= DCNSL_HNDL_FLAG_LOGGING_ERROR;
            }
            | SET LOGGING CRITICAL {
		    dhndl->flags &= ~DCNSL_HNDL_FLAG_LOGGING_MASK;
                    dhndl->flags |= DCNSL_HNDL_FLAG_LOGGING_CRITICAL;
            }
            ;


unset_cmd : UNSET {
                    dhndl->vars.flags &= ~DCNSL_VARS_USE_ADDRESS;
                    dhndl->vars.flags &= ~DCNSL_VARS_USE_U32;
                    dhndl->vars.flags &= ~DCNSL_VARS_USE_U64;
            }
            | UNSET ADDRESS {
                    dhndl->vars.flags &= ~DCNSL_VARS_USE_ADDRESS;
            }
            | UNSET MASK {
                    dhndl->vars.flags &= ~DCNSL_VARS_USE_MASK;
            }
            | UNSET U64 {
                    dhndl->vars.flags &= ~DCNSL_VARS_USE_U64;
            }
            | UNSET U32 {
                    dhndl->vars.flags &= ~DCNSL_VARS_USE_U32;
            }
            ;

hex_2_decimal_cmd : H2D hex_num {
              dcnslprint("0x%016llx %llu\n", $2, $2 );
	    }
            ;

decimal_2_hex_cmd : D2H num {
              dcnslprint("%llu 0x%016llx\n", $2, $2 );
	    }
            ;

calc      : expression {
              dcnslprint(" = %llu 0x%016llx\n", $1 ,$1 );
          }
          ;

help      : HELP {
              dhelp();
          }
	  | HELP text {
		  dcnslprint(" Help for %s does not exist !\n", (char *)$2);
          }
	  | HELP EXIT {
                  dcli_help(EXIT);
          }
	  | HELP EXEC {
                  dcli_help(EXEC);
          }
	  | HELP JEXEC {
                  dcli_help(JEXEC);
          }
	  | HELP HISTORY {
                  dcli_help(HISTORY);
          }
	  | HELP LIST {
                  dcli_help(LIST);
          }
	  | HELP RUN {
                  dcli_help(RUN);
          }
	  | HELP SET {
                  dcli_help(SET);
          }
	  | HELP UNSET {
                  dcli_help(UNSET);
          }
          ;

expression : expression PLUS expression { $$ = $1 + $3; }
           | expression MINUS expression { $$ = $1 - $3; }         
           | expression MULT expression { $$ = $1 * $3; }         
           | expression DIV expression { 
                   if($3 == 0 )
                       yyerror("Divide By Zero");
                   else
                       $$ = $1 / $3; 
           } 
           | expression MODULUS expression { $$ = $1 % $3; }    
           | expression BITOR expression { $$ = $1 | $3; }         
           | expression BITXOR expression { $$ = $1 ^ $3; }         
           | expression BITAND expression { $$ = $1 & $3; }         
           | expression LEFTSHIFT expression { $$ = (unsigned long long) $1 << $3; }       
           | expression RIGHTSHIFT expression { $$ = (unsigned long long)$1 >> $3; }    
           | BITCOMP expression { $$ = ~$2; }         
           | MINUS expression %prec UMINUS {$$ = -$2; }
           | OPAREN expression CPAREN  { $$ = $2; }
           | num     { $$ = $1; }
           | hex_num { $$ = $1; }
           ;

num     : NUMBER { 
               $$ = $1;  

           }
           ;

hex_num : HEXNUMBER {
               $$ = $1; 
	   }
           ;

text    :  IDENT  {
 	       $$ = yytext;
/*               strcat( aptmpstrbuff, " " );
               strcat( aptmpstrbuff, yytext ); */
	   }
	   ;

exit_cmd : EXIT  { 
              dcnslprint("exiting....\n");
	      if (total_failures){
		      dcnslprint("%d total failures\n", total_failures);
		      YYABORT;
	      }
              YYACCEPT; 
           } 
           ;


/* start of subroutines section */


%%


