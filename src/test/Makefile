# Isabel: Qt Test Automation
# ==========================
#  :Copyright: (C) 2016  Nelson Gonçalves
#  :License: GPLv3
#

#--------------------------------------------------------
# no need to change below this line
BUILD     := ../build
UT_SERVER := $(BUILD)/ut_server
QT_APP    := $(BUILD)/application
QUICK_APP := $(BUILD)/calqlatr

all: $(UT_SERVER) $(QT_APP) $(QUICK_APP)

$(UT_SERVER):
	@echo '==================================='
	@echo 'Building the Server Unit Test cases'
	@echo '==================================='	
	-$(QMAKE) -o ut_server/Makefile ut_server/ut_server.pro
	-rm -rf $(BUILD)/*.o
	-rm -rf $(BUILD)/moc_*
	make -C ut_server

$(QT_APP):
	@echo '================================='
	@echo 'Building the Qt example app      '
	@echo '================================='	
	-$(QMAKE) -o qt_app/Makefile qt_app/application.pro
	-rm -rf $(BUILD)/*.o
	-rm -rf $(BUILD)/moc_*
	make -C qt_app
	
$(QUICK_APP):
	@echo '================================='
	@echo 'Building the QtQuick example app ' 
	@echo '================================='	
	-$(QMAKE) -o quick_app/Makefile quick_app/calqlatr.pro
	-rm -rf $(BUILD)/*.o
	-rm -rf $(BUILD)/moc_*
	make -C quick_app

.PHONY: clean

clean:
	@echo '================================='
	@echo 'Cleaning the previous builds     ' 
	@echo '================================='		
	-make -C test_lib clean
	-make -C qt_app clean
	-make -C quick_app clean
	-rm -rf $(BUILD)/*