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
#include <string.h>
#include <readline/readline.h>
#include <dirent.h>

#include "dconsole.h"
#include "ddiag.h"
#include "dcnsl.h"
#include <dlfcn.h>
#include <regex.h>

void dlogger_debug(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	return;
}

void dlogger_info(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	return;
}

void dlogger_warn(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	return;
}

void dlogger_error(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	return;
}

void dlogger_critcal(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	return;
}
