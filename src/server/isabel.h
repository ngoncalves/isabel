/*
   Isabel
   =========
   Copyright (C) 2016  Nelson Gonçalves
   
   License
   -------

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Summary
   -------

   The library entry point.
 */
#ifndef __ISABEL_H__
#define __ISABEL_H__

/*--------------------- Public Variable Declarations ----------------*/
#define DEFAULT_ISABEL_PORT (4242)				/* the server listening port */
#define ISABEL_PORT_ENV     "ISABEL_PORT"		/* name of the environment variable, that sets the port */

#if defined(__linux__) || defined(__APPLE__) || defined(__unix__)
	#define LIB_INIT_FUNC __attribute__((constructor))
	#define LIB_EXIT_FUNC __attribute__((destructor))
#else
	#define LIB_INIT_FUNC
#endif

/*--------------------- Public Function Declarations ----------------*/

/* The library initialization function.

	This function is executed as soon as the library is loaded.
	It then launches the server main function.
*/
LIB_INIT_FUNC void isabel_initialize(void);

/* Main server function, which runs in its own thread.

	This function creates the TCP server which listens
	for the requests from the client or the inspector.
 */
void isabel_main(void);

#endif
