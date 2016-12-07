#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class CheckRect
{
public:
	CheckRect(void);
	~CheckRect(void);
    Mat loadimage(string path);
	static bool isRectangle(vector<cv::Point2f>& rectanglePoints);
	static CvRect identifyRect(Mat & image, int mod,vector<cv::Point2f> points2d);
	static void drawContour(cv::Mat &image, std::vector<cv::Point2f> points2d, cv::Scalar color, int thickness = 4,
                                int lineType = 8, int shift = 0);
	static Mat editRect(Mat &image,CvRect rect);
	static void writeImage(Mat& image,string path);
private:
	Mat myimage;
};

