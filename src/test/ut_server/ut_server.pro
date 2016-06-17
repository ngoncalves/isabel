QT 			+= core
QT 			-= gui
CONFIG      += debug
OBJECTS_DIR = ../../build
MOC_DIR     = ../../build
DESTDIR 	= ../../build
INCLUDEPATH += ../../qjson ../../server /usr/include/google/protobuf
LIBS        += -L /usr/lib -lprotobuf -lcairo -lX11

HEADERS  	= ../../server/isabelSLIP.h \
			  ut_slip.h	

SOURCES  	= ../../server/isabelSLIP.cpp \
			  ut_slip.cpp \
			  main.cpp
				
