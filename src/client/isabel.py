# -*- coding: iso-8859-15 -*-
"""
   Isabel: Qt Test Automation
   ==========================
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

   This Python module provides an abstraction for interacting with the 
   Application Under Test (AUT) using the Isabel client.
"""

import client
import tinydb
import time
import cv2
import protocol_pb2

class Tester():
	"""
	Base class for the Isabel framework. It wraps the client functionality
	"""

	def __init__(self):
		"""
		Class properties initialization
		"""
		self.client = client.Client()

	def connect(self,host,port):
		"""
		Connect to the server at the given location.

		@host the IPv4 address of the server 
		@port the listening port of the server

		#returns True if successfull, False otherwise
		"""
		return self.client.connect(host,port)

	def disconnect(self):
		"""
		Disconnect from the server.

		#returns True if successfull, False otherwise
		"""
		return self.client.disconnect()

class User(Tester):
	"""
	This class provides functionality to simulate a user interacting
	with the application. 
	"""

	def mouse_move(self,xpos,ypos):
		"""
		Move the mouse to the specified screen coordinates.

		@xpos  the X mouse screen position
		@ypos  the Y mouse screen position

		#returns True if successfull, False otherwise

		This function simulates an absolute mouse movement
		"""
		return self.client.simulate_mouse_move([xpos,ypos])

	def mouse_click(self,button,delay=0.1):
		"""
		Click the specified mouse button, at the current mouse position.

		@button  the number of the button to click (1 = left, 2 = midle, 3 = right)
		@delay   time interval, in seconds, between pressing and relasing the button 

		#returns True if successfull, False otherwise
		"""
		# press the mouse button
		start = time.time()
		if not self.client.simulate_mouse_button(button,True):
			return False

		# release the mouse button
		now = time.time()
		if now - start < delay:
			time.sleep(delay - (now - start))

		return self.client.simulate_mouse_button(button,False)

	def keyboard_symbol(self,symbol,delay=0.1):
		"""
		Generate a key press, followed of a release, to generate the given 
		symbol.

		@symbol  the keyboard symbol to generate, as string of length 1
		@delay   time interval, in seconds, between pressing and relasing the button 

		#returns True if successfull, False otherwise

		Note that the conversion of the symbol to a keyboard key ID is done
		by the X11 running at the server side.
		"""
		# press the keyboard key thar corresponds to the symbol
		start = time.time()
		if not self.client.simulate_keyboard(symbol,True):
			return False

		# release the key
		now = time.time()
		if now - start < delay:
			time.sleep(delay - (now - start))

		return self.client.simulate_keyboard(symbol,False)

	def start_record(self,delay=1.0):
		"""
		This function requests the server to begin recording the user.

		@delay  how long to wait before begining to record the user 

		#returns True if successfull, False otherwise
		"""
		time.sleep(delay)
		return self.client.start_recording_user()
		
	def stop_record(self,output):
		"""
		This function requests the server to stop recording the user and 
		then saves all recorded events onto a CSV formated file

		@output  the name of the file where the recorded events are stored, in CSV format

		#returns True if successfull, False otherwise
		"""
		events = self.client.stop_recording_user()
		if None == events:
			# an error occurred 
			return False
		else:
			# save the events to file, in CSV format:
			# - for keyboard events     : time,type,key name, pressed
			# - for mouse button events : time,type,button ID, pressed
			# - for mouse move events   : time,type,xpos, ypos
			with open(output,'w') as csv:
				for event in events:
					line = str(event.instant) + ',' + str(event.type) + ','
					if protocol_pb2.UserEvent.KEYBOARD == event.type:
						line += event.key + ',' + str(event.press)
					elif protocol_pb2.UserEvent.MOUSE_BUTTON == event.type:
						line += str(event.button) + ',' + str(event.press)
					else:
						line += str(event.xpos) + ',' + str(event.ypos)
					
					line += '\n'
					csv.write(line)

			return True

	def str2bool(self,value):
		"""
		Convert the string in value to a bolean value.

		@value  string to convert

		#returns True if the string is 'True', False for any other value
		"""
		if 'True' == value:
			return True
		else:
			return False

	def replay(self,events):
		"""
		Read the user events from a CSV formated file and replay them on
		the server side.

		@events  the CSV file with the events to replay

		#returns True if successfull, False otherwise
		"""
		# load the events from file and replay them as they are being read
		with open(events,'r') as csv:
			line    = csv.readline().strip()
			last_t0 = 0
			while len(line) > 0:
				fields = line.split(',')
				ok     = False
				start  = time.time()

				if len(fields) == 4:
					t0 = int(fields[0])
					ev_type = int(fields[1])
					
					if protocol_pb2.UserEvent.KEYBOARD == ev_type:
						press = self.str2bool(fields[3])
						key   = fields[2]
						ok    = self.client.simulate_keyboard(key,press)
					elif protocol_pb2.UserEvent.MOUSE_BUTTON == ev_type:
						button = int(fields[2])
						press  = self.str2bool(fields[3])
						ok     = self.client.simulate_mouse_button(button,press)
					elif ev_type in [protocol_pb2.UserEvent.MOUSE_MOVE_REL,protocol_pb2.UserEvent.MOUSE_MOVE_ABS]:
						xpos = int(fields[2])
						ypos = int(fields[3])
						ok   = self.client.simulate_mouse_move([xpos,ypos],ev_type == protocol_pb2.UserEvent.MOUSE_MOVE_REL) 

				if not ok:
					return False 

				# wait for the next event, taking into account the elapsed time in
				# communicating with the server
				end = time.time()
				if (end - start) < (t0 - last_t0)*0.001:
					time.sleep((t0 - last_t0)*0.001 - (end - start))
				
				last_t0 = t0

				# read the next event
				line = csv.readline().strip()

		return True		

class Screen(Tester):
	"""
	This class provides the functionality to take screenshots and 
	to further process the resulting images.
	"""
		
	def take_screenshot(self,file_name,winid=0):
		"""
		Request the server to take a screenshot of the entire screen.

		@file_name  name of the image file where the screenshot is save, in PNG format
		@winid      the X11 window on which to take the screenshot

		#returns True if successfull, False otherwise

		If win_id is zero, then the screenshot includes all of the screen
		"""
		return self.client.take_screenshot(file_name,win_id=winid)

	def find(self,needle,screen):
		"""
		Locate the object image in a screenshot.

		@needle  the file containing the image to search for, in the screenshot 
		@screen  the image file containing a screenshot 

		#returns if found, a list with the center (x,y) coordinates of the needle
		         in the screen, otherwise an empty list
		"""
		# read the two images
		needle_img = cv2.imread(needle)
		screen_img = cv2.imread(screen)

		# find the needle in the screen
		result = cv2.matchTemplate(needle_img,screen_img,cv2.cv.CV_TM_SQDIFF_NORMED) 
		mn,_,location,_ = cv2.minMaxLoc(result)

		return location

class Application(Tester):
	"""
	This class provides the functionalities to list the objects in
	the Application Under Test and to modify their properties. It is
	also possible to remotely kill the application.

	The class maintains an internal database with the state of the 
	application, aka the Qt objects that exist at a given moment in time.
	The database is stored on disk, and purged when an object is created.
	"""

	def __init__(self,db_file='./objects.json'):
		"""
		Class initialization.

		@db_file  name of the file containing the GUI Qt objects

		The database file is opened, and then purged. If the file does not
		exist, then it is created.
		"""
		Tester.__init__(self)
		self.db     = tinydb.TinyDB(db_file)  # the Qt objects database
		self.db.purge()

	def kill(self):
		"""
		Kill the remote application.

		#returns True if successfull, False otherwise
		"""
		return self.client.kill_app()

	def refresh(self):
		"""
		Refresh the internal database with the current Qt objects.

		#returns True if successfull, False otherwise
		"""
		# clean the objects database, them request the server for the 
		# current list of ojects
		self.db.purge()
		objects = self.client.fetch_object_tree()
		if not objects:
			# did not return any objects	
			return False  
		else:
			# insert the objects in the database 
			for obj in objects:
				self.db.insert({ 'id'         : obj.id,		  # the object unique identifier number 
								 'type'       : obj.type,	  # the object number 
								 'parent'     : obj.parent,   # the id of the object parent, 0 if its at the root
								 'address'    : obj.address,  # the memory location of the object
								 'name'       : obj.name,     # if available, the object name
								 'properties' : dict()		  # map for the object properties	
					           })

			# all done
			return True

	def refresh_object(self,obj):
		"""
		Request the properties for the object with the given identifier.

		@obj  the object unique identifier

		#returns True if successfull, False otherwise 

		This refreshes the properties of the object in the database 
		"""
		properties = self.client.fetch_object(obj)

		if not properties:	
			# either the object ID is invalid, or failed to retrieve the properties
			return False 
		else:
			# convert the object properties to a dictionary
			obj_properties = dict()
			for prop in properties:
				obj_properties[prop.name] = { 'writable' : prop.writable,
 											  'value'    : prop.value
											}

			# now update the object properties
			Obj = tinydb.Query()
			self.db.update({'properties' : obj_properties}, Obj.id == obj)

			# all done 
			return True

	def modify(self,obj,prop,value):
		"""
		Modify the property of the specified object.

		@obj        object identifier
		@prop   	name of the property to modify
		@value 		the property new value

		#returns True if successfull, False otherwise
		"""
		if not self.client.set_object_property(obj,prop,value):
			# failed to update the object properties
			return False
		else:
			# succeeded in changing the property, update the local db 
			query = tinydb.Query()
			obj = self.db.search(query.id == obj)
			obj[0]['properties'][prop] = value
			self.db.update(obj[0],query.id == obj)

			# all done 
			return True

	def get_object(self,obj):
		"""
		Return the object as it is currently on the database

		@obj  the object identifier

		#returns dictionary with the object properties, None if not found  
		"""
		query = tinydb.Query()
		obj = self.db.search(query.id == obj)
		if obj and len(obj) == 1:
			return obj[0]
		else:
			return None 
