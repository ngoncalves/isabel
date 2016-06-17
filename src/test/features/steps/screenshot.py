# -*- coding: iso-8859-15 -*-
"""
   Isabel: Qt Test Automation
   ==========================
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

   Implementation of the test steps for the screenshot feature.
"""
ROOT_SRC_DIR = '../../'

import sys
import os
import subprocess
import time

# add the path to the client
sys.path.append(ROOT_SRC_DIR + 'client')
import client
		
#----------- behave steps ------------------------------------------
@given('the AUT is running')
def step_impl(context):
	os.environ['LD_PRELOAD'] = os.getcwd() + '/' + ROOT_SRC_DIR + 'build/libserver.so'
	command = os.getcwd() + '/' + ROOT_SRC_DIR + 'build/application'
	context.aut = subprocess.Popen([command])

	# give the application time to start
	time.sleep(2)
	assert context.aut.pid > 0

@given('the client is connected to the server')
def step_impl(context):
	context.client = client.Client()
	assert context.client.connect('localhost')

@when('the client requests a screenshot')
def step_impl(context):
	assert context.client.take_screenshot(os.getcwd() + '/' + ROOT_SRC_DIR + 'build/screenshot.png')

@then('a PNG file is created on the client side')
def step_impl(context):
	# verify the file was created and has a relevant size
	assert os.path.isfile(os.getcwd() + '/' + ROOT_SRC_DIR + 'build/screenshot.png')
	assert os.path.getsize(os.getcwd() + '/' + ROOT_SRC_DIR + 'build/screenshot.png') > 50*1024

	# close the client and the application
	assert context.client.disconnect()
	context.aut.kill()
	del context.aut