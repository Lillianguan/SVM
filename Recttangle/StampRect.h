#pragma once

#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include "FileOperation.h"
#include "Plate.h"

using namespace cv;
using namespace std;

class StampRect
{
public:
	StampRect(bool saveRegions = false, bool showSteps = false);
	virtual ~StampRect(void);
    void classificationImage();
	vector<Plate> run(string imagePath,int x, int y) const;
private:
	bool _saveRegions;
	bool _showSteps;
	int plates_Typenum;
	string lern_path;
	vector<string> path_Plates;
	int numPlates;
	Mat image;
	Mat histeq(Mat in) const;
	static bool verifySizes(RotatedRect mr);
};

