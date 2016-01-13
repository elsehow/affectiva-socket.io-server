OpenCV-webcam-demo
------------------

*Dependencies*

- Affdex SDK 2.0 (32 bit)
- OpenCV for Windows 2.4.9: http://sourceforge.net/projects/opencvlibrary/files/opencv-win/2.4.9/
- Visual Studio 2013 or higher
- Affectiva SDK License

OpenCV-webcam-demo is a simple app that uses the camera connected to your PC to view your facial expressions and face points.

Data points are written to a ".txt" file in the "Release" folder.



COMPILING
---------

- Download OpenCV and place the folder in the root folder.
- Open the "opencv-webcam-demo/opencv-webcam-demo" file and use Visual Studio to compile.
- This will output a ".exe" file in "Release/".


RUNNING
-------

- TEMPORARY
	- By default (for now) the SDK will take in a photo in the "Release" folder called "1.png".
	- To change mode (photo, video, webcam), look for the booleans within the source file "OpenCV-webcam-demo.cpp"
	- Video mode will take in a video in webm format in the "Release" folder called "1.webm".
		- Video mode is currently buggy. I'm working on resolving this.

- Double click "opencv-webcam-demo.exe" to run.
	- A file called "emotion-analysis.txt" should be outputted with the results.


TROUBLESHOOTING
---------------

- If you get some sort of "failed to open webcam" error, try relaunching.
- If that fails continuously, clean and recompile.