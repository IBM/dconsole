/*
 *  "dsrand.c" 
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
 * PRNG. Nothing fancy here. Lifted from the KISS generator
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "dcnsl.h"

#define MAX_STREAMS 32

 /* Seed variables  */
static unsigned long x[MAX_STREAMS], y[MAX_STREAMS], z[MAX_STREAMS], c[MAX_STREAMS];

int dcnsld_srand32_init_stream(unsigned long seed, int stream)
{ 
	if (stream < 0 || stream >= MAX_STREAMS)
		return EINVAL;
	if ( seed == 0 ){
		x[stream]=123456789; 
		y[stream]=362436000;
		z[stream]=521288629;
		c[stream]=7654321;
		return 0;
	}
	
	x[stream] = y[stream] = z[stream] = c[stream] = seed;

	return 0;
}

int dcnsld_srand32_init(unsigned long seed)
{ 
	int i;
	int status=0;
	for (i=0; i< MAX_STREAMS; i++){
		status=dcnsld_srand32_init_stream(seed, i);
		if ( status )
			return status;
	}
	return 0;
}


unsigned int dcnsld_srand32(int stream)
{ 
	unsigned long long t, a=698769069ULL;

	/* default to stream zero if we are out of range */
	if (stream < 0 || stream >= MAX_STREAMS)
		stream=0;
	x[stream]=69069*x[stream]+12345; 
	/* y must never be set to zero! */
	y[stream]^=(y[stream]<<13); y[stream]^= (y[stream]>>17); y[stream]^= (y[stream]<<5);
	t=a*z[stream]+c[stream]; c[stream]=(t>>32);/* Also avoid setting z=c=0! */
	return 	x[stream]+y[stream]+(z[stream]=t); 
}

