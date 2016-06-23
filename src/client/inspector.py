#!/usr/bin/env python
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

   This Python module a GUI for inspecting the objects tree in the 
   application under test.

"""

import client
import protocol_pb2
import ttk
import tkMessageBox
import tkFileDialog
import sys

class Inspector(ttk.Frame):
	""" 
	Provides a simple GUI to inspect the object tree of the
	application under test
	""" 
	def __init__(self, master=None):
		"""
		Class properties initialization. This also
		creates the GUI widgets 
		"""
		ttk.Frame.__init__(self, master)
		self.grid()                       
		
		# create the GUI elements
		self.tree_label			   = ttk.Label(self,text='Objects Tree')
		self.properties_label	   = ttk.Label(self,text='Object Properties')
		self.objects_tree          = ttk.Treeview(self,columns=('id'),yscrollcommand=self.tree_scroll)
		self.property_list         = ttk.Treeview(self,columns=('Writtable','Value'),yscrollcommand=self.properties_scroll)
		self.refresh_tree_button   = ttk.Button(self,text='Refresh Tree',command=self.refresh_tree)
		self.refresh_object_button = ttk.Button(self,text='Refresh Object',command=self.refresh_object)
		self.screenshot_button     = ttk.Button(self,text='Screenshot',command=self.screenshot)
		
		# place them on the GUI
		self.tree_label.grid(column=0,row=0,)
		self.properties_label.grid(column=1,row=0)
		self.objects_tree.grid(column=0,row=1)
		self.property_list.grid(column=1,row=1)
		self.refresh_tree_button.grid(column=0,row=2)
		self.screenshot_button.grid(column=1,row=2)
		self.refresh_object_button.grid(column=2,row=2)
	
		# add event handlers
		self.objects_tree.bind('<<TreeviewSelect>>',self.object_selected)

		# create the properties for communicating with the server
		self.client = client.Client()
		self.current_selection = ''

	def destroy(self):
		"""
		This method is closed when the application is closed.
		"""
		self.client.disconnect()

	def connect(self,host,port):
		"""
		Connect to the server running at the given location.

		@host  the IPv4 address of the host
		@port  the server listening port

		#returns True if successfull, False otherwise
		""" 
		return self.client.connect(host,port)
		
	def screenshot(self):
		"""
		Ask the user for the name of the file where to save the 
		screenhot, and then requests the server to take a screenshot 
		of the whole screen.
		"""
		file_name = tkFileDialog.asksaveasfilename(parent=self,filetypes=[('Portable Network Graphics','*.png')],title='Where to save the screenshot')
		if file_name:
			if not self.client.take_screenshot(file_name):
				tkMessageBox.showerror('Error','Failed to take the screenshot')
			else:
				tkMessageBox.showinfo('Info','Screenshot saved at: ' + file_name)


	def refresh_tree(self):
		"""
		Request the server to the current objects tree. 
		"""
		objects = self.client.fetch_object_tree()
		if not objects:
			tkMessageBox.showerror('Error','Failed to retrieve the objects tree')
		else:
			# erase the current tree and fill it with the current objects
			for row in self.objects_tree.get_children():
				self.objects_tree.delete(row)

			# now show the current objects tree			
			for obj in objects:
				parent = ''
				if not obj.parent == 0:
					parent = str(obj.parent)
				self.objects_tree.insert(parent,'end',obj.id,text=obj.type,values=(str(obj.id)))

			# and then select the first object in the tree
			self.objects_tree.selection_set(1)

	def object_selected(self,event):
		"""
		Update the properties of the currently selected object in the tree.		
	
		@event  Tkinter information about the event that occurred
		"""
		focus = self.objects_tree.focus()
		if focus and not focus == self.current_selection:
			# a different object has been selected, update the object properties
			self.current_selection = focus
			self.refresh_object()

	def refresh_object(self):
		"""
		Request the server for the properties of the currently 
		selected object.
		"""
		if not self.current_selection:
			print '[inspector] no object is selected' 
		else:
			properties = self.client.fetch_object(int(self.current_selection))
			if not properties:
				tkMessageBox.showerror('Error','Failed to retrieve the object properties')
			else:
				# erase the current properties
				for row in self.property_list.get_children():
					self.property_list.delete(row)

				# add the object properties
				for prop in properties:
					self.property_list.insert(parent='',index='end',iid=None,text=prop.name,values=(str(prop.writable),prop.value))

def show_usage():
	"""
	Print how to call the script and then exit.
	"""
	print 'Usage:',sys.argv[0],'host port'
	print '  where:'
	print '    host  : the IPv4 address of the server'
	print '    port  : the server listening address'
	sys.exit(1)

if __name__ == '__main__':
	if len(sys.argv) != 3:
		show_usage()
	else:
		inspector = Inspector()
		inspector.master.title('Qt Inspector')
		if inspector.connect(sys.argv[1],int(sys.argv[2])):
			inspector.mainloop()
			sys.exit(0)
		else:
			print '[inspector] failed to connect to server' 
			sys.exit(1)
