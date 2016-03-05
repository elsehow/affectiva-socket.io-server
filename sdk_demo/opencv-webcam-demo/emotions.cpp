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
			
			//Video timestamp object
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

				process_last_timestamp = image.getTimestamp();

			}
		}
	};

	void onImageCapture(Frame image) override { //Function for missed frames.
	};

	static void output_to_STDOUT() {
		if (!JSON_OUT.empty()) {
			JSON_OUT.pop_back(); //Remove extra comma
			JSON_OUT += "]"; //End of list of objects
			std::cout << JSON_OUT;
			exit(EXIT_SUCCESS); //TEMPORARY
		}
		else {
			std::cerr << "Unable to proces video." << endl;
			exit(EXIT_FAILURE); //TEMPORARY
		}
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
			processing = true;
		}
	}

	void notify() {
		processing = false;
		PlottingImageListener::output_to_STDOUT();
	}
};

int main(int argsc, char ** argsv) 
{
	if (argsc != 2) {
		cerr << "Missing video file path argument." << endl;
		return 1;
	}

	string file = argsv[1];

	wstring path_to_file;
	path_to_file.assign(file.begin(), file.end());
	
	try{
		// Parse and check the data folder (with assets)
		const std::wstring AFFDEX_DATA_DIR = L"C:\\Program Files (x86)\\Affectiva\\Affdex SDK\\data";
		const std::wstring AFFDEX_LICENSE_FILE = L"C:\\Program Files (x86)\\Affectiva\\Affdex SDK\\affdex.license";

		shared_ptr<ImageListener> listenPtr(new PlottingImageListener());	// Instanciate the ImageListener class

		//VIDEO INPUT
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