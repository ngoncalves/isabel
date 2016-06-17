TEMPLATE = app

QT += qml quick

OBJECTS_DIR = ../../build
MOC_DIR     = ../../build
DESTDIR 	= ../../build

SOURCES += main.cpp

RESOURCES += calqlatr.qrc \
    		 ./shared/shared.qrc

OTHER_FILES = calqlatr.qml \
    		  content/Button.qml \
    		  content/Display.qml \
    		  content/NumberPad.qml \
    		  content/calculator.js \
			  content/images/paper-edge-left.png \
              content/images/paper-edge-right.png \
              content/images/paper-grip.png
