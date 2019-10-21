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

// Training/Test Cells
vector<Mat> trainCells;
vector<Mat> testCells;
vector<Mat> trainDepthCells;
vector<Mat> testDepthCells;
vector<string> trainLabels;
vector<string> testLabels;
Mat image;

//some boolean variables for different functionality within this program
bool evaluate = false; // evaluation

bool useCountour = true; // countour
bool edgeDetection = false;// edge detection

bool useThreshold = false;// thresholds
bool trackObjects = false; // track Objects
bool useMorphOps = false; // Erode and Dialtes

// files pathname
string pathName;
string labelpathName;

// open file
std::fstream outputFile;

// labels convertion
void switchLabel(string &line, int &labels) {
	
	if (line.compare("Baby") == 0) {
		labels = 1;
	}
	else if (line.compare("Dog") == 0) {
		labels = 2;
	}
	else if (line.compare("Dinosaur") == 0) {
		labels = 3;
	}
	else if (line.compare("Coffee Tin") == 0) {
		labels = 4;
	}
	else if (line.compare("Mug") == 0) {
		labels = 5;
	}
	else if (line.compare("Car") == 0) {
		labels = 6;
	}
	else if (line.compare("Camera") == 0) {
		labels = 7;
	}
	else if (line.compare("Keyboard") == 0) {
		labels = 8;
	}
	else if (line.compare("Koala") == 0) {
		labels = 9;
	}
	else if (line.compare("Blackberry") == 0) {
		labels = 10;
	}
	else if (line.compare("Diet Coke Bottle") == 0) {
		labels = 11;
	}
	else if (line.compare("Duck") == 0) {
		labels = 12;
	}
	else if (line.compare("Dragon") == 0) {
		labels = 13;
	}
	else if (line.compare("Android") == 0) {
		labels = 14;
	}
}

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

// Function header for threshold
void thresh_callback(int, void*);

void on_trackbar(int, void*)
{//This function gets called whenever a
	// trackbar position is changed
}

// Integer to String
string intToString(int number) {
	std::stringstream ss;
	ss << number;
	return ss.str();
}

// Trackbars
void createTrackbars() {

	//create window for trackbars
	namedWindow(trackbarWindowName, 0);

	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
//create trackbars and insert them into window
//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
//the max value the trackbar can move (eg. H_HIGH), 
//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
//                                  ---->    ---->     ---->      
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);
}

// draw crosshairs on your tracked image!
void drawObject(int x, int y, Mat &frame) {

//use some of the openCV drawing functions to draw crosshairs
//on your tracked image!

//UPDATE:JUNE 18TH, 2013
//added 'if' and 'else' statements to prevent
//memory errors from writing off the screen (ie. (-25,-25) is not within the window!)

	circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2);
	if (y - 25 > 0)
		line(frame, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
	else 
		line(frame, Point(x, y), Point(x, 0), Scalar(0, 255, 0), 2);
	
	if (y + 25 < FRAME_HEIGHT)
		line(frame, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
	else 
		line(frame, Point(x, y), Point(x, FRAME_HEIGHT), Scalar(0, 255, 0), 2);
	
	if (x - 25 > 0)
		line(frame, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
	else 
		line(frame, Point(x, y), Point(0, y), Scalar(0, 255, 0), 2);
	if (x + 25 < FRAME_WIDTH)
		line(frame, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
	else 
		line(frame, Point(x, y), Point(FRAME_WIDTH, y), Scalar(0, 255, 0), 2);

	putText(frame, intToString(x) + "," + intToString(y), Point(x, y + 30), 1, 1, Scalar(0, 255, 0), 2);

}

//HOGDescriptor hog(Size(640, 480), Size(16, 16), Size(8, 8), Size(8, 8), 9);
HOGDescriptor hog;

// Feacture Extractor
void CreateTrainTestHOG(vector<vector<float> > &traintestHOG, vector<Mat> &traintestCells, vector<Mat> &traintestDepthCells) {

	for (int y = 0; y < traintestCells.size(); y++) {
		vector<float> descriptors;
		vector<float> depthDescriptors;
		try {
			hog.compute(traintestCells[y], descriptors, Size(64, 128), Size(16, 16));
		}
		catch (std::bad_alloc& excepObj)
		{
			std::cout << "bad_alloc Exception :: Out Of Memory " << excepObj.what() << '\n';
		}
		//descriptors.insert(descriptors.end(), depthDescriptors.begin(), depthDescriptors.end());
		traintestHOG.push_back(descriptors);
	}
}

// Vector to Matrix
void ConvertVectortoMatrix(vector<vector<float> > &traintestHOG, Mat &traintestMat)
{
	int descriptor_size = traintestHOG[0].size();
	int traintestHOG_size = traintestHOG.size();
	cout << "traintestHOG : " << traintestHOG_size << endl;
	for (int i = 0; i < traintestHOG.size(); i++) {
		for (int j = 0; j < descriptor_size; j++) {
			traintestMat.at<float>(i, j) = traintestHOG[i][j];
			//cout << "Image Count : " << traintestMat.at<float>(i, j) << endl;
		}
	}
}

// Set SVM parameters
void getSVMParams(SVM *svm)
{
	cout << "Kernel type     : " << svm->getKernelType() << endl;
	cout << "Type            : " << svm->getType() << endl;
	//cout << "TermCriteria    : " << svm->getTermCriteria() << endl;
	cout << "C               : " << svm->getC() << endl;
	cout << "Degree          : " << svm->getDegree() << endl;
	cout << "Nu              : " << svm->getNu() << endl;
	cout << "Gamma           : " << svm->getGamma() << endl;
}

// SVM Train Model
void SVMtrain(Mat &trainMat, vector<int> &trainLabels, SVM *svm) {
	//Ptr<SVM> svm = SVM::create();
	//svm->setGamma(0.50625);
	svm->setGamma(0.01);
	svm->setC(10);
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-3));
	//svm->setC(100);
	svm->setKernel(SVM::LINEAR);
	svm->setType(SVM::C_SVC);
	getSVMParams(svm);
	Ptr<TrainData> td = TrainData::create(trainMat, ROW_SAMPLE, trainLabels);
	svm->train(td);

	//svm->trainAuto(td);
	svm->save("C:\\Users\\nirup\\Desktop\\Shalini Study Reading University\\VI\\Kinect\\objectrecognition.yml");
}

// SVM Evaluation
	void SVMevaluate(Mat &testResponse, float &count, float &accuracy, vector<int> &testLabels) {
		string response;
		int read = 1;

		for (int i = 0; i < testResponse.rows; i++)
		{
			//cout << testResponse.at<float>(i, 0) << " = " << testLabels[i] << endl;
			outputFile << testResponse.at<float>(i, 0) << " = " << testLabels[i] << endl;

			if (testResponse.at<float>(i, 0) == testLabels[i]) {
				count = count + 1;

			}
		}
		accuracy = (count / testResponse.rows) * 100;
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
		        
		  	// detect as background
			if (cnt == 0) {
				//if (imgCount < 10) {
					trainCellsLabels.push_back(0); // "Background"
					//trainCells.push_back(grayscale);
					trainCells.push_back(image);
					trainDepthCells.push_back(currentDepth);
				//}
				printLabel = 1;
				imgCount++;
			}
			else {
				// atleast 25 percentage of object
				if (cnt > 10000) {
					if (printLabel == 1) {
						readFrame++;
						try {
							cout << "labels : " << labels[readFrame] << endl;
						}
						catch (std::bad_alloc& excepObj)
						{
							std::cout << "bad_alloc Exception :: OUt Of Memory " << excepObj.what() << '\n';
						}
						printLabel = 0;
					}
					//cout << "trainCellsLabels : " << readFrame << endl;
					trainCellsLabels.push_back(readFrame); // "Background"
					//trainCells.push_back(grayscale);
					trainCells.push_back(image);
					trainDepthCells.push_back(currentDepth);
				}
			}
	}
}

// load image and build test data/labels sets
	void loadTestLabel(string &pathName, vector<string> &labels, vector<Mat> &testCells, vector<int> &testCellsLabels, SVM *svm)
	{
		// variable declaration
		Mat currentRGB;
		Mat currentDepth;
		Mat HSV; //matrix storage for HSV image
		Mat threshold; //matrix storage for binary threshold image
		int readFrame = 0;
		int printLabel = 0;
		int ImgCount = 0;
		int label = 0;

		//x and y values for the location of the object
		int x = 0, y = 0;

		//create slider bars for HSV filtering
		//createTrackbars();

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
			//imshow("RGB", currentRGB);
			//imshow("Depth", currentDepth);

			int cnt = 0;
			Scalar intensity;
			for (int j = 0; j < currentDepth.rows; j++)
			{
				for (int i = 0; i < currentDepth.cols; i++)
				{
					intensity = currentDepth.at<uchar>(j, i);
					//cout << "intensity1 : " << intensity.val[0] << endl;
					if (intensity.val[0] != 95 && intensity.val[0] != 96 && intensity.val[0] != 97
						&& intensity.val[0] != 98 && intensity.val[0] != 99 && intensity.val[0] != 255
						)
					{
						cnt++;
						//cout << "intensity : " << intensity.val[0] << endl;
					}
				}
			}

			if (cnt == 0) {

				// Test Build set for each complete object
				if (evaluate) {
					
					// train hog
					std::vector<std::vector<float> > testHOG;
					CreateTrainTestHOG(testHOG, testCells, testDepthCells);
					cout << "CreateTestHOG : " << endl;

					int descriptor_size = testHOG[0].size();
					cout << "Descriptor Size : " << descriptor_size << endl;

					Mat testMat(testHOG.size(), descriptor_size, CV_32FC1);

					ConvertVectortoMatrix(testHOG, testMat);
					cout << "ConvertVectortoMatrix : " << endl;

					// Prediction
					Mat testResponse;
					svm->predict(testMat, testResponse);
					cout << "Prediction : " << endl;
					cout << " test response" << testResponse.size() << endl;

					// Evaluate Accuracy
					float count = 0;
					float accuracy = 0;
					SVMevaluate(testResponse, count, accuracy, testCellsLabels);
					cout << "Accuracy        : " << accuracy << "%" << endl;
					outputFile << "Accuracy        : " << accuracy << "%" << endl;

					// clear data sets
					testCells.clear();
					testDepthCells.clear();
					testCellsLabels.clear();
					testResponse.release();

					// Stop evaluation
					evaluate = false;
				}

				// Build Test Data Sets
				testCellsLabels.push_back(0); // "Background"
				//testCells.push_back(grayscale);
				testCells.push_back(image);				
				testDepthCells.push_back(currentDepth);
				printLabel = 1;

			}
			else {
				// atleast 25 percentage of object
				if (cnt > 10000) {
					if (printLabel == 1) {
						readFrame++;
						// read correct label No based on train label
						string line = labels[readFrame];
						switchLabel(line, label);
						cout << "labels : " << line << endl;
						cout << "Actual label : " << label << endl;
						printLabel = 0;
						evaluate = true;
					}
					//cout << "testCellsLabels : " << readFrame << endl;
					testCellsLabels.push_back(label); // "Background"
					//testCells.push_back(grayscale);
					testCells.push_back(image);
					testDepthCells.push_back(currentDepth);
				}
			}

			// Check for keyboard input
			key = cv::waitKey(10);
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
	
	// Train hog
	vector<vector<float> > trainHOG;
	vector<vector<float> > testHOG;
	CreateTrainTestHOG(trainHOG, trainCells, trainDepthCells);
	cout << "CreateTrainHOG : " << endl;
	outputFile << "CreateTrainHOG : " << endl;
	
	// Descriptor Size
	int descriptor_size = trainHOG[0].size();
	cout << "Descriptor Size : " << descriptor_size << endl;
	outputFile << "Descriptor Size : " << descriptor_size << endl;

	// Training Matrix
	Mat trainMat(trainHOG.size(), descriptor_size, CV_32FC1);
	
	// Convert Vector to Matrix
	ConvertVectortoMatrix(trainHOG, trainMat);
	cout << "ConvertTrainVectortoMatrix : " << endl;
	outputFile << "ConvertTrainVectortoMatrix : " << endl;
	
	// Train SVM
	Ptr<SVM> svm = SVM::create();
	SVMtrain(trainMat, trainCellsLabels, svm);
	cout << "Train SVM : " << endl;
	outputFile << "Train SVM : " << endl;
	
	// Test Data Sets

    // Build Test Data Set
	pathName = "C:\\VI\\VI-2018-19-KinectPackage\\VI-2018-KinectPackage\\Set2\\Set2";
	loadTestLabel(pathName, testLabels, testCells, testCellsLabels, svm);

	cout << "Test Data Build : " << endl;
	outputFile << "Test Data Build : " << endl;
	
	return 0;
}
