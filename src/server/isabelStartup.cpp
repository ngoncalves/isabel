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
 */
#include "isabelStartup.h"

#include <QtConcurrent>

/*--------------------- Private Variable Declarations ----------------*/

#define STARTUP_SLEEP_WAIT 	(1000)					/* wait time in miliseconds, in between checks to see if the application has started */
#define STARTUP_MAX_WAIT	(STARTUP_SLEEP_WAIT*5)  /* maximum amount of time to wait, in miliseconds */

/*--------------------- Private Class Declarations --------------------*/

/*--------------------- Public Class Definitions --------------------*/

isabelStartup::isabelStartup(T_INIT_FUNCTION initFunc)
{
	server_init = initFunc;
}

void isabelStartup::watchForStartup(void)
{
	QtConcurrent::run(this, &isabelStartup::doWait);
}
		
void isabelStartup::doWait(void)
{
	/* wait at most 10 seconds for the application to start */
	int timeout = STARTUP_MAX_WAIT;
		
	qDebug() << "[isabel] waiting for the application to startup";

	/* wait for the target application to begin */
	while(QCoreApplication::startingUp() && (timeout > 0))
	{
		QThread::msleep(STARTUP_SLEEP_WAIT);
		timeout -= STARTUP_SLEEP_WAIT;
	}
	
	qDebug() << "[isabel] launching server";

	/* run the initialization function as soon as the application 
	   as started the main loop
	 */
	QMetaObject::invokeMethod(this,"runInitFunc",Qt::QueuedConnection);
}

Q_INVOKABLE void isabelStartup::runInitFunc(void)
{
	/* initialize the server */
	server_init();

	/* inform that the initialization is complete */
	emit startupComplete();
}
