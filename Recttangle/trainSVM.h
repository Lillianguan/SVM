#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <vector>
#include "Plate.h"
#include "FileOperation.h"



using namespace std;
using namespace cv;

class trainSVM
{
public:
	trainSVM(bool saveImages = false);
	~trainSVM(void);
	static trainSVM* getInstance();
	map<string, vector<Plate>> process(vector<Plate>& posible_regions);
	void fsOperatorAndDraw(Mat input_image, vector<Plate>& posible_regions, string filename);
	bool _saveImages;
private:
	static trainSVM * inst_;
	CvSVMParams SVM_params;
	Mat SVM_TrainingData;
	Mat SVM_Classes;
	FileStorage fs;
};

