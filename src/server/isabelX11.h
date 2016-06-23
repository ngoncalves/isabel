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

   This module interfaces the X11 server, providing the functionality to:
   	- capture user mouse and keyboard events
   	- replay user events
 */
#ifndef __ISABEL_X11_H__
#define __ISABEL_X11_H__

#include <QObject>
#include <QTimer>

#include <string>
#include <vector>

#include "protocol.pb.h"

/*--------------------- Public Variable Declarations ----------------*/

#define X11_KEYS_SIZE (32)

typedef struct{
	int32_t   xpos; 				// mouse x position 
	int32_t   ypos;					// mouse y position
	uint8_t   buttons; 				// mouse pressed buttons (bitmap)
	uint32_t  modifiers; 			// the keyboard modifiers: shift, alt, ctrl, etc
	char    keys[X11_KEYS_SIZE];	// keyboard state (pressed keys)
} T_X11_STATE; 

/*--------------------- Public Class Declarations -------------------*/

class isabelX11 : public QObject {

	Q_OBJECT

public:

	/* Class initialization.
	*/
	isabelX11(QObject *parent);

	/* Class destructor.
	*/
	~isabelX11();

	/* Begin recording the user events.
	 */
	void start_recording(void); 

	/* Stop recording the user events.

		#returns the list of recorded events
	 */
	std::vector<UserEvent *> &stop_recording(void); 

	/* Simulate user an user

		@user the user event to simulate

		#returns true if successfull, false otheriwse
	 */
	bool simulate_user(const UserEvent &event);

public slots:
	/* Record the user events.
	 */
	void record_user(void);

private:
	/* Return the current mouse state and the keybaord modifiers

		@state 	 on return, contains the mouse state and keyboard modifiers
	*/
	void get_x11_state(T_X11_STATE *state); 

private:
	QTimer 					 *timer; 					/* sets the rate at which user events are captured */
	std::vector<UserEvent *> events; 					/* list of recorded user events */
	unsigned int             instant;					/* count the elapsed time, when recording events */
	T_X11_STATE			 	 *last_state; 				/* last state of X11 mouse and keyboard */
	void 			 	 	 *display;					/* X11 client */
}; 

#endif
