/*
 *  "banner.c" 
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dcnsl.h"

char dbanner[];
char keyboard_banner[];

void dprint_banner(void)
{
	pfsprint("%s\n", dbanner);
}

char dbanner[] = \
"            \n"
"  \e[00;32m                  \n"
"  \e[01;32m            xx     \e[01;33m Welcome to dconsole\n"
"  \e[01;32m            xx     \e[01;33m Utilities and \n"
"  \e[01;32m xxxxxxxxxxxxx     \e[01;33m Diagnostics Shell\n"
"  \e[01;32m xxxxxxxxxxxxx     \n"
"  \e[00;32m                   \e[01;37m\n"
"  \e[01;31m xx          xx    \e[01;37m My Brain Is Full!\n"
"  \e[01;31m xx          xx    \n"
"  \e[01;31m xxxxxxxxxxxxxx    \n"
"  \e[01;31m xxxxxxxxxxxxxx    \e[01;37m Type \e[00;32m'?'\n"
"  \e[01;30m                   \e[01;37m to display available commands.\n"
"  \e[01;33m  xxxxxxxxxxxx     \n"
"  \e[01;33m xx          xx    \e[01;37m Type \e[00;32m'? <command>'\n"
"  \e[01;33m xx          xx    \e[01;37m to display help for specific commands.\n"
"  \e[01;33m xxxxxxxxxxxxxx    \n"
"\e[00m                        \n";


char keyboard_banner[] = \
" ____ ____ ____ ____ ____ ____ ____ _________ ____ ____\n"
"||W |||e |||l |||c |||o |||m |||e |||       |||T |||o ||\n"
"||__|||__|||__|||__|||__|||__|||__|||_______|||__|||__||\n"
"|/__\\|/__\\|/__\\|/__\\|/__\\|/__\\|/__\\|/_______\\|/__\\|/__\\|\n"
" ____ ____ ____ ____ ____ ____ ____ ____ _________\n"
"||P |||l |||a |||t |||f |||o |||r |||m |||       ||\n"     
"||__|||__|||__|||__|||__|||__|||__|||__|||_______||\n"  
"|/__\\|/__\\|/__\\|/__\\|/__\\|/__\\|/__\\|/__\\|/_______\\|\n"  
" ____ ____ ____ ____ ____ ____ ____ ____\n"  
"||S |||e |||r |||v |||i |||c |||e |||s ||\n"             
"||__|||__|||__|||__|||__|||__|||__|||__||\n"            
"|/__\\|/__\\|/__\\|/__\\|/__\\|/__\\|/__\\|/__\\|\n"
" ____ ____ ____ ____ ____ ____ ____ ____\n"
"||C |||o |||n |||s |||o |||l |||e |||  ||\n"             
"||__|||__|||__|||__|||__|||__|||__|||__||\n"            
"|/__\\|/__\\|/__\\|/__\\|/__\\|/__\\|/__\\|/__\\|\n";            

