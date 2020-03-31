/*
 *  "dlogger.c" 
 *
 *  (c) COPYRIGHT 2020 IBM Corp.
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
#include <stdarg.h>
#include <string.h>
#include <readline/readline.h>
#include <dirent.h>

#include "dconsole.h"
#include "ddiag.h"
#include "dcnsl.h"
#include <dlfcn.h>
#include <regex.h>

#define dcp_p_red "\033[1;31m"
#define dcp_p_green "\033[1;32m"
#define dcp_p_yellow "\033[1;33m"
#define dcp_p_none "\033[0m"

void dlogger_debug(char *format, ...)
{
	va_list ap;
	char buf[256];
	int len;
	memset(buf,0,256);
	strcpy(buf,dcp_p_green"DEBUG: "dcp_p_none);
	len = strlen(buf);
	va_start(ap, format);
	vsnprintf(buf+len, 256-len, format, ap);
	dcnsl_str_pdebug(buf);
	return;
}

void dlogger_info(char *format, ...)
{
	va_list ap;
	char buf[256];
	int len;
	memset(buf,0,256);
	strcpy(buf,dcp_p_green"INFO: "dcp_p_none);
	len = strlen(buf);
	va_start(ap, format);
	vsnprintf(buf+len, 256-len, format, ap);
	dcnsl_str_pinfo(buf);
	return;
}

void dlogger_warn(char *format, ...)
{
	va_list ap;
	char buf[256];
	int len;
	memset(buf,0,256);
	strcpy(buf,dcp_p_yellow"WARN: "dcp_p_none);
	len = strlen(buf);
	va_start(ap, format);
	vsnprintf(buf+len, 256-len, format, ap);
	dcnsl_str_pwarn(buf);
	return;
}

void dlogger_error(char *format, ...)
{
	va_list ap;
	char buf[256];
	int len;
	memset(buf,0,256);
	strcpy(buf,dcp_p_red"ERROR: "dcp_p_none);
	len = strlen(buf);
	va_start(ap, format);
	vsnprintf(buf+len, 256-len, format, ap);
	dcnsl_str_perror(buf);
	return;
}

void dlogger_critical(char *format, ...)
{
	va_list ap;
	char buf[256];
	int len;
	memset(buf,0,256);
	strcpy(buf,dcp_p_red"CRITICAL: "dcp_p_none);
	len = strlen(buf);
	va_start(ap, format);
	vsnprintf(buf+len, 256-len, format, ap);
	dcnsl_str_pcritical(buf);
	return;
}
