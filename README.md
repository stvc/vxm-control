# About
A GUI frontend for creating tool paths for a VXM stepping motor controller
based on video input. The controller commands used in this application are
specific to Velmex VXM stepping motor controllers.

# Building
This application requires the qt5 framework, along with the serialport and
multimedia extensions. To build, generate a Makefile:

    $ qmake -o Makefile vxm-control.pro

and build:

    $ make

If building with MSVC gives a "failure during conversion to COFF" error, check
comment in qmake file (vxm-control.pro).

