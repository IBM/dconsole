/*
 *  "libpfsdiag.h" 
 *
 *  (c) COPYRIGHT 2018 IBM Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#if !defined( __LIBPFSDIAG__ )
#define __LIBPFSDIAG__

#define DECLARE_TEST( p, s, f )	\
	int p( unsigned int flags )

/*
 * Psuedo Randon Number Generator.
 */
#define PFSD_SRAND_MAX_STREAMS 256

unsigned int pfsd_srand32(int stream);
int pfsd_srand32_init_stream(unsigned long seed, int stream);
int pfsd_srand32_init(unsigned long seed);
void pfs_logger(char *, unsigned int);
void pfs_set_debug_level(unsigned int);
unsigned int pfs_get_debug_level(void);

#define PFS_FAIL "FAIL: "
#define PFS_INFO "INFO: "
#define PFS_WARN "WARN: "
#define PFS_DEBUG "DEBUG: "

#define PFS_PRINT( format, args...) do \
		{ \
			fprintf(stdout, PFS_INFO format, ## args);	\
			fflush(stdout); \
		} while(0)

#define PFS_PRINT_FAIL( format, args...) do \
		{ \
			fprintf(stderr, PFS_FAIL format, ## args);	\
			fflush(stderr); \
		} while(0)

#define PFS_PRINT_WARN( format, args...) do \
		{ \
			fprintf(stderr, PFS_WARN format, ## args);	\
			fflush(stderr); \
		} while(0)

#define PFS_PRINT_DEBUG( level, format, args...) do	\
		{ \
		        if ( level >= pfs_get_debug_level()){			\
		               fprintf(stderr, PFS_DEBUG format, ## args);		\
			       fflush(stderr);					\
			} \
		} while(0)



#endif /*__LIBPFSDIAG__*/
