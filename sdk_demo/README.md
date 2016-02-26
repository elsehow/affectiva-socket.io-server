Dependencies

- Affdex SDK 2.0 (32 bit)
- OpenCV for Windows 2.4.9: http://sourceforge.net/projects/opencvlibrary/files/opencv-win/2.4.9/
- Visual Studio 2013 or higher
- Affectiva SDK License

Data points are outputted to STDOUT as JSON.



COMPILING
---------

- Download OpenCV and place the folder in the root folder.
- Open the "opencv-webcam-demo/opencv-webcam-demo" file and use Visual Studio to compile.
- This will output a ".exe" file in "Release/".
- "opencv_ffmpeg248.dll" is a necessary codec for running video analysis.
	- It needs to be placed in the same directory as the executable.


RUNNING
-------

- Open CMD and cd to the folder contain the ".exe" file.
- Run using "sdk_demo.exe [type] [path to file]"
	- Type options: video, photo

TROUBLESHOOTING
---------------

- If you get some sort of "could not read image data" error, make sure the path to the image file is correct.