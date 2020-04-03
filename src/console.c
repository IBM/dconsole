/*
 *  "console.c" 
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
#include <unistd.h>
#include <string.h>
#include "dcnsl.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <libudev.h>
#include <json-c/json.h>
#include <glib.h>

#define MAXPATHLEN 256

int init_window(void);
int cleanup(void);

int main (int argc, char **argv)
{

	char *path=argv[0];
	int status = 0;
	char *ret;
	const char ch = '/';
	int length;
	char fullpath[MAXPATHLEN];
	GKeyFile *gkf_ptr;
	GError *gerror;
	gsize  glength;

	/* /proc/self is a symbolic link to the process-ID subdir
	 * of /proc, e.g. /proc/4323 when the pid of the process
	 * of this program is 4323.
	 *
	 * Inside /proc/<pid> there is a symbolic link to the
	 * executable that is running as this <pid>.  This symbolic
	 * link is called "exe".
	 *
	 * So if we read the path where the symlink /proc/self/exe
	 * points to we have the full path of the executable.
	 */

	length = readlink("/proc/self/exe", fullpath, sizeof(fullpath));
     
	/* Catch some errors: */
	if (length < 0) {
		fprintf(stderr, "Error resolving symlink /proc/self/exe.\n");
		exit(EXIT_FAILURE);
	}
	if (length >= MAXPATHLEN) {
		fprintf(stderr, "Path too long. Truncated.\n");
		exit(EXIT_FAILURE);
	}

	/* I don't know why, but the string this readlink() function 
	 * returns is appended with a '@'.
	 */
	fullpath[length] = '\0';       /* Strip '@' off the end. */

	printf("Full path is: %s\n", fullpath);

	gkf_ptr = g_key_file_new();

	if (!g_key_file_load_from_file(gkf_ptr, "/etc/dconsole/dconsole.conf", G_KEY_FILE_NONE, &gerror)){
		fprintf (stderr, "Could not read config file %s\n", "/etc/dconsole/dconsole.conf");
		fprintf(stderr, "%s", gerror->message);
		return EXIT_FAILURE;
	}

#if 0
	ret = strrchr(path, ch);
	printf ("0x%x %s\n", ret, path);
	*ret=0; 
#endif
        

	init_window();
	dcnsld_srand32_init(/*PFSD_SRAND_CNSL_STREAM*/255);

	status = ddebugger(fullpath, (void *)gkf_ptr);
	cleanup();
	g_key_file_free (gkf_ptr);
	return status;
}

int init_window(void)
{
	return 0;
}

int cleanup(void)
{
	return 0;
}

int dconsole_readline(char *buf,int  max_size, char *prompt)
{
	char *input;
	int len;
	memset((void *)buf, 0, max_size);	
	input = readline(prompt);

	if(!input)
		return 0;
	len = strlen(input);
	if(len)
		add_history(input);
	strncpy(buf,input,max_size);
	strcat(buf,"\n");
	free(input);
	return len+1;
}

/*
  Keep format the same as the web services reply

        self.reply_info['timestamp'] = datetime.datetime.utcnow().isoformat()
        self.reply_info['context'] = context
        self.reply_info['message'] = message
        self.reply_info['header'] = 0
        self.reply_info['payload'] = payload
        self.reply_info['duration'] = duration
*/

void dconsole_show_json(struct dcnslc_exec_reply *reply)
{
	json_object *reply_object;

	reply_object = json_object_new_object();
	json_object_object_add(reply_object, "timestamp", json_object_new_string(reply->timestamp));
	json_object_object_add(reply_object, "context", json_object_new_string(reply->context));
	dcnslprint("\t%s\n", json_object_to_json_string(reply_object));
	json_object_put(reply_object);
}

char  *dconsole_get_json_str(struct dcnslc_exec_reply *reply)
{
	json_object *reply_object;
	const char *jstr = NULL;
	char *json_str = NULL;

	reply_object = json_object_new_object();
	json_object_object_add(reply_object, "timestamp", json_object_new_string(reply->timestamp));
	json_object_object_add(reply_object, "context", json_object_new_string(reply->context));
	json_object_object_add(reply_object, "message", json_object_new_string(reply->message));
	json_object_object_add(reply_object, "header", json_object_new_string(reply->header));
	json_object_object_add(reply_object, "payload", json_object_new_string(reply->payload));
	json_object_object_add(reply_object, "duration", json_object_new_string(reply->duration));
	jstr = json_object_to_json_string(reply_object);
	json_str = malloc(strlen(jstr)+32);
        strcpy(json_str, jstr);
	json_object_put(reply_object);
	return json_str;
}


void dconsole_show_verbosity(void)
{

}


