/*
 *  "ddiag.h" 
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

#if !defined __PFS_DIAG__
#define  __PFS_DIAG__

#include <stdint.h>

#define PFS_MAX_DURATION_STR 256
#define PFS_DIAG_FAIL (-1)
#define PFS_DIAG_PASS (0)

#define PFS_DF_VERBOSE          (1<<0)
#define PFS_DF_CONTINUE_ON_FAIL (1<<1)
#define PFS_DF_RUN_ONCE         (1<<2)
#define PFS_DF_RUN_QUERY_YN     (1<<3)

struct ddiag_summary {
	int num_pass;
	int num_fail;
};

#define DECLARE_TEST( p, s, f )	\
	int p( unsigned int flags, void *symtbl )

struct ddiagnostic {
	/* int (*p2f)(unsigned int,...); */
	int (*p2f)(unsigned int, void *symtbl);
	char *description;
	unsigned int flags;
	int disable;
	int status;
	int run;
	struct ddiag_summary summary;
};



#define PFS_HNDL_FLAG_FAIL_CONTINUE (1<<0)
#define PFS_HNDL_FLAG_RANDOM        (1<<1)
/* Verbosity Flags. The naming follows python logging (except for 'none') */
#define PFS_HNDL_FLAG_VERBOSITY_NONE     0
#define PFS_HNDL_FLAG_VERBOSITY_LSB      2
#define PFS_HNDL_FLAG_VERBOSITY_DEBUG    (1<<PFS_HNDL_FLAG_VERBOSITY_LSB)
#define PFS_HNDL_FLAG_VERBOSITY_INFO     (2<<PFS_HNDL_FLAG_VERBOSITY_LSB)
#define PFS_HNDL_FLAG_VERBOSITY_WARN     (3<<PFS_HNDL_FLAG_VERBOSITY_LSB)
#define PFS_HNDL_FLAG_VERBOSITY_ERROR    (4<<PFS_HNDL_FLAG_VERBOSITY_LSB)
#define PFS_HNDL_FLAG_VERBOSITY_CRITICAL (5<<PFS_HNDL_FLAG_VERBOSITY_LSB)
#define PFS_HNDL_FLAG_VERBOSITY_MASK  (PFS_HNDL_FLAG_VERBOSITY_DEBUG | PFS_HNDL_FLAG_VERBOSITY_INFO | \
				       PFS_HNDL_FLAG_VERBOSITY_WARN | PFS_HNDL_FLAG_VERBOSITY_ERROR | \
				       PFS_HNDL_FLAG_VERBOSITY_CRITICAL )
#define PFS_HNDL_FLAG_LOGGING_NONE     0
#define PFS_HNDL_FLAG_LOGGING_LSB      6
#define PFS_HNDL_FLAG_LOGGING_DEBUG    (1<<PFS_HNDL_FLAG_LOGGING_LSB)
#define PFS_HNDL_FLAG_LOGGING_INFO     (2<<PFS_HNDL_FLAG_LOGGING_LSB)
#define PFS_HNDL_FLAG_LOGGING_WARN     (3<<PFS_HNDL_FLAG_LOGGING_LSB)
#define PFS_HNDL_FLAG_LOGGING_ERROR    (4<<PFS_HNDL_FLAG_LOGGING_LSB)
#define PFS_HNDL_FLAG_LOGGING_CRITICAL (5<<PFS_HNDL_FLAG_LOGGING_LSB)
#define PFS_HNDL_FLAG_LOGGING_MASK  (PFS_HNDL_FLAG_LOGGING_DEBUG | PFS_HNDL_FLAG_LOGGING_INFO | \
				       PFS_HNDL_FLAG_LOGGING_WARN | PFS_HNDL_FLAG_LOGGING_ERROR | \
				       PFS_HNDL_FLAG_LOGGING_CRITICAL )

#define PFS_VARS_USE_ADDRESS (1<<0)
#define PFS_VARS_USE_U32     (1<<1)
#define PFS_VARS_USE_U64     (1<<2)
#define PFS_VARS_USE_MASK    (1<<3)

/* variables */
struct dvariables {
	uint64_t addr;
	uint32_t u32;
	uint64_t u64;
	uint64_t flags;
	int      verbosity;
	unsigned int mask;
};

struct dl_symbol_table {
	void *(*p2f)(void *);
	char name[64];
};
	
struct dhandle {
	struct ddiagnostic *table;
	struct dvariables vars;
	int num_tests;
	unsigned int flags;
	int loop_count;
	int loop_decrement;
	char *cnsl_path;
	void *zc;
	int zc_level;
	/* symbol table provided to dynamically linked diags */
	struct dl_symbol_table *dl_symtbl;
};

struct ddp_table_list_entry {
	struct ddiagnostic *table;
	char   description[32];
};

extern struct ddiagnostic *dpts_get_table(unsigned int flags);
extern int dpts_show(struct ddiagnostic *tbl);
extern int dpts_init(struct ddiagnostic *tbl);
extern int dpts_clear_run(struct ddiagnostic *tbl);
extern int dpts_clear_disable(struct ddiagnostic *tbl);
extern int dpts_run_script(char *cmdstr);
extern int dplist_show(void);
extern int dplist_load(char *);
extern int dpts_summary(struct ddiagnostic *tbl);
extern int dpts_summary_clear(struct ddiagnostic *tbl);
extern void cnsl_show_flags(struct dhandle *hndl);
extern int cnsl_validate_range(struct dhandle *hndl,int r1, int r2);
extern void cnsl_set_hndl(struct dhandle *hndl);
extern struct dhandle *cnsl_get_hndl(void);
extern int cnsl_yn(const char *msg);
extern int cnsl_get_random_test( int sn, int en );
extern void cnsl_duration( unsigned long total_seconds, char *dstr );
extern char *cnsl_get_verbosity_str(struct dhandle *hndl);
extern char *cnsl_get_logging_str(struct dhandle *hndl);

#endif /* __PFS_DIAG__ */
