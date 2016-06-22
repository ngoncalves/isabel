#!/usr/bin/env bash

#
# Build script for Isabel
# Adapt the path of to your local installation of Qt
#

QTDIR=/usr/local/Qt-5.4.1

###############################################################
# no need to change below this line

show_usage()
{
	echo ""
	echo "Usage: $0 isabel | tests | clean | purge | depends"
	echo ""
	echo "where:"
	echo "   isabel  : build the server and client"
	echo "   tests   : build and run the test cases"
	echo "   clean   : clean previous build"
	echo "   purge   : clean the previous build and all intermediate files"
	echo "   depends : install all the necessary build dependencies" 
	echo ""
	exit 1
}

run_make()
{
	# setup the Qt build environment
	export PATH=$QTDIR/bin:$PATH
	export LD_LIBRARY_PATH=$QTDIR/lib:$LD_LIBRARY_PATH
	export QT_PLUGIN_PATH=$QTDIR/plugins

	if [ $1 == "isabel" ]
	then
		make -C src clean
		make -C src
	elif [ $1 == "tests" ]
	then
		make -C src tests
		make -C src run-tests
	elif [ $1 == "clean" ]
	then
		make -C src clean
	elif [ $1 == "purge" ]
	then
		make -C src real-clean
		rm -rf bin/lib*
	elif [ $1 == "depends" ]
	then
		echo "[build] installing build dependencies:"
		sudo apt-get update
		sudo apt-get install libprotoc-dev protobuf-compiler libx11-dev libxtst-dev libcairo2-dev python-pip python-opencv
		sudo pip install tinydb protobuf
	else
		echo "[build] unknown option: $1" 
		show_usage
	fi
}

echo "$#" 

if ! [ $# -eq 1 ]
then
	show_usage
else
	run_make $1
fi
exit 0

