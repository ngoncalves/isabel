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

   This module defines an object that assists the loading of the library.
   This is done in four steps:
   	1. create a thread
   	2. in that thread, blocking wait until the target application has started
   	3. wait for the application event loop has started
   	4. create the TCP server that listens for requests
 */
#ifndef __ISABEL_STARTUP_H__
#define __ISABEL_STARTUP_H__

#include <QObject>

/*--------------------- Public Variable Declarations ----------------*/

/* function pointer to an initialization function */
typedef void (*T_INIT_FUNCTION)(void);

/*--------------------- Public Class Declarations --------------------*/

class isabelStartup : public QObject
{
	Q_OBJECT

public:
	/*
		Constructor for this class.

		@initFunc 	the function that initializes the server 
	*/
	isabelStartup(T_INIT_FUNCTION initFunc);

	/* Calls the private method ::doWait() in a separate thread. 
	*/
	void watchForStartup(void);

	/* Call the function to create the server.
	 */
	Q_INVOKABLE void runInitFunc(void);		


Q_SIGNALS:
	void startupComplete(void);	/* emited after the initialization function has finished */

private:
	/* Wait until the target application has started. And then
	   connect runInitFunc to be called once the main event loop
	   has begun.
	*/
	void doWait();

private:
	T_INIT_FUNCTION server_init;
};

#endif