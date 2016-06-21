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

   See the header file for details.

 */
#include "isabelServer.h"
#include "isabelSLIP.h"
#include "isabelSerialize.h"

#include <QByteArray>
#include <QFile>
#include <QApplication>
   
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

#include <QtCore/QObject>
#include <QtCore/QBuffer>
#include <QtCore/QDataStream>
#include <QtCore/QMetaType>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>

#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickView>

#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlListProperty>

/*--------------------- Private Variable Declarations ----------------*/

/*--------------------- Public Class Definitions -------------------*/

isabelServer::isabelServer(int port, QObject *parent)
: QObject(parent)
{
	x11 = new isabelX11(this); 

	/* create the TCP server */
	server = new QTcpServer(this);

	connect(server,SIGNAL(newConnection()),this,SLOT(new_connection()));

	if(!server->listen(QHostAddress::Any,port))
	{
		qDebug() << "[isabel] TCP server failed to start" ;
	}
}

isabelServer::~isabelServer()
{
	server->close();
	delete server;
	delete x11;
}

void isabelServer::new_connection(void)
{
	/* get the client connection */
	QTcpSocket *client = server->nextPendingConnection();

	/* handle its requests until the connection is closed */
	connect(client,SIGNAL(readyRead()),this,SLOT(ready_read()));
	connect(client,SIGNAL(disconnected()),this,SLOT(disconnected()));
}

void isabelServer::ready_read(void)
{
	QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());

	/* read the complete request from the client */
	QByteArray rx_packet = slip_decode(client->readAll());

	if(0 < rx_packet.count())
	{
		Request  request; 
		Response response; 
	
		request.ParseFromArray(rx_packet.constData(),rx_packet.count());

		switch(request.type())
		{
			case Request::FETCH_OBJECT_TREE:
				fetch_object_tree(response);
				break; 

			case Request::FETCH_OBJECT:
				fetch_object(response,request.id());
				break; 

			case Request::WRITE_PROPERTY:
				write_object_property(response,request.id(),request.property());
				break; 

			case Request::RECORD_USER:
				record_user(response,request.start());
				break;

			case Request::SIMULATE_USER:
				simulate_user(response,request);
				break; 

			case Request::TAKE_SCREENSHOT:
				take_screenshot(response);
				break;

			case Request::KILL_APP:
				/* before quitting ,send the reply to the client */
				{
					response.set_error(Response::NO_ERROR);

					QByteArray tx_packet(response.ByteSize(),0);
					response.SerializeToArray(tx_packet.data(),tx_packet.count());
					client->write(slip_encode(tx_packet));
					client->waitForBytesWritten();
				}
				
				/* goodbye */
				QApplication::quit();
				break;
			default:
				response.set_error(Response::INVALID_REQUEST);
				break; 
		}

		QByteArray tx_packet(response.ByteSize(),0);
		response.SerializeToArray(tx_packet.data(),tx_packet.size());
		client->write(slip_encode(tx_packet));
		client->waitForBytesWritten();
	}
}

void isabelServer::disconnected(void)
{
	QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
	client->deleteLater();
}

void isabelServer::fetch_object_tree(Response &response)
{
	/* clean the list of objects */
	objects.clear(); 

	/* and rebuild it */
	QList<QWidget*> widgets = QApplication::topLevelWidgets();
	QList<QWindow*> windows = QApplication::topLevelWindows();

	Q_FOREACH(QObject *object, widgets)
	{
		add_object(0,object,response);
	}
	
	Q_FOREACH(QObject *object, windows)
	{
		/* QML based windows are not shown as widgets, need to treat them separately:
			- first we add the window
			- then we add the root object of the QQuickView
		 */
		add_object(0,object,response);
		 		
		QQuickView *viewObj = qobject_cast<QQuickView*>(object);

		if(NULL != viewObj)
		{
			QQuickItem *rootObject = viewObj->rootObject();
			add_object(0,rootObject,response);	
		}
	}

	response.set_error(Response::NO_ERROR);
}

void isabelServer::add_object(unsigned int parent, QObject *obj, Response &response)
{
	/* first add the object */
	unsigned int id = objects.size() + 1; 
	objects.insert(std::pair<unsigned int, QObject *>(id,obj)); 
	
	Object *qtObj = response.add_objects(); 
	qtObj->set_id(id);
	qtObj->set_parent(parent);
	qtObj->set_address(reinterpret_cast<quintptr>(obj));
	qtObj->set_type(obj->metaObject()->className());
	qtObj->set_name(obj->objectName().toUtf8().constData());

	/* and then its children */
	Q_FOREACH(QObject* child, obj->children())
	{
		add_object(id,child,response);
	}
}

void isabelServer::fetch_object(Response &response, unsigned int id)
{
	std::map<unsigned int,QObject *>::iterator iter = objects.find(id);

	if(objects.end() == iter)
	{
		response.set_error(Response::UNKNOWN_OBJECT_ID);	
	}
	else
	{
		QObject *object = iter->second;

		for(int i = 0; i < object->metaObject()->propertyCount(); i++)
		{
			Property* prop = response.add_properties();
			QMetaProperty property = object->metaObject()->property(i);
			
			prop->set_name(property.name());
			prop->set_writable(property.isWritable());

			QByteArray value = serialize_encode(property.read(object));
			prop->set_value(value.constData(),value.count());
		}

		response.set_error(Response::NO_ERROR);		
	}
}

void isabelServer::write_object_property(Response &response, unsigned int id, const Property &property)
{
	std::map<unsigned int,QObject *>::iterator iter = objects.find(id);

	if(objects.end() == iter)
	{
		response.set_error(Response::UNKNOWN_OBJECT_ID);	
	}
	else
	{
		QObject *object = iter->second;

		QByteArray buffer(property.value().c_str(),property.value().size());
		QVariant value = serialize_decode(buffer);
		object->setProperty(property.name().c_str(),value);

		response.set_error(Response::NO_ERROR);		
	}
}

void isabelServer::record_user(Response &response, bool start)
{
	if(start)
	{
		x11->start_recording(); 
		response.set_error(Response::NO_ERROR);
	}
	else
	{
		std::vector<UserEvent *> events = x11->stop_recording(); 

		for(unsigned int e = 0; e < events.size(); e++)
		{
			UserEvent *ev = response.add_events();
			ev->CopyFrom(*events[e]); 
		}

		response.set_error(Response::NO_ERROR);
	}
}

void isabelServer::simulate_user(Response &response, const Request &request)
{
	if(x11->simulate_user(request.user()))
	{
		response.set_error(Response::NO_ERROR);
	}
	else
	{
		response.set_error(Response::X11_ERROR);
	}
}

void isabelServer::take_screenshot(Response &response)
{
	if(x11->screenshot("/tmp/screenshot.png"))
	{
		/* now load the file into RAM and send it over the wire */
		QFile file("/tmp/screenshot.png");
		if(!file.open(QIODevice::ReadOnly))
		{
			response.set_error(Response::UNKNOWN_ERROR);	
		}
		else
		{
			QByteArray blob = file.readAll();
			response.set_image(blob.constData(),blob.size());
			response.set_error(Response::NO_ERROR);
		}
	}
	else
	{
		response.set_error(Response::X11_ERROR);
	}
}
