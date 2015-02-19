# About
A GUI front-end for creating tool paths for a VXM stepping motor controller
based on video input. The controller commands used in this application are
specific to Velmex VXM stepping motor controllers.

# Building
This application requires the qt5 framework, along with the serial port and
multimedia extensions. To build, generate a Makefile:

    $ qmake -o Makefile vxm-control.pro

and build:

    $ make

If building with MSVC gives a "`failure during conversion to COFF`" error, check
comment in qmake file (vxm-control.pro).

# Usage
To use the VXM Controller software follow these steps:

	1.) ensure the EasyCAP driver software is installed
	2.) plug in both the EasyCAP video capture device, and the USB Serial
		adapter
	3.) launch the Controller software by running vxm-control.exe
	4.) in the controller software, configure the camera input by selecting
		Config -> Camera..., and selecting the EasyCAP device. Also specify if
		it is necessary to mirror the image
	5.) In the controller software, configure the serial connection. This can
		be accomplished by selecting Config -> Serial... and selecting the
		appropriate COM device. All other options can be left as default.
	6.) Connect to the controller by clicking the 'Connect' button.
	7.) If this is the first time running the software, you may need to
		calibrate the controller. The controller is calibrated by selecting an
		initial point, moving the platform a specific number of steps, and
		measuring the difference in pixels. This is done by selecting the
		'Calibrate' button, and following the instructions.
		TIPS:
			- Select a reference point that is in the middle of the screen.
				The platform can be moved into position prior to calibration.
			- Ensure the platform is properly aligned.
			- Make sure the platform has enough room to move in each direction
				so that the limit switches aren't activated during calibration.
	8.) Draw the shapes/paths you wish to weld, and and click the Move button.


### WARNING / Potential Usage Problems
The limit switches for one axis are wired incorrectly, so it is possible
for the platform to get 'stuck' if it activates those limit switches. The
easiest way get the platform 'unstuck' is to temporarily switch around
the two sets of limit switches and then use vxm-control program to
manually move the platform off of the switch.

> Note: the platform wont move if the limit switches aren't plugged in, so
> simply plugging in the other axis' switches will trick the platform into
> moving again.


