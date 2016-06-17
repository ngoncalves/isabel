Isabel Tutorial
===============
:Author: Nelson Gonçalves
:Email: nelsongoncalves@patois.eu
:Website: https://github.com/ngoncalves


Preliminary
-----------

Isabel used the LDD_PRELOAD technique to inject a shared library, the server,
from which it then provides access to the Application Under Test (AUT). So,
first launch the AUT as follows:

``$ LDD_PRELOAD=<path to libisabel.so>/libisabel.so aut arg1 arg2``

In the folder `bin`, a Bash script provides a wrapper for this command.

The library is very lightweight and the server is minimally invasive. That being
stated, of course that there is a performance penalty for using Isabel. So far
I have not noticed any visible slowdown of the test applications, but your mileage
may vary.

Client
------

After launching the sever, you can start the client. The communication to the server
is through a TCP socket, so the client can run in a different machine.

The client code is divided in two python modules: 

	* `client.py`, which implements the communication protocol to the server
	* `isabel.py`, which provides a higher abstraction for interacting with the server

The latter provides three main classes:
	
	* User: for simulating and recording user keyboard and mouse events
	* Application: for reading and modifying the AUT QObjects
	* Screen: for taking screenshots and locating sample images in them

Record And Replay User
----------------------

This example shows how to record the user interaction with the application and 
then replay what was recorded::

	import isabel

	# create the interface object and connect to the server, assuming
	# it is running on the same host. The port is by default, 4242
	user = isabel.User()
	user.connect('localhost',4242)

	# record the user for 20 seconds. All keyboard and mouse events are recorded,
	# even those not meant for the AUT
	user.start_record()
	import time; time.sleep(20)

	# stop recording and save the events locally in a CSV file
	user.record_stop('./recorded_events.csv')

	# now replay the user events
	user.record

Screenshots
-----------

This example shows how to take a screenshot of the whole screen, locate a given
image (for instance of an icon) and click it::

	import isabel

	# create the interface object and connect to the server, assuming
	# it is running on the same host. The port is by default, 4242
	screen = isabel.Screen()
	screen.connect('localhost',4242)

	# take a screenshot and save it locally in PNG format 
	screen.take_screenshot('./my_screenshot.png')

	# locate an icon on it
	pos = screen.find('./icons/new.png','./my_screenshot.png')

	# move the mouse to the icon and click it. Need the User class
	user = isabel.User()
	user.connect('localhost',4242)

	user.mouse_move(pos[0],pos[1])
	user.mouse_click()

Accessing QObjects
------------------

This example shows how to access QObjects and modify their properties. It
assumes that the application has a QWindown object::

	import isabel

	# create the interface object and connect to the server, assuming
	# it is running on the same host. The port is by default, 4242
	app = isabel.Application()
	app.connect('localhost',4242)

	# get the object tree
	app.refresh()

	# for all objects that have a 'title' property, change it to the 
	# same value. The object IDs begin at 1
	for obj in range(1,len(app.db.all())):
		# get the object current properties
		if not app.refresh_object(obj):
			print 'Failed to refresh object:',obj
			break
		else:
			if 'title' in app.get_object(obj)['properties'].keys():
				# might not work if the property is not writtable
				# not the double quotes. Property values are sent/received
				# as strings and since the value itself is a string, hence
				# the double quoted string
				app.modify(obj,'title','"isabel"')