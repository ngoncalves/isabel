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

   This Python module provides a client interface to the server that is
   running on the library that was injected on the application under test.
"""

import protocol_pb2
import socket 
import logging
import struct
import random
import time

class SLIP():
	"""
	Implementation of the SLIP protocol encoding and decoding rules.
	"""

	def __init__(self):
		"""
		Class properties initialization
		"""
		# SLIP constants, do not modify
		self.SLIP_END 	  = 0xC0
		self.SLIP_ESC 	  = 0xDB
		self.SLIP_ESC_END = 0xDC
		self.SLIP_ESC_ESC = 0xDD

	def encode(self,packet):
		"""
		SLIP encode the given packet.

		@packet the data to encode, as a bytearray

		#returns the data encoded as SLIP
		"""
		slip = bytearray()

		slip.append(self.SLIP_END)

		for p in packet:
			if self.SLIP_END == p:
				slip.append(self.SLIP_ESC)
				slip.append(self.SLIP_ESC_END)
			elif self.SLIP_ESC == p:
				slip.append(self.SLIP_ESC)
				slip.append(self.SLIP_ESC_ESC)
			else:
				slip.append(p)

		slip.append(self.SLIP_END)

		return slip

	def decode(self,packet):
		"""
		Decode the given packet, using the SLIP protocol.

		@packet  the data to decode, as a bytearray

		#returns bytearray with the decoded data
		"""
		slip = bytearray()

		if len(packet) == 0:
			return slip

		# find the begining of the packet 
		i = 0
		while i < len(packet) and self.SLIP_END != packet[i]:
			i += 1

		if i < len(packet) and self.SLIP_END == packet[i]:
			# skip the first delimiter
			i += 1
			while i < len(packet) and self.SLIP_END != packet[i]:
				if self.SLIP_ESC == packet[i]:
					i += 1
					if self.SLIP_ESC_END == packet[i]:
						slip.append(self.SLIP_END)
					elif self.SLIP_ESC_ESC == packet[i]:
						slip.append(self.SLIP_ESC)
					else:
						# invalid SLIP packet, return an empty bytearray
						logging.error('[SLIP] invalid encoding')
						slip = bytearray()
						break
				else:
					slip.append(packet[i])

				i += 1

		return slip

class Client():
	"""
	Implementation of the client to the Isabel server
	"""

	def __init__(self):
		"""
		Definition of the class properties
		"""
		self.sock = None
		self.slip = SLIP()
	
	def connect(self,host,port=4242,timeout=0.5):
		"""
		Connect to the Isabel server on the given address.

		@host   	address where the server is running
		@port   	port number where the server is listening
		@timeout 	how long to wait for a reply from the server, in seconds

		#returns True if successfull, False otherwise
		"""
		if self.sock:
			logging.warning('[Client] already connected, disconnecting first')
			self.disconnect()

		try:
			logging.info('[Client] connecting to server at (%s,%d)' %(host,port))
			self.sock = socket.create_connection((host,port))
			self.sock.settimeout(timeout)
			logging.info('[Client] connected to server')
			return True
		except socket.error as e:
			logging.error('[Client] failed to connected: %s' % str(e))
			if self.sock:
				self.sock.close()				
			self.sock = None
			return False 

	def disconnect(self):
		"""
		Disconnect from the server.

		#returns True if successfull, False otherwise
		"""
		if self.sock:
			logging.info('[Client] disconnecting from the server')
			try:
				self.sock.shutdown(socket.SHUT_RDWR)
				self.sock.close()
			except socket.error:
				# ignore since we are closing the socket
				pass
				
			self.sock = None
			return True
		else:
			logging.warn('[Client] not connected to the server')

	def send(self,req):
		"""
		Send the request to the server and wait for the reply.

		@req 	protobuf Request object with the request

		#returns reply from the server, as a protobuf Response, or None in case of error
		"""
		logging.info('[Client] sending request to the server')
		if not self.sock:
			logging.warn('[Client] not connected !')
			return None

		try:
			# encode the request using SLIP, then send it
			message = self.slip.encode(bytearray(req.SerializeToString()))
			self.sock.send(message)

			# wait until all data is received (a timeout occurs)
			logging.info('[Client] wait for the server reply')
			reply = bytearray()
			try:
				while True:
					reply.extend(bytearray(self.sock.recv(4096)))
					if 0 == len(reply):
						logging.error('[Client] nothing received from the server')
						return None
					elif self.slip.SLIP_END == reply[-1]:
						# got the full packet, no need to wait any longer 
						break

			except socket.timeout:
				# ignore the timeout
				pass

			if len(reply) <= 2:
				# invalid or incomplete response, ignore it
				logging.error('[Client] response is too small')
				return None 
			else:
				# parse and return the response, using the protobuf encoding
				reply = self.slip.decode(reply)
				if 0 < len(reply):
					response = protocol_pb2.Response()
					response.ParseFromString(str(reply))
					return response
				else:
					logging.error('[Client] SLIP decoded response is too small')
					return None 	
		except socket.error as e:
			logging.error('[Client] failed to communicate with the server: %s' % str(e))
			return None

	def fetch_object_tree(self):
		"""
		Request the server to send the application current list of Qt objects.

		#returns the list of objects, empty in case of error
		"""
		request = protocol_pb2.Request()
		request.type = protocol_pb2.Request.FETCH_OBJECT_TREE 
		response = self.send(request)
		if not response or response.error != protocol_pb2.Response.NO_ERROR:
			logging.error('[Client] failed to retrieve the object tree')
			return []
		else:
			return response.objects

	def fetch_object(self,obj):
		"""
		Request the server to send all of the properties from a Qt object.

		@obj  the identifier of the object

		#returns list of object properties, empty in case of error
		"""
		request = protocol_pb2.Request()
		request.type = protocol_pb2.Request.FETCH_OBJECT 
		request.id = obj
		response = self.send(request)
		if not response or response.error != protocol_pb2.Response.NO_ERROR:
			logging.error('[Client] failed to retrieve the object properties')
			return []
		else:
			return response.properties

	def set_object_property(self,obj,name,value):
		"""
		Modify the specified property on the given object.

		@obj  	the identifier of the object
		@name 	name of the property to modify
		@value 	new value, as a bytearray, of the property

		#returns True if successfull, False otherwise
		"""
		request = protocol_pb2.Request()
		request.type = protocol_pb2.Request.WRITE_PROPERTY 
		request.id = obj

		request.property.name  	  = name
		request.property.value 	  = value
		request.property.writable = True

		response = self.send(request)
		if not response or response.error != protocol_pb2.Response.NO_ERROR:
			logging.error('[Client] failed to set the object property')
			return False
		else:
			return True

	def start_recording_user(self):
		"""
		Start to record all of the user mouse and keyboard events

		#returns True if successfull, False otherwise
		"""
		request = protocol_pb2.Request()
		request.type = protocol_pb2.Request.RECORD_USER 
		request.start = True
		response = self.send(request)
		if not response or response.error != protocol_pb2.Response.NO_ERROR:
			logging.error('[Client] failed to record the user events')
			return False
		else:
			return True

	def stop_recording_user(self):
		"""
		Stop recording the user mouse and keyboard events.

		#returns the list of events recorded so far, None in case of error
		"""
		request = protocol_pb2.Request()
		request.type = protocol_pb2.Request.RECORD_USER 
		request.start = False
		response = self.send(request)
		if not response or response.error != protocol_pb2.Response.NO_ERROR:
			logging.error('[Client] failed to record the user events')
			return None
		else:
			result = []
			for event in response.events: 
				result.append(event)

			return result

	def simulate_keyboard(self,key,press):
		"""
		Simulate a key press or release

		@key   the key to press/release
		@press the state of the key
		
		#returns True if successfull, False otherwise
		"""
		request      = protocol_pb2.Request()
		request.type = protocol_pb2.Request.SIMULATE_USER
		
		request.user.type  = protocol_pb2.UserEvent.KEYBOARD
		request.user.press = press 
		request.user.key   = key
		
		response = self.send(request)
		if not response or response.error != protocol_pb2.Response.NO_ERROR:
			logging.error('[Client] failed to simulate a keyboard event')
			return False
		else:
			return True

	def simulate_mouse_move(self,coords,relative=False):
		"""
		Simulate a mouse movement.

		@coords   the x and y position of the mouse
		@relative if False, coords are an absolute position, otherwise a relative displacement

		#returns True if successfull, False otherwise
		"""
		request      = protocol_pb2.Request()
		request.type = protocol_pb2.Request.SIMULATE_USER
		
		if relative:
			request.user.type = protocol_pb2.UserEvent.MOUSE_MOVE_REL
		else:
			request.user.type = protocol_pb2.UserEvent.MOUSE_MOVE_ABS

		request.user.xpos = coords[0]
		request.user.ypos = coords[1]
		
		response = self.send(request)
		if not response or response.error != protocol_pb2.Response.NO_ERROR:
			logging.error('[Client] failed to simulate a mouse move event')
			return False
		else:
			return True

	def simulate_mouse_button(self,button,press):
		"""
		Simulate a mouse button press or release

		@button   the mouse button to press/release
		@press    the state of the button

		#returns True if successfull, False otherwise
		"""
		request      = protocol_pb2.Request()
		request.type = protocol_pb2.Request.SIMULATE_USER
		
		request.user.type   = protocol_pb2.UserEvent.MOUSE_BUTTON
		request.user.press  = press 
		request.user.button = button

		response = self.send(request)
		if not response or response.error != protocol_pb2.Response.NO_ERROR:
			logging.error('[Client] failed to simulate a mouse button event')
			return False
		else:
			return True

	def take_screenshot(self,name,win_id=0):
		"""
		Take a screenshot and save it to the file with the given name.

		@name    file name where to save the screenshot
		@win_id  identifier of the window from where to take the screenshot

		#returns True if successfull, False otherwise

		The file is saved locally, on the client side, in PNG format.
		If window ID is zero, then the screenshot is taken on the whole screen. Otherwise
		it is taken from the specified window ID (which is X11 specific)
		"""
		logging.info('[Client] taking a screenshot of the whole screen')

		request      = protocol_pb2.Request()
		request.type = protocol_pb2.Request.TAKE_SCREENSHOT
		request.id   = win_id

		response = self.send(request)
		if not response or response.error != protocol_pb2.Response.NO_ERROR:
			logging.error('[Client] failed to take screenshot')
			return False 

		# got the screenshot, save it to a file
		with open(name,'wb') as image:
			image.write(response.image)

		return True

	def kill_app(self):
		"""
		Force Application Under Test to close.

		#returns True if successfull, False otherwise
		"""
		logging.info('[Client] killing the remote application')

		request = protocol_pb2.Request()
		request.type = protocol_pb2.Request.KILL_APP
		response = self.send(request)
		if not response or response.error != protocol_pb2.Response.NO_ERROR:
			logging.error('[Client] failed to kill the remote application')
			return False 
		else:
			return True
		

