/*
 *  "dconsole.h" 
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

#if !defined __PFS_CONSOLE__
#define  __PFS_CONSOLE__
#if !defined __PFS_DIAG__
#include "ddiag.h"
#endif

/* #include "list.h" */
#include "pfsd_list.h"

#define PFSPRINT printf
#define pfsprint PFSPRINT
#define pfsp_debug(format,...)		\
	do \
		{\
			struct dhandle *hndl = cnsl_get_hndl();	\
			if ((hndl->flags & PFS_HNDL_FLAG_VERBOSITY_MASK) <= PFS_HNDL_FLAG_VERBOSITY_DEBUG) \
				printf(format,##__VA_ARGS__);		\
		}\
	while(0)

#define pfsp_info(format,...)		\
	do \
		{\
			struct dhandle *hndl = cnsl_get_hndl();	\
			if ((hndl->flags & PFS_HNDL_FLAG_VERBOSITY_MASK) <= PFS_HNDL_FLAG_VERBOSITY_INFO) \
				printf(format,##__VA_ARGS__);		\
		}\
	while(0)

#define pfsp_warn(format,...)		\
	do \
		{\
			struct dhandle *hndl = cnsl_get_hndl();	\
			if ((hndl->flags & PFS_HNDL_FLAG_VERBOSITY_MASK) <= PFS_HNDL_FLAG_VERBOSITY_WARN) \
				printf(format,##__VA_ARGS__);		\
		}\
	while(0)

#define pfsp_error(format,...)		\
	do \
		{\
			struct dhandle *hndl = cnsl_get_hndl();	\
			if ((hndl->flags & PFS_HNDL_FLAG_VERBOSITY_MASK) <= PFS_HNDL_FLAG_VERBOSITY_ERROR) \
				printf(format,##__VA_ARGS__);		\
		}\
	while(0)

#define pfsp_critical(format,...)		\
	do \
		{\
			struct dhandle *hndl = cnsl_get_hndl();	\
			if ((hndl->flags & PFS_HNDL_FLAG_VERBOSITY_MASK) <= PFS_HNDL_FLAG_VERBOSITY_CRITICAL) \
				printf(format,##__VA_ARGS__);		\
		}\
	while(0)


struct history_buffer {
	void               *data;
	unsigned int        count; /* used count      */
	struct list_head    pool;  /* List of buffers */
};

struct history_handle {
	struct history_buffer  *history_list;
};

extern int ddebugger(char *path);
extern void dprint_banner(void);
extern int history_list_init(struct history_handle *h_hndl);
extern int history_list_add(char *buf, size_t len, struct history_handle *h_hndl);
extern int history_list_show(struct history_handle *h_hndl);
extern struct history_handle *history_get_handle(void);
extern int history_list_get(struct history_handle *h_hndl, char *buf, int *cp);
extern int history_list_get_entry_num(struct history_handle *h_hndl, char **buf, int entry_num);
extern int dconsole_readline(char *buf,int  max_size, char *prompt);


/*
  Keep format the same as the web services reply

        self.reply_info['timestamp'] = datetime.datetime.utcnow().isoformat()
        self.reply_info['context'] = context
        self.reply_info['message'] = message
        self.reply_info['header'] = 0
        self.reply_info['payload'] = payload
        self.reply_info['duration'] = duration
*/

struct pfsc_exec_reply {
	char  *timestamp;
	char  *context;
	char  *message;
	char  *header;
	char  *payload;
	char  *duration;
};

extern void dconsole_show_json(struct pfsc_exec_reply *reply);
extern char *dconsole_get_json_str(struct pfsc_exec_reply *reply);

#endif /* __PFS_CONSOLE__ */
