#include "freenect-playback-wrapper.h"
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include <opencv2/ml.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/ml/ml.hpp>
#include <fstream> // text file reading
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace cv::ml;
using namespace std;

int SZ = 20;
float affineFlags = WARP_INVERSE_MAP | INTER_LINEAR;
int imgCount = 0;

// files pathname
string pathName;
string labelpathName;

// load labels 
void loadLabels(string &labelpathName, vector<string> &labels) {
	int cnt = 0;
	std::stringstream sstm;
	string line = sstm.str();
	
	// read label file
	ifstream myReadFile;

	// open file
	myReadFile.open(labelpathName);

	// if file is open
	if (myReadFile.is_open()) {
		labels.push_back("Background");
		//cout << "Labels : " << labels[cnt] << endl;
		while (!myReadFile.eof()) {
			getline(myReadFile, line);
			labels.push_back(line);
			cnt++;
			//cout << "Labels : " << labels[cnt] << endl;
		}
	}
}

// load image and build training Data/Labels sets
void loadTrainLabel(string &pathName, vector<string> &labels, vector<Mat> &trainCells, vector<int> &trainCellsLabels) {
        // variable declaration
	Mat currentRGB;
	Mat currentDepth;
	Mat HSV; //matrix storage for HSV image
	Mat threshold; //matrix storage for binary threshold image
	
	//x and y values for the location of the object
	int x = 0, y = 0;
	
	// read training set
	FreenectPlaybackWrapper wrap(pathName);

	// Create the RGB, Depth, HSV, Threshold Windows
	namedWindow("RGB", cv::WindowFlags::WINDOW_AUTOSIZE | cv::WindowFlags::WINDOW_GUI_EXPANDED);
	//namedWindow("Depth", cv::WindowFlags::WINDOW_AUTOSIZE | cv::WindowFlags::WINDOW_GUI_EXPANDED);

	// The key value represents a key pressed on the keyboard,
	// where 27 is the ESCAPE key
	char key = '0';
	char file_name[100];
	// The status represents the current status of the Playback
	// wrapper. 
	//
	// A value of 0 represents that it has finished
	// playback.
	//
	// The status can by bitwise AND to determine if the RGB or
	// Depth image has been updated using the State enum.
	uint8_t status = 255;
	
	while (key != 27 && status != 0)
		{
			// Loads in the next frame of Kinect data into the
			// wrapper. Also pauses between the frames depending
			// on the time between frames.
			status = wrap.GetNextFrame();

			// Determine if RGB is updated, and grabs the image
			// if it has been updated
			if (status & State::UPDATED_RGB)
				currentRGB = wrap.RGB;

			// Determine if Depth is updated, and grabs the image
			// if it has been updated
			if (status & State::UPDATED_DEPTH)
				currentDepth = wrap.Depth;

			// Show the images in the windows
			imshow("RGB", currentRGB);
			//imshow("Depth", currentDepth);
		
		   	//ImgCount++;

			// identify the image with atleast 25% object area based on pixel value change
			int cnt = 0;
			Scalar intensity;
			for (int j = 0; j < currentDepth.rows; j++)
			{
				for (int i = 0; i < currentDepth.cols; i++)
				{
					intensity = currentDepth.at<uchar>(j, i);
					if (intensity.val[0] != 95 && intensity.val[0] != 96 && intensity.val[0] != 97
						&& intensity.val[0] != 98 && intensity.val[0] != 99 && intensity.val[0] != 255
						)
					{
						cnt++;
					}
				}
			}
	}
}

int main(int argc, char * argv[])
{

	// variable declaration of vector typ
	vector<int> trainCellsLabels;
	vector<int> testCellsLabels;

	// outputfile
	outputFile.open("C:\\VI\\VI-2018-19-KinectPackage\\VI-2018-KinectPackage\\ObjectDetectionOutput.txt", std::ios::out);

	// load train variables
	labelpathName = "C:\\VI\\VI-2018-19-KinectPackage\\VI-2018-KinectPackage\\bksey\\Set1Labels.txt";
	//labelpathName = "C:\\Users\\nirup\\Desktop\\Shalini Study Reading University\\VI\\Kinect\\Set1Labels.txt";
	loadLabels(labelpathName, trainLabels);
	//cout << "TrainSetLabels : " << endl;

        // load test variables
	labelpathName = "C:\\VI\\VI-2018-19-KinectPackage\\VI-2018-KinectPackage\\bksey\\Set2Labels.txt";
	//labelpathName = "C:\\Users\\nirup\\Desktop\\Shalini Study Reading University\\VI\\Kinect\\Set2Labels.txt";
	loadLabels(labelpathName, testLabels);
	//cout << "TestSetLabels : " << endl;
	
	// Training Data Sets

        // Build Training Data Set
	pathName = "C:\\VI\\VI-2018-19-KinectPackage\\VI-2018-KinectPackage\\Set1\\Set1";
	loadTrainLabel(pathName, trainLabels, trainCells, trainCellsLabels);
	cout << "Training Data Build : " << endl;
	outputFile << "Training Data Build : " << endl;
	
	return 0;
}
