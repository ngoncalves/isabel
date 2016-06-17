# Isabel: Qt Test Automation
# ==========================
#  :Copyright: (C) 2016  Nelson Gonçalves
#  :License: GPLv3
#

# set this to the correct path on your system
QMAKE := /opt/Qt5.4.1/5.4/gcc_64/bin/qmake

#--------------------------------------------------------
# no need to change below this line

export QMAKE

BIN    := ../bin
BUILD  := ./build
SERVER := ./server
CLIENT := ./client
TEST   := ./test

LIBSERVER := $(BUILD)/libserver.so

.PHONY: clean client protocol

all: $(LIBSERVER) client tests
	@echo '================================='
	@echo 'Copying binaries                 '
	@echo '================================='	
	-cp -f $(LIBSERVER) $(BIN)

$(LIBSERVER): $(SERVER)/protocol.pb.cpp 
	@echo '================================='
	@echo 'Generating Makefile for server   '
	@echo '================================='	
	-$(QMAKE) -o $(SERVER)/Makefile $(SERVER)/server.pro
	@echo '================================='
	@echo 'Building the server              '
	@echo '================================='	
	-rm -rf $(BUILD)/*.o
	-rm -rf $(BUILD)/moc_*
	make -C $(SERVER)

client: $(CLIENT)/protocol_pb2.py

$(SERVER)/protocol.pb.cpp: ./protocol.proto 
	@echo '=============================='
	@echo 'Generating protobuf C++ code  '
	@echo '=============================='
	-protoc --cpp_out $(SERVER) $^

$(CLIENT)/protocol_pb2.py: ./protocol.proto
	@echo '================================='
	@echo 'Generating protobuf Python code  '
	@echo '================================='
	-protoc --python_out $(CLIENT) $^

tests: $(LIBSERVER) client
	@echo '=========================='
	@echo 'Building the test cases   '
	@echo '=========================='
	make -C $(TEST)

.PHONY: clean clean-all run-tests

run-tests: tests
	@echo '==================================='
	@echo 'Running the Server Unit Test cases '
	@echo '==================================='	
	$(BUILD)/ut_server
	
	@echo '==================================='
	@echo 'Running the Client Unit Test cases '
	@echo '==================================='		
	python $(TEST)/ut_client/ut_client.py

	@echo '====================='
	@echo 'Testing the features '
	@echo '====================='		
	-cd $(TEST)/features; behave --stop

clean:
	@echo '=========================='
	@echo 'Cleaning previous builds  '
	@echo '=========================='
	-make -C $(SERVER) clean
	-rm -rf $(CLIENT)/*.pyc
	-rm -rf $(BUILD)/*

real-clean: clean
	make -C $(TEST) clean
