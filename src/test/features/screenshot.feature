# Isabel: Qt Test Automation
# ==========================
#  :Copyright: (C) 2016  Nelson Gonçalves
#  :License: GPLv3

Feature: Taking a screenshot of the entire display

	The server that is running on the Application Under Test (AUT), provides
	the functionality to take a screenshot of the entire display. Without
	a more complex testing environment, it is not possible to antecipate
	how the screenshot will lool like. For this reason, the feature only
	verifies that a PNG file was created.

Scenario: taking a screenshot of the entire display
	Given the AUT is running
	And the client is connected to the server
	When the client requests a screenshot
	Then a PNG file is created on the client side