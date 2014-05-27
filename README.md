# About
A GUI frontend for creating tool paths for a VXM stepping motor controller
based on video input.

# Building
This application requires the qt5 framework, along with the serialport and
multimedia extensions. To build, generate a Makefile:

    $ qmake -o Makefile vxm-control.pro

and build:

    $ make

