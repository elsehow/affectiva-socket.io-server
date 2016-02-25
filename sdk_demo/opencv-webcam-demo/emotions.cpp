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

string JSON_OUT = "";

class PlottingImageListener : public ImageListener
{
	int results_counter = 0;
public:
	void onImageResults(std::map<FaceId,Face> faces, Frame image) {
		if (faces.size() != 0) {
			if (results_counter == 0) {
				JSON_OUT += "[{\n"; //Beginning of list and new Frame object
			}
			if (results_counter != 0) {
				JSON_OUT += "\n{\n"; //New Frame object
			}
			results_counter++;
			time_t t = time(0);
			struct tm now;
			localtime_s(&now, &t);

			//Date and Time object
			JSON_OUT += "    \"date and time\" : \""
				+ to_string(now.tm_mon + 1) + '-' + to_string(now.tm_mday) + '-' + to_string(now.tm_year + 1900)
				+ ' ' + to_string(now.tm_hour) + ':' + to_string(now.tm_min) + ':' + to_string(now.tm_sec) + "\",\n";
			//Timestamp object
			JSON_OUT += "    \"timestamp\" : " + to_string(image.getTimestamp()) + ",\n";

			for (unsigned int i = 0; i < faces.size(); i++)
			{
				Face f = faces[i];
				VecFeaturePoint points = f.featurePoints;
				Orientation headAngles = f.measurements.orientation;

				JSON_OUT += "    \"measurements\" : {"; //Begin Measurement object
				JSON_OUT += "\n        \"yaw\" : " + to_string(headAngles.yaw) + ","; //Yaw
				JSON_OUT += "\n        \"roll\" : " + to_string(headAngles.roll) + ","; //Roll
				JSON_OUT += "\n        \"interoculardist\" : " + to_string(f.measurements.interocularDistance); //Interocular Distance
				JSON_OUT += "\n    },\n"; //End Measurement object

				//Output the results of the different classifiers.
				std::vector<std::string> expressions{ "smile", "innerBrowRaise", "browRaise", "browFurrow", "noseWrinkle",
														"upperLipRaise", "lipCornerDepressor", "chinRaise", "lipPucker", "lipPress",
														"lipSuck", "mouthOpen", "smirk", "eyeClosure", "attention" };

				float * values = (float *)&f.expressions;

				JSON_OUT += "    \"expressions\" : {"; //Begin Expressions object

				int expressions_counter = 1; //Counter to find last expression
				for (string expression : expressions)
				{
					if (expressions_counter == expressions.size()) { //Last expression so no comma at end
						JSON_OUT += "\n        \"" + expression + "\" : " + std::to_string(int(*values));
					}
					else {
						JSON_OUT += "\n        \"" + expression + "\" : " + std::to_string(int(*values)) + ",";
					}
					values++;
					expressions_counter++;
				}
				JSON_OUT += "\n    },\n"; //End Expressions object

				std::vector<std::string> emotions{ "joy", "fear", "disgust", "sadness", "anger", "surprise", "contempt", "valence", "engagement" };
				values = (float *)&f.emotions;

				JSON_OUT += "    \"emotions\" : { "; //Begin Emotions object
				int emotions_counter = 1;
				for (string emotion : emotions)
				{
					if (emotions_counter == emotions.size()) { //Last emotion so no comma at end
						JSON_OUT += "\n        \"" + emotion + "\" : " + std::to_string(int(*values));
					}
					else {
						JSON_OUT += "\n        \"" + emotion + "\" : " + std::to_string(int(*values)) + ",";
					}
					values++;
					emotions_counter++;
				}
				JSON_OUT += "\n    }"; //End Emotions object

				JSON_OUT += "\n},"; //End of Frame object

				/* cout << "Found data points with timestamp: " << image.getTimestamp()
					<< "," << image.getWidth()
					<< "x" << image.getHeight()
					<< " cfps: " << capture_fps
					<< " pnts: " << points.size() << endl; */
				process_last_timestamp = image.getTimestamp();

			}
		}
	};

	void onImageCapture(Frame image) override { //Function for missed frames.
	};

	static void write_to_JSON() {
		JSON_OUT.pop_back(); //Remove extra comma
		JSON_OUT += "]"; //End of list of objects

		std::cout << "Writing to JSON file called emotions_analysis.json" << endl;

		ofstream json;
		json.open("emotions_analysis.json", ios_base::app);
		json << JSON_OUT;
		json.close();
		cout << "Done writing to JSON file." << endl;

		exit(EXIT_SUCCESS); //TEMPORARY
	}

	static void output_to_STDOUT() {
		JSON_OUT.pop_back(); //Remove extra comma
		JSON_OUT += "]"; //End of list of objects

		std::cout << JSON_OUT;
		exit(EXIT_SUCCESS);
	}

};

class MyProcessStatusListener : public ProcessStatusListener //FOR VIDEO PROCESSING ONLY
{
	bool processing = true;
public:
	void onProcessingFinished() override {
		notify();
	};

	void onProcessingException(AffdexException exception) override {
		std::cerr << exception.getExceptionMessage() << endl;
	}

	void wait() {
		while (processing) {
		}
	}

	void notify() {
		processing = false;
		PlottingImageListener::output_to_STDOUT();
	}
};

int main(int argsc, char ** argsv) 
{
	if (argsc != 3) {
		cerr << "Missing data type and file argument." << endl;
		return 1;
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
			VideoDetector videoDetector(30.0); //initialize VideoDetector
			MyProcessStatusListener listener; //initialize Callback Listener

			//Set up VideoDetector settings
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
			return 0;
		}
		
		//PHOTO INPUT
		if (photo)
		{
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

			if (JSON_OUT != "") { //Found face in photo so write to JSON file
				std::cout << JSON_OUT;
				return 0;
			}
			else if (JSON_OUT == "") {
				return 1;
			}
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