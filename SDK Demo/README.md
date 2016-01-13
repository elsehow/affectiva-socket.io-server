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

- Double click "opencv-webcam-demo.exe".


TROUBLESHOOTING
---------------

- If you get some sort of "webcam failed to open" error, clean and recompile the project.