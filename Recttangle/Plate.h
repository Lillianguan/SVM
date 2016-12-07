
#ifndef Plate_h
#define Plate_h

#include <string.h>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

class Plate{
    public:
        Plate();
        Plate(Mat img, Rect pos);
        Rect position;
        Mat plateImg;
        vector<char> chars;
        vector<Rect> charsPos;        
};

#endif
