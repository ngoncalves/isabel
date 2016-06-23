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
	See the respective header file for details.

 */
#include "isabelX11.h"

#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>

#include <unistd.h>
#include <cstring>

extern "C"
{
	#include <X11/Xlib.h>
	#include <X11/XKBlib.h>
	#include <X11/extensions/XTest.h>
}

#include <cassert>
#include <cstdio>

/*--------------------- Private Data Declarations ----------------*/

#define USER_SAMPLE_TIME (10)	/* sampling at 100 Hz */

/*--------------------- Private Function Declarations ----------------*/

/*--------------------- Public Class Declarations -------------------*/

isabelX11::isabelX11(QObject *parent)
: QObject(parent)
{
	timer  = new QTimer(this); 

	connect(timer,SIGNAL(timeout()),this,SLOT(record_user())); 

	display    = (void *)XOpenDisplay(NULL);
	last_state = new T_X11_STATE();

	assert(display != NULL);
	assert(last_state != NULL);
}

isabelX11::~isabelX11()
{
	delete timer; 
	events.clear();
	XCloseDisplay((Display *)display);
	delete last_state;
}

void isabelX11::start_recording(void)
{
	/* stop the timer and clear the previous recorded events */
	if(timer->isActive())
	{
		timer->stop();		
	}

	/* clear the queue and the event instant counter */
	events.clear(); 
	instant = 0; 

	/* get the current state */
	get_x11_state(last_state);

	/* and begin the recording with the mouse absolute position */
	UserEvent *event = new UserEvent(); 
	event->set_type(UserEvent::MOUSE_MOVE_ABS);
	event->set_instant(instant); 
	event->set_xpos(last_state->xpos); 
	event->set_ypos(last_state->ypos); 
	events.push_back(event);

	/* reset the keyboard state */
	memset(last_state->keys,0x00,X11_KEYS_SIZE); 

	/* start the timer */
	timer->start(USER_SAMPLE_TIME);
}

std::vector<UserEvent *> &isabelX11::stop_recording(void)
{
	timer->stop(); 

	return events;
}

void isabelX11::record_user(void)
{
	T_X11_STATE state;

	get_x11_state(&state);

	/* did the mouse moved from its last position ? */
	if((state.xpos != last_state->xpos) || (state.ypos != last_state->ypos))
	{
		/* add a mouse relative movement event */
		UserEvent *event = new UserEvent(); 
		event->set_type(UserEvent::MOUSE_MOVE_REL);
		event->set_instant(instant); 
		event->set_xpos(state.xpos - last_state->xpos); 
		event->set_ypos(state.ypos - last_state->ypos); 

		events.push_back(event);

		/* save the current mouse position */
		last_state->xpos = state.xpos; 
		last_state->ypos = state.ypos; 
	}

	if(last_state->buttons != state.buttons)
	{
		uint8_t changed = state.buttons ^ last_state->buttons; 

		for(unsigned int i = 0; i < 8; i++)
		{
			if(changed & (1 << i))
			{
				bool pressed = last_state->buttons & (1 << i) ? false : true; 

				/* add a mouse button event */
				UserEvent *event = new UserEvent(); 
				event->set_type(UserEvent::MOUSE_BUTTON);
				event->set_instant(instant);
				event->set_press(pressed); 
				event->set_button(i + 1);

				events.push_back(event);		
			}	
		}
		
		last_state->buttons = state.buttons;
	}

	/* record the changes in the keyboard keys */
	for(int k = 0; k < X11_KEYS_SIZE; k++)
	{
		/* see if the state of a key has changed */
		if(state.keys[k] != last_state->keys[k])
		{
			unsigned char difference = state.keys[k] ^ last_state->keys[k];
						
			for(int i = 0; i < 8; i++)
			{
				if((difference >> i) & 0x01)
				{
					UserEvent *event = new UserEvent(); 
					event->set_type(UserEvent::KEYBOARD);
					event->set_instant(instant); 
					
					if((state.keys[k] >> i) & 0x01)
					{
						event->set_press(true);
					}
					else
					{
						event->set_press(false); 
					}

					int shift = state.modifiers & ShiftMask ? 1 : 0;
					event->set_key(XKeysymToString(XkbKeycodeToKeysym((Display *)display,8*k + i,0,shift))); 

					events.push_back(event); 
				}	
			}
		}
					
		last_state->keys[k] = state.keys[k];
	}

	/* increment the elapsed time */
	instant += USER_SAMPLE_TIME;
}


bool isabelX11::simulate_user(const UserEvent &event)
{
	bool result = true; 
	
	switch(event.type())
	{
		case UserEvent::MOUSE_MOVE_REL:
			XWarpPointer((Display *)display,None,None,0,0,0,0,event.xpos(),event.ypos());
			XFlush((Display *)display);
			break; 

		case UserEvent::MOUSE_MOVE_ABS:
			/* first move the mouse to the origin, then to the absolute position */
			{
				T_X11_STATE state;
				get_x11_state(&state); 

				XWarpPointer((Display *)display,None,None,0,0,0,0,-state.xpos,-state.ypos);
				XWarpPointer((Display *)display,None,None,0,0,0,0,event.xpos(),event.ypos());
				XFlush((Display *)display);
			}
			break; 

		case UserEvent::MOUSE_BUTTON:
			{
				XTestFakeButtonEvent((Display *)display,event.button(),event.press(),CurrentTime);
				XFlush((Display *)display);
  			}
			break; 

		case UserEvent::KEYBOARD:
			{
  				/* take from here: http://www.doctort.org/adam/nerd-notes/x11-fake-keypress-event.html */
				/* get the window with the current focus */
				Window winFocus;
   				int    revert;
   				XGetInputFocus((Display *)display,&winFocus,&revert);

				/* build the event */
				XKeyEvent ev;
				memset(&ev,0,sizeof(ev));

   				ev.display     = (Display *)display;
   				ev.window      = winFocus;  
   				ev.root        = XDefaultRootWindow((Display *)display);
   				ev.subwindow   = None;
   				ev.time        = CurrentTime;
   				ev.x           = 1;
   				ev.y           = 1;
   				ev.x_root      = 1;
   				ev.y_root      = 1;
   				ev.same_screen = True;
   				ev.keycode     = XKeysymToKeycode((Display *)display,XStringToKeysym(event.key().c_str()));
   				ev.state       = XkbKeysymToModifiers((Display *)display,XStringToKeysym(event.key().c_str()));

   				if(event.press())
   				{
   					ev.type = KeyPress;		
   				}
   				else
   				{
   					ev.type = KeyRelease;				
   				}
      
   				XSendEvent(ev.display, ev.window, True, KeyPressMask, (XEvent *)&ev);
			} 
			break; 

		default:
			result = false;
			break; 
	}

	return result;
}

void isabelX11::get_x11_state(T_X11_STATE *state)
{
	memset(state,0x00,sizeof(T_X11_STATE));

	/* use XQueryPointer to get the current mouse state */
	XEvent event; 

	XQueryPointer((Display *)display,
				  DefaultRootWindow((Display *)display),
				  &(event.xbutton.root),
				  &(event.xbutton.window),
				  &(event.xbutton.x_root),
				  &(event.xbutton.y_root),
				  &(event.xbutton.x),
				  &(event.xbutton.y), 
				  &(event.xbutton.state));

	/* then copy only the relevant state information */
	state->xpos    = event.xbutton.x; 
	state->ypos    = event.xbutton.y; 
	state->buttons = (event.xbutton.state >> 8) & 0x00FF; 

	/* and now get the keyboard state */
	XQueryKeymap((Display *)display,state->keys);
	state->modifiers = event.xbutton.state & 0x00FF;
}


/*--------------------- Private Function Definitions ----------------*/
