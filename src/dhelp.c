/*
 *  "dhelp.c" 
 *
 *  (c) COPYRIGHT 2018 IBM Corp.
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stdio.h>
#include "dhelp.h"
#include "dconsole.h"
#include "dcnsl.h"

static char *dconsole_help_strs[] = {
	"\nCommand: ? or HELP\n\n",
	"Description:  ? provides a utility to display help messages about commands\n",
	"              available at the parser level and tests.\n",
	"              <> = required arguments, [] = optional arguments,\n",
	"              * = command not available\n",
	"              Abbreviated versions are shown in uppercase\n",
	"              Address & Data are hex values and should be preceded by '0x'.\n\n",  
	"Syntax: ?                - display this message\n",
	"        ?  <command>     - display help for <command>\n",
	" ---------------------------------------------------------------------------\n",
	"  EXit      Exit parser\n",  
	"  EXEC      Execute shell command\n",  
	"  JEXEC     Execute shell command, output is in JSON format\n",  
	"  RUN       Execute tests\n",  
	"  LIST      List tests\n",  
	"  LOAD      Load a test suite\n",  
	"  ENABLE    Enable tests\n",  
	"  DISABLE   Disables tests\n",  
	"  SET       Set parser variable\n",  
	"  UNSET     Unset parser variable\n",  
	"  SUMMARY   Show test pass/fail summary\n",  
	"\nCalculator: The parser supports a calculator.\n",
	"              Available operators are:\n",
	"              +, -, *, /, %, <<, >>, &, |, ^, ~, (, )\n",
	""

};


int dhelp(void)
{
	int i;  

	for( i = 0;i < (sizeof( dconsole_help_strs ) / sizeof( char *) ); i++ )
		pfsprint("%s",dconsole_help_strs[i] );

	return 0;
}

static char *exit_cmd_help[]  = {
	"\nCommand: EXIT\n\n",
	"Description: The EXIT command is used to exit the parser\n",
	" Syntax: EXIT -Exit Parser\n" 
};


static char *exec_cmd_help[]  = {
	"\nCommand: EXEC\n\n",
	"Description: The EXEC command is used to execute shell commands\n",
	" Syntax: EXEC \"<shell command string>\"\n",
	" Example: EXEC \"ls -als\"\n"
};

static char *jexec_cmd_help[]  = {
	"\nCommand: JEXEC\n\n",
	"Description: The JEXEC command is used to execute shell commands. Results are in JSON format\n",
	"             Refer to web-services API for format of JSON string.\n",
	" Syntax: JEXEC \"<shell command string>\"\n",
	" Example: JEXEC \"ls -als\"\n"
};

#if defined PFS_CONSOLE_HISTORY
static char *history_cmd_help[]  = {
	"\nCommand: History\n\n",
	"Description: Show the command history\n",
	" Syntax: History\n",
	"\nCommand: !<num>\n\n",
	"Description: Run command <num> from the history list\n",
	" Syntax: !5  - runs command 5 from the history list\n"

};
#endif

static char run_cmd_help[] =
	"\nCommand: RUN\n\n"
	"Description: The Run command is used to execute tests\n"
	" Syntax: Run <test spec> <loop spec>\n"
	" Example: Run 1-10 loop 5\n";

static char enable_cmd_help[] =
	"\nCommand: ENABLE\n\n"
	"Description: the enable command is used to enable disabled tests\n"
	" Syntax: enable <test spec>\n"
	" Example: enable 1-3\n";

static char disable_cmd_help[] =
	"\nCommand: DISABLE\n\n"
	"Description: the disable command is used to disable tests\n"
	" Syntax: disable <test spec>\n"
	" Example: disable 1-3\n";

static char list_cmd_help[] =
	"\nCommand: LIST\n\n"
	"Description: The List command lists the tests\n"
	" Syntax: LIST\n";


static char load_cmd_help[] =
	"\nCommand: LOAD\n\n"
	"Description: The load command loads a test suite\n"
	"   Typing load by itself will display the available test suites\n"
	" Syntax: LOAD <test suite>\n";

static char set_cmd_help[] =
	"\nCommand: SET\n\n"
	"Description: The set command sets parser variables\n"
	"   Typing set by itself will display the variable\n"
	" Syntax: set <variable> <value>\n";

static char unset_cmd_help[] =
	"\nCommand: UNSET\n\n"
	"Description: The unset command unsets parser variables\n"
	"   Typing unset by itself will clear the variable\n"
	" Syntax: unset <variable>\n";


static char summary_cmd_help[] =
	"\nCommand: SUMMARY\n\n"
	"Description: Display the pass/fail status for the current\n"
	"   test list or clear the summary\n"
	" Syntax: summary <clear>\n";


int dcli_help( int command )
{
	int i;

	switch( command ){
	case EXIT:
		for( i = 0;i < (sizeof( exit_cmd_help ) / sizeof( char *) ); i++ )
			pfsprint("%s",exit_cmd_help[i] );
		break;
	case EXEC:
		for( i = 0;i < (sizeof( exec_cmd_help ) / sizeof( char *) ); i++ )
			pfsprint("%s",exec_cmd_help[i] );
		break;
	case JEXEC:
		for( i = 0;i < (sizeof( jexec_cmd_help ) / sizeof( char *) ); i++ )
			pfsprint("%s",jexec_cmd_help[i] );
		break;
	case RUN:
		pfsprint("%s",run_cmd_help );
		break;
	case LIST:
		pfsprint("%s",list_cmd_help );
		break;
	case LOAD:
		pfsprint("%s",load_cmd_help );
		break;
	case ENABLE:
		pfsprint("%s",enable_cmd_help );
		break;
	case DISABLE:
		pfsprint("%s",disable_cmd_help );
		break;
	case SET:
		pfsprint("%s",set_cmd_help );
		break;
	case UNSET:
		pfsprint("%s",unset_cmd_help );
		break;
	case SUMMARY:
		pfsprint("%s",summary_cmd_help );
		break;
#if defined PFS_CONSOLE_HISTORY
	case HISTORY:
		for( i = 0;i < (sizeof( history_cmd_help ) / sizeof( char *) ); i++ )
			pfsprint("%s",history_cmd_help[i] );
		break;
#endif
	default:
		pfsprint("invalid command %d\n", command );
		break;
	}
	
	return 0;
}








