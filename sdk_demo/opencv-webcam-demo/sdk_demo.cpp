#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <opencv\highgui.h>
#include <filesystem>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Frame.h"
#include "Face.h"
#include "ImageListener.h"
#include "FrameDetector.h"
#include "VideoDetector.h"
#include "PhotoDetector.h"
#include "AffdexException.h"

#include <fstream>
#include <ctime>
#include <string>

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
		fout << image.getTimestamp() << endl;
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
			cout << "Writing data points to text file with timestamp: " << image.getTimestamp()
				<< "," << image.getWidth()
				<< "x" << image.getHeight()
				<< " cfps: " << capture_fps
				<< " pnts: " << points.size() << endl;
			process_last_timestamp = image.getTimestamp();

		}
		fout << endl;
		fout.close();
	};

	void onImageCapture(Frame image) override {
	};

};

class MyProcessStatusListener : public ProcessStatusListener
{
public:
	bool processing = true;
	void onProcessingFinished() override {
		notify();
	};

	void onProcessingException(AffdexException exception) override {
		std::cerr << exception.getExceptionMessage() << endl;
	}

	void wait() {
		std::cerr << "Video processing has started." << endl << endl;
		while (processing) {
		};
	}

	void notify() {
		std::cerr << endl  << "Video processing has finished." << endl;
		processing = false;
	}
};


int main(int argsc, char ** argsv) 
{
	if (argsc != 3) {
		cout << "Missing data type and file argument." << endl;
		return 0;
	}

	string data_type = argsv[1];
	string file = argsv[2];

	wstring path_to_file;
	path_to_file.assign(file.begin(), file.end());

	boolean video = false;
	boolean photo = false;

	if (data_type.compare("video") == 0) {
		video = true;
	}
	else if (data_type.compare("photo") == 0) {
		photo = true;
	}
	
	try{
		// Parse and check the data folder (with assets)
		const std::wstring AFFDEX_DATA_DIR = L"C:\\Program Files (x86)\\Affectiva\\Affdex SDK\\data";
		const std::wstring AFFDEX_LICENSE_FILE = L"C:\\Program Files (x86)\\Affectiva\\Affdex SDK\\affdex.license";

		shared_ptr<ImageListener> listenPtr(new PlottingImageListener());	// Instanciate the ImageListener class

		//VIDEO INPUT
		if (video)
		{
			cout << "Setting up Video Detector." << endl;
			VideoDetector videoDetector(30.0); //initialize VideoDetector
			MyProcessStatusListener listener; //initialize callback listener

			//set up VideoDetector settings
			videoDetector.setDetectAllEmotions(true);
			videoDetector.setDetectAllExpressions(true);
			videoDetector.setClassifierPath(AFFDEX_DATA_DIR);
			videoDetector.setLicensePath(AFFDEX_LICENSE_FILE);
			videoDetector.setImageListener(listenPtr.get());
			videoDetector.setProcessStatusListener(&listener);

			//Start the video detector
			videoDetector.start();
			if (videoDetector.isRunning()) {
				//Start processing the video
				videoDetector.process(path_to_file);
				//Wait until processing has finished
				listener.wait();
			}
			videoDetector.stop();
			cout << "Video Detector stopped." << endl;
			return 0;
		}
		
		//PHOTO INPUT
		if (photo)
		{
			cout << "Setting up Photo Detector." << endl;
			PhotoDetector photoDetector;
			photoDetector.setDetectAllEmotions(true);
			photoDetector.setDetectAllExpressions(true);
			photoDetector.setClassifierPath(AFFDEX_DATA_DIR);
			photoDetector.setLicensePath(AFFDEX_LICENSE_FILE);
			photoDetector.setImageListener(listenPtr.get());
			//Start the photo detector thread
			photoDetector.start();
			if (photoDetector.isRunning()) {
				cv::Mat img;
				img = cv::imread(file);
				if (!img.data) {
					std::cerr << "Could not read image data or file does not exist." << endl;
					return 1;
				}
				Frame f(img.size().width, img.size().height, img.data, Frame::COLOR_FORMAT::BGR);
				photoDetector.process(f);
			}
			photoDetector.stop();
			cout << "Photo Detector has stopped." << endl;
			return 0;




		}

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