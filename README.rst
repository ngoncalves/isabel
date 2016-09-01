Isabel
=======
:author: Nelson Gonçalves
:Website: www.patois.eu/isabel.html

Summary
-------

Isabel is a test automation tool for Qt based applications running in POSIX systems
with X11 windows server. Or to put it in other words, most Linux distros.

It is licensed as GPL v3, except for the source of the sample Qt applications that
are used for testing. These are originally from the open-source version of Qt5.4,
and are licensed under BSD. *Note that this does not imply in any way that Qt endorses
the Isabel framework*.

Isabel uses also QtJson_, by Eeli Reilin, for the serialization of QVariant objects.

The source code is available here_, from Github.

Status
------

Currently, all of the plannned functionality is implemented in Isabel, but it has not been
tested thouroughly. The client API might also change in the near future.

How To Use
----------

Isabel is a client-server tool, where the server is a shared library that is injected
on the application under test. Through the server, the client can:

	* list all of the application QObjects and their properties
	* modify the properties of QObjects
	* take screenshots of the whole screen
	* record and replay mouse and keyboard events 
	* simulate mouse and keyboard events

Isabel comes with a python client, see the `docs` folder for a small tutorial on how
to use the framework.

Known Issues
------------

The screenshot functionality does not work on AMD cards that use the closed source 
fglrx driver. When taking a screenshot, all you get is a black picture.

Although it can record all key presses made by the user, on Qt5.4 it cannot replay
modifier keys (Shift, Ctrl, etc). The reason for this is that Qt5 changed to Xcb, 
over from the X11 client, and this bug is still not fixed.

Serialization of all QVariant objects is not fully working, this is due to a limitation
of QtJson. To be fair, the author strongly recomends not to use QtJson on Qt5 since 
it comes with JSON serialization. However QtJson is used in Isabel because there is
not simple alternative for Qt4 based applications.

Isabel does not work with statically built Qt applications. This is a limitation due
to using LD_PRELOAD for injecting the Isabel code into the applications.

Building Pre-Requisites 
------------------------

In order to build the Isabel server, the following software is required:

 * Qt development enviroment
 * libx11, libXtst and libXext for interfacing the X11 windows server
 * libcairo, for generating screenshots in PNG format
 * libprotobuf_, Google's protobuf library for inter-process communication
 * python-tk, for the inspector GUI
 * make, gcc and friends

The Python client has the following dependencies:

 * python v2.7 or greater
 * tinydb_, a lightweight and simple Python document database
 * opencv_ for python, a library for image manipulation

Most of these software packages are readily available from your distro repositories. The
bravest can of course use the Internet's and compile from scratch, everybody else can just
use apt-get. To install the server dependencies:

  $ sudo apt-get install libprotoc-dev protobuf-compiler libx11-dev libxtst-dev libcairo2-dev

The client dependencies can be installed as follows:
	
  $ sudo apt-get install pip python-opencv python-tk	
  $ sudo pip install tinydb protobuf

For building the tests, the following is necessary:

 * Qt development enviroment
 * python
 * behave_ for the beahvior driven tests

Building Instructions
---------------------

In order to build Isabel:

	1. checkout the code from the repository
	2. edit in the file `build.sh`, the Qt root directory 
	3. run: `./build.sh isabel`
	4. if the build is successfull, the binaries are in isabel/bin

The tests are build and executed apart from the Isabel framework. Use `./build.sh tests`
to build and then run the tests.

Inspector GUI
-------------

This is a very simple GUI that allows to visualize the application objects tree and their
properties, as seen by the client. It also provides an handy shortcut for taking screenshots
of the application under test. To use the inspector simply go to the folder src/client
and run the script with the server location:

	$ cd src/client
	$ ./inspector.py localhost 4242

The GUI is written in Tkinter, which might be seem paradoxical given that Isabel is aimed for
Qt testing. The reason is simple: I am lazy. The inspector code is less than 300 lines, including
comments and blank lines, and it runs out of the box on most platforms. Try doing that with
Qt.

Contributing
------------

Contributions are welcome, in particular those related to:

 - bug fixing, adding and extending the test cases
 - extending the Python client
 - extending Isabel to run on other platforms (Windows, Android, etc..)
 - function hooking 
 - documentation and tutorials

However please note that all contributions must be accompanied by their respective test
code. The exceptions, of course, are the documentation and tutorials.

.. _QtInspector: https://github.com/robertknight/Qt-Inspector
.. _QtJson: https://github.com/gaudecker/qt-json
.. _tinydb: https://pypi.python.org/pypi/tinydb
.. _opencv: http://opencv.org/
.. _behave: http://pythonhosted.org/behave/
.. _libprotobuf: https://github.com/google/protobuf
.. _here: https://github.com/ngoncalves/isabel