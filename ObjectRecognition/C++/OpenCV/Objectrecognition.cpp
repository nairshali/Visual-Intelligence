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
