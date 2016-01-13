#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <opencv\highgui.h>
#include <filesystem>

#include "Frame.h"
#include "Face.h"
#include "ImageListener.h"
#include "FrameDetector.h"
#include "AffdexException.h"

#include <fstream>
#include <ctime>

using namespace std;
using namespace affdex;

float last_timestamp = -1.0f;
float capture_fps = -1.0f;
float process_last_timestamp = -1.0f;
float process_fps = -1.0f;


class PlottingImageListener : public ImageListener
{
public:
	void onImageResults(std::map<FaceId,Face> faces, Frame image) {
		
		std::ofstream fout;
		fout.open("emotion-analysis.txt", std::ios_base::app);

		for (unsigned int i = 0; i < faces.size(); i++)
		{
			fout << "///////////////////////////////////////////////" << endl;
			time_t t = time(0);
			struct tm now;
			localtime_s(&now, &t);
			fout << "DATE AND TIME: "
				 << (now.tm_mon + 1) << '-' << (now.tm_mday) << '-' << (now.tm_year + 1900) 
				 << ' ' << (now.tm_hour) << ':' << (now.tm_min) << ':' << (now.tm_sec)
				 << endl;

			Face f = faces[i];
			VecFeaturePoint points = f.featurePoints;

			Orientation headAngles = f.measurements.orientation;
			std::string strAngles = "Pitch: " + std::to_string(headAngles.pitch) +
				" Yaw: " + std::to_string(headAngles.yaw) +
				" Roll: " + std::to_string(headAngles.roll) +
				" InterOcularDist: " + std::to_string(f.measurements.interocularDistance);
			fout << endl << "MEASUREMENTS" << endl;
			fout << strAngles << endl; //write Pitch, Yaw, Roll, and InterOcularDist

			//Output the results of the different classifiers.
			std::vector<std::string> expressions{ "smile", "innerBrowRaise", "browRaise", "browFurrow", "noseWrinkle",
													"upperLipRaise", "lipCornerDepressor", "chinRaise", "lipPucker", "lipPress",
													"lipSuck", "mouthOpen", "smirk", "eyeClosure", "attention" };

			std::vector<std::string> emotions{ "joy", "fear", "disgust", "sadness", "anger", "surprise", "contempt", "valence", "engagement" };

			float * values = (float *)&f.expressions;
			fout << endl << "EXPRESSIONS" << endl;
			for (string expression : expressions)
			{
				fout << expression << ": " << std::to_string(int(*values)) << endl;
				values++;
			}

			values = (float *)&f.emotions;

			fout << endl << "EMOTIONS" << endl;
			for (string emotion : emotions)
			{
				fout << emotion << ": " << std::to_string(int(*values)) << endl;
				values++;
			}
			std::cerr << "Timestamp: " << image.getTimestamp()
				<< "," << image.getWidth()
				<< "x" << image.getHeight()
				<< " cfps: " << capture_fps
				<< " pnts: " << points.size() << endl;
			process_last_timestamp = image.getTimestamp();

		}
		fout << endl;
		fout.close();
	};

	void onImageCapture(Frame image) override
	{};
};

int main(int argsc, char ** argsv) 
{

	try{
		// Parse and check the data folder (with assets)
		const std::wstring AFFDEX_DATA_DIR = L"C:\\Program Files (x86)\\Affectiva\\Affdex SDK\\data";
		const std::wstring AFFDEX_LICENSE_FILE = L"C:\\Program Files (x86)\\Affectiva\\Affdex SDK\\affdex.license";

		int framerate = 30;
		int process_frame_rate = 30;
		int buffer_length = 2;

		FrameDetector frameDetector(buffer_length, process_frame_rate);		// Init the FrameDetector Class
		shared_ptr<ImageListener> listenPtr(new PlottingImageListener());	// Instanciate the ImageListener class

		cv::VideoCapture webcam(0);	//Connect to the first webcam
		webcam.set(CV_CAP_PROP_FPS, framerate);	//Set webcam framerate.
		std::cerr << "Setting the webcam frame rate to: " << framerate << std::endl;
		auto start_time = std::chrono::system_clock::now();
		if (!webcam.isOpened())
		{
			std::cerr << "Error opening webcam!" << std::endl;
			return 1;
		}

		//Initialize detectors
		frameDetector.setDetectAllEmotions(true);
		frameDetector.setDetectAllExpressions(true);
		frameDetector.setClassifierPath(AFFDEX_DATA_DIR);
		frameDetector.setLicensePath(AFFDEX_LICENSE_FILE);
		frameDetector.setImageListener(listenPtr.get());
		//Start the frame detector thread.
		frameDetector.start();

		do{
			cv::Mat img;
			if (!webcam.read(img))	//Capture an image from the camera
			{
				std::cerr << "Failed to read frame from webcam! " << std::endl;
				break;
			}

			//Calculate the Image timestamp and the capture frame rate;
			const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time);
			const float seconds = milliseconds.count() / 1000.f;
			// Create a frame
			Frame f(img.size().width, img.size().height, img.data, Frame::COLOR_FORMAT::BGR, seconds);
			capture_fps = 1.0f / (seconds - last_timestamp);
			last_timestamp = seconds;
			std::cerr << "Capture framerate = " << capture_fps << std::endl;
			frameDetector.process(f);  //Pass the frame to detector
		} while (!GetAsyncKeyState(VK_ESCAPE));

		frameDetector.stop();	//Stop frame detector thread
	}
	catch (AffdexException ex)
	{
		std::cerr << "Encountered an AffdexException " << ex.what();
		return 1;
	}
	catch (std::exception ex)
	{
		std::cerr << "Encountered an exception " << ex.what();
		return 1;
	}
	catch (std::runtime_error err)
	{
		std::cerr << "Encountered a runtime error " << err.what();
		return 1;
	}

	return 0;
}