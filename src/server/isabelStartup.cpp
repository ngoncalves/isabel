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
#include <QApplication>

#include <cstdio>
#include <ctime>

/*--------------------- Private Variable Declarations ----------------*/

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
	int timeout = 3000;		/* waiting timeout: 30 seconds */

	fprintf(stderr,"[isabel] waiting for the application to startup\n");

	/* wait for the target application to begin */
	while(QApplication::startingUp() && timeout > 0)
	{
		/* snooze */
		QThread::msleep(10);
		timeout -= 10;
	}
	
	if(timeout <= 0)
	{
		fprintf(stderr,"[isabel] timeout while waiting for the application to startup\n");
	}
	else
	{
		/* run the initialization function as soon as the application 
		   as started the main loop
		 */
		QMetaObject::invokeMethod(this,"runInitFunc",Qt::QueuedConnection);
	}
}

Q_INVOKABLE void isabelStartup::runInitFunc(void)
{
	/* initialize the server */
	server_init();

	/* inform that the initialization is complete */
	emit startupComplete();
}
