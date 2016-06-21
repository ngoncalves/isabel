Isabel
=======
:Author: Nelson Gonçalves
:Email: nelsongoncalves@patois.eu
:Website: https://github.com/ngoncalves

Summary
-------

Isabel is a test automation tool for Qt based applications running in POSIX systems
with X11 windows server. Or to put it in other words, most Linux distros although
Windows support is planned.

Isabel is licensed as GPL v3, see the file LICENSE.txt for details. The exception 
is the source code for the sample Qt applications in the sub-folders `src/test/example_qt`
and `src/test/example_quick`. These are originally from the open-source version of Qt5.4,
and are licensed under BSD.

Isabel was initially forked from QtInspector_ by Robert Knight and uses QtJson_, by
Eeli Reilin, for the serialization of QVariant objects.

The example Qt applications were taken verbatim from the Qt Creator set of examples.
*Note that this does not imply in any way that Qt endorses the Isabel framework*.

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

Building Pre-Requisites 
------------------------

In order to build the Isabel server, the following software is required:

 * Qt development enviroment
 * libx11, libXtst and libXext for interfacing the X11 windows server
 * libcairo, for generating screenshots in PNG format
 * libprotobuf_, Google's protobuf library for inter-process communication
 * make, gcc and friends

The Python client has the following dependencies:

 * python v2.7 or greater
 * tinydb_, a lightweight and simple Python document database
 * opencv_ for python, a library for image manipulation

Most of these software packages are readily available from your distro repositories. The bravest can
of course use the Internet's and compile from scratch, everybody else can just use apt-get. To install
the server dependencies:

  $ sudo apt-get install libprotoc-dev protobuf-compiler libx11-dev libxtst-dev libcairo2-dev

The client dependencies can be installed as follows:
	
  $ sudo apt-get install pip python-opencv	
  $ sudo pip install tinydb protobuf

For building the tests, the following is necessary:

 * Qt development enviroment
 * python
 * behave_ for the beahvior driven tests

Building Instructions
---------------------

In order to build Isabel:

	1. checkout the code from the repository
	2. edit the file isabel/src/Makefile, with the location of qmake on your system
	3. go to the root directory and type: make -C src/
	4. if the build is successfull, the binaries are in isabel/bin

The tests are build and executed apart from the Isabel framework. Use `make tests`
to build and then run the tests.

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