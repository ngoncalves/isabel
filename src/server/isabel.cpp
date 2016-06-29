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

   See the header file for details.
 */

#include "isabel.h"
#include "isabelStartup.h"
#include "isabelServer.h"

#include <QtGlobal>
#include <QByteArray>
#include <QCoreApplication>

#include <cstdio>

/*--------------------- Private Variable Definitions ----------------*/

/* the TCP server that provides access to the application internals */
isabelServer *server = NULL;	

/* name of the environment variable that prevents multiple loading of the library */
#define ISABEL_IS_LOADED "ISABEL_IS_LOADED"  

/*--------------------- Private Function Declarations ---------------*/

/*--------------------- Public Function Declaration  ----------------*/

LIB_INIT_FUNC void isabel_initialize(void)
{
	fprintf(stderr,"[isabel] loaded library \n");

	if(qEnvironmentVariableIsSet(ISABEL_IS_LOADED))
	{
		fprintf(stderr,"[isabel] an instance was previously loaded, doing nothing \n");
	}
	else
	{ 
		qputenv(ISABEL_IS_LOADED,"true");

		/* create the Qt object that will wait for the application to start */
		isabelStartup* init = new isabelStartup(isabel_main);
		QObject::connect(init,SIGNAL(startupComplete()),init,SLOT(deleteLater()));
		init->watchForStartup();
	}
}

LIB_EXIT_FUNC void isabel_destroy(void)
{
	fprintf(stderr,"[isabel] unloading library \n");

	/* clean up used resources and unset the used resources */
	if(NULL != server)
	{
		qunsetenv(ISABEL_IS_LOADED);
		delete server;
	}
}

void isabel_main(void)
{
	/* get the port to use */
	int  port = DEFAULT_ISABEL_PORT;
	
	if(qEnvironmentVariableIsSet(ISABEL_PORT_ENV))
	{
		QByteArray value = qgetenv(ISABEL_PORT_ENV);
		port = value.toInt();
	}
	
	/* create the server */
	server = new isabelServer(port,QCoreApplication::instance());
}

/*--------------------- Private Function Definitions ----------------*/
