/*
 *  "ddispatcher.c" 
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <dirent.h>

#include "dconsole.h"
#include "ddiag.h"
#include "dcnsl.h"
#include <dlfcn.h>
#include <regex.h>

#if 1
struct ddp_table_list_entry dispatch_table_list[] =  {
        { NULL, }};
#else
extern struct ddp_table_list_entry dispatch_table_list[];
#endif

static struct dhandle *cnsl_hndl_ptr=NULL;
static struct ddiagnostic *current_table = NULL;

struct ddiagnostic *dpts_get_table(unsigned int flags)
{
	if (current_table == NULL)
		dplist_load("test");
	return current_table;
}



int dplist_show(void)
{
	DIR *d;
	struct dirent *dir;
	int i = 0;
	regex_t re;
	regmatch_t rm[2];
	char *tofind = "_dc.so$";
	int ret;
	char strtoprint[64];

	if (regcomp(&re, tofind, REG_EXTENDED) != 0){
		fprintf(stderr, "Failed to compile regex '%s'\n", tofind);
		return -1;
	}

	d = opendir("/usr/local/lib");
	if ( d ) {
	        while ((dir = readdir(d)) != NULL) {
			ret = regexec(&re, dir->d_name, (size_t)0,NULL,0);
			if ( ret == 0){
				bzero(strtoprint,64);
				strncpy(strtoprint, dir->d_name+3, strlen(dir->d_name)-9);
				printf("%-20s (%s)\n", strtoprint, dir->d_name);
				i++;
			}
		}
		closedir(d);

	}
	d = opendir("/usr/lib");
	if ( d ) {
	        while ((dir = readdir(d)) != NULL) {
			ret = regexec(&re, dir->d_name, (size_t)0,NULL,0);
			if ( ret == 0){
				bzero(strtoprint,64);
				strncpy(strtoprint, dir->d_name+3, strlen(dir->d_name)-9);
				printf("%-20s (%s)\n", strtoprint, dir->d_name);
				i++;
			}
		}
		closedir(d);
	}

	regfree(&re);
	return i;
}


int dplist_load(char *name)
{
	int i=0;
	int found = 0;
	char libname[64];
	char dispatch_table_name[128];
	void *dllib;

	sprintf(libname,"lib%s_dc.so", name);
	sprintf(dispatch_table_name,"%s_dispatch_table", name);
	
	printf(" -- Loading library %s\n", libname );
	dllib = dlopen(libname, RTLD_LAZY);

	if ( dllib == NULL ){
		printf ("Error opening %s\n",libname);
		return -1;
	}

	current_table = (struct ddiagnostic *)dlsym(dllib, dispatch_table_name);
	if ( current_table != NULL ) {
		printf(" -- Loaded table %s\n", dispatch_table_name );
	}
	else {
		printf(" -- Error: Loading table %s\n", dispatch_table_name );
		return -1;
	}

	return 0;
}


int dpts_show(struct ddiagnostic *tbl)
{
	int i=0;
	dcnslprint("\n%-6s %-40s %-11s %-10s %-10s\n","tnum","Description", "Flags", "disable", "status");
	dcnslprint("-------------------------------------------------------------------------------\n");
	while (tbl->p2f){
		dcnslprint("[%4d] %-40s 0x%08x %7d %10d\n",i+1,tbl->description, tbl->flags, tbl->disable, tbl->status);
		tbl++;
		i++;
	}
	return i;
}

int dpts_init(struct ddiagnostic *tbl)
{
	int i=0;
	while (tbl->p2f){
		tbl->run = 0;
		tbl->status = 0;
		tbl->summary.num_pass=0;
		tbl->summary.num_fail=0;
		tbl++;
		i++;
	}
	return i;
}

int dpts_summary(struct ddiagnostic *tbl)
{
	int i=0;
	dcnslprint("\n%-6s %-40s %-8s %-8s %-10s\n","tnum","Description", "pass", "fail", "status");
	dcnslprint("-------------------------------------------------------------------------------\n");
	while (tbl->p2f){
		dcnslprint("[%4d] %-40s %4d %7d %7d\n",
		       i+1,tbl->description, tbl->summary.num_pass, tbl->summary.num_fail, tbl->status);
		tbl++;
		i++;
	}
	return i;
}

int dpts_summary_clear(struct ddiagnostic *tbl)
{
	int i=0;
	while (tbl->p2f){
		tbl->summary.num_pass=0;
		tbl->summary.num_fail=0;
		tbl++;
		i++;
	}
	return i;
}


int dpts_clear_run(struct ddiagnostic *tbl)
{
	int i=0;
	while (tbl->p2f){
		tbl->run = 0;
		tbl++;
		i++;
	}
	return i;
}

int dpts_clear_disable(struct ddiagnostic *tbl)
{
	int i=0;
	while (tbl->p2f){
		tbl->disable = 0;
		tbl++;
		i++;
	}
	return i;
}


/* dispatch table support:  script/shell command execution */
int dpts_run_script_old(char *cmdstr)
{
	int status;
	status = system((const char *)cmdstr);
	if (status != 0)
		return DCNSL_DIAG_FAIL;
	return DCNSL_DIAG_PASS;
}

/* dispatch table support:  script/shell command execution */
int dpts_run_script(char *cmdstr)
{
	FILE *fp;
	char path[1035];
	int status;

	/* Open the command for reading. */
	fp = popen(cmdstr, "r");
	if (fp == NULL) {
		dcnslp_error("Failed to run command %s\n",cmdstr );
		return DCNSL_DIAG_FAIL;
	}

	/* Read the output a line at a time - output it. */
	while (fgets(path, sizeof(path), fp) != NULL) {
		dcnslp_info("%s", path);
	}

	/* close */
	status = pclose(fp);

	if (!status)
		return DCNSL_DIAG_PASS;
	else
		return DCNSL_DIAG_FAIL;
}


void cnsl_show_flags(struct dhandle *hndl)
{
}

char *cnsl_get_verbosity_str(struct dhandle *hndl)
{
	int level = hndl->flags & DCNSL_HNDL_FLAG_VERBOSITY_MASK;
	level = level >> DCNSL_HNDL_FLAG_VERBOSITY_LSB;

	char *verbosity_levels[] = {
		"none", "debug", "info", "warn", "error", "critical" };
	
	return verbosity_levels[level];
}

char *cnsl_get_logging_str(struct dhandle *hndl)
{
	int level = hndl->flags & DCNSL_HNDL_FLAG_LOGGING_MASK;
	level = level >> DCNSL_HNDL_FLAG_LOGGING_LSB;

	char *logging_levels[] = {
		"none", "debug", "info", "warn", "error", "critical" };
	
	return logging_levels[level];
}

int cnsl_validate_range(struct dhandle *hndl,int r1, int r2)
{
	if ((r1 > 0) && (r1 <= r2) && (r2 <= hndl->num_tests))
		return 1;
	dcnslp_debug(" Out of Range\n");
	return 0;
}

void cnsl_set_hndl(struct dhandle *hndl)
{
	cnsl_hndl_ptr=hndl;
}

struct dhandle *cnsl_get_hndl(void)
{
	return cnsl_hndl_ptr;
}

int cnsl_yn(const char *msg)
{
	int yn = 0;
	char *input = NULL;

	if (msg)
		dcnslprint("%s\n",msg);	

	while (1){
		input = readline(" Are you sure you want to continue?[YES/NO] ");
		if(!input)
			continue;
		if (strcmp("YES", input)==0){
			yn=1;
			break;
		}
		if (strcmp("NO", input)==0){
			yn=0;
			break;
		}
	}
	if (input)
		free(input);
	return yn;
}

int cnsl_get_random_test( int sn, int en )
{
	int num_tests = en-sn+1;
	int tnum;

	if (num_tests < 0)
		return -1;

	while (1){
		tnum = (dcnsld_srand32(/*PFSD_SRAND_CNSL_STREAM*/255) % num_tests) +1;
		if (tnum >= sn && tnum <= en)
			break;
	}
	return tnum;
}

void cnsl_duration( unsigned long total_seconds, char *dstr )
{

	unsigned int hours = 0;
	unsigned int minutes =0;
	unsigned int seconds=0;
	
	if (total_seconds > 0) {
		hours = total_seconds/60/60;
		minutes = total_seconds/60%60;
		seconds = total_seconds%60;
	}

	if (dstr == NULL){
		dcnslp_info("total seconds %lu (%uh:%um:%us)\n", total_seconds, hours,minutes,seconds);
		return;
	}

	snprintf(dstr, DCNSL_MAX_DURATION_STR, "total seconds %lu (%uh:%um:%us)", total_seconds, hours,minutes,seconds);
	return;
}
