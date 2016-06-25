Known Issues
===============
:Author: Nelson Gonçalves
:Email: nelsongoncalves@patois.eu
:Website: https://github.com/ngoncalves


Rationale
---------

The day only has 24 hours and I think it is preferable to release
something that is good enough to generate interest and feedback, than
to sit years in seclusion while building the perfect software that will
change mankind.

Server
------

With Qt5, replaying keyboard symbols with shift group 1, that is symbols formed
by combining SHIT with other keys ('A', 'B', e.g.), does not work. The SHIFT 
key is ignored, apparently because Qt5 switched to XCB for interacting with the
X11 server. Note that the SHIFT key presses and releases are still captured, just
not replayed correctly.

The serialization of the object properties is not fully functional, as some QVariants
will not be serialized. This is a limitation of the current serialization solution,
which is based on QtJSON. The most straightforward solution would be to simply re-implement
the Qt serialization protocol, but this requires free time that I don't have.

Taking a screenshot when the Qt application is running in full screen may or may not
work. Typically all you get a picture with all pixels black. The reason for this is
not known yet.

Client
------

Making a request when the client is not connected, results in some exceptions being
raised. The error handling, in general, could be improved as well.
