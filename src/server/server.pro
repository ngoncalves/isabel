TEMPLATE    = lib
QT 			+= concurrent network widgets quick
CONFIG      += release plugin dll
CONFIG      -= debug
OBJECTS_DIR = ../build
MOC_DIR     = ../build
DESTDIR 	= ../build
INCLUDEPATH += ../qjson /usr/include/google/protobuf
LIBS        += -L /usr/lib -lprotobuf -lcairo -lX11 -lXtst -lXext

HEADERS  	= isabel.h \
			  isabelStartup.h \
			  isabelServer.h \
			  isabelX11.h \
			  isabelSLIP.h \
			  isabelSerialize.h \
			  json.h \
			  protocol.pb.h

SOURCES  	= isabel.cpp \
			  isabelStartup.cpp \
			  isabelServer.cpp \
			  isabelX11.cpp \
			  isabelSLIP.cpp \
			  isabelSerialize.cpp \
			  json.cpp \
			  protocol.pb.cc
