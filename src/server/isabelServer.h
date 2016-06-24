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

   The TCP server that handles the requests from clients. Note that
   only one client can connect at any given instant. This is because
   there is no reason for multiple clients to concurrently modify and
   inspect the application under test.

 */
#ifndef __ISABEL_SERVER_H__
#define __ISABEL_SERVER_H__

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include <string>
#include <map>

#include "protocol.pb.h"
#include "isabelX11.h"

/*--------------------- Public Variable Declarations ----------------*/

/*--------------------- Public Class Declarations -------------------*/

class isabelServer : public QObject {

	Q_OBJECT

public:

	/* Class initialization.

		This launches the TCP server and prepares all of the signals
		for handling the client connections.

		@port	the TCP port where to listen
		@parent the parent QObject
	*/
	isabelServer(int port, QObject *parent);

	/* Class destructor.

		Stops the TCP server.
	*/
	~isabelServer();

public slots:
	/* Handle a new client connection.
	 */
	void new_connection(void);

	/* Handle a request sent by the client.
	*/
	void ready_read(void);

	/* Delete the socket when the client has disconnected
	*/
	void disconnected(void);	

private:
	/* Return the complete list of object in the application.

		@response  protobuff where the response is returned
	*/
	void fetch_object_tree(Response &response);

	/* Return all of the given object properties.

		@response  protobuff where the response is returned
		@id  	   object identifier
	*/
	void fetch_object(Response &response, unsigned int id);

	/* Modify, or add, an object property.

		@response  	protobuff where the response is returned
		@request   protobuff with the request
	*/
	void write_object_property(Response &response, unsigned int id, const Property &property);

	/* Begin, or stop, the recording of the user input events.

		@response  protobuff where the response is returned
		@start 	   if true, begin recording the user, otherwise end the recording
		
		In case the function stops the recording, the response contains
		the list of events recorded until that instant.
	*/
	void record_user(Response &response, bool start);

	/* Simulate the user interaction.

		@response  protobuff where the response is returned
		@request   protobuff with the request
	*/
	void simulate_user(Response &response, const Request &request);

	/* Take a shot of the whole screen.

		@response  protobuff where the response is returned
		@win_id    the X11 window identifier, use 0 for the whole screen
	*/
	void take_screenshot(Response &response, uint32_t win_id);

	/* Add the object to the list of objects.

		@parent 	the ID of the parent
		@obj 		the Qt object to add
		@response 	the protobuff response, which gets build incrementally
	*/
	void add_object(unsigned int parent, QObject *obj, Response &response);

private:
	QTcpServer   *server;						/* the TCP server that listens to client requests */
	isabelX11    *x11;							/* interface with the X11 server */
	std::map<unsigned int, QObject *> objects; 	/* the current list of Qt objects */
}; 

#endif
