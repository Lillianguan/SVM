#include "StdAfx.h"
#include "trainSVM.h"


trainSVM::trainSVM(bool saveImages)
{
	//Set SVM params
	SVM_params.svm_type = CvSVM::C_SVC;
	SVM_params.kernel_type = CvSVM::LINEAR; 
	SVM_params.degree = 2;
	SVM_params.gamma = 1;
	SVM_params.coef0 = 0;
	SVM_params.C = 1;
	SVM_params.nu = 0;
	SVM_params.p = 0;
	SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 1000, 0.01);

	_saveImages = saveImages;
}

trainSVM::~trainSVM(void)
{
}

map<string, vector<Plate>> trainSVM::process(vector<Plate>& posible_regions)
{
	//read learned data
	fs.open("SVM.xml", FileStorage::READ);
	fs["TrainingData"] >> SVM_TrainingData;
	fs["classes"] >> SVM_Classes;
	fs.release();

	//classification
	CvSVM svmClassifier(SVM_TrainingData, SVM_Classes, Mat(), Mat(), SVM_params);

	//map to vector
	map<string, vector<Plate>> regions;
	for (auto i = 0; i < posible_regions.size(); i++)
	{
		auto img = posible_regions[i].plateImg;
		auto p = img.reshape(1, 1);
		p.convertTo(p, CV_32FC1);
		auto response = static_cast<int>(svmClassifier.predict(p, false));
		if (response != 0)
		{
			stringstream ss;
			ss << response;
			cout << "Found Type:" << response << endl;
			regions["Type" + ss.str()].push_back(posible_regions[i]);
			float decision = svmClassifier.predict(p, true);
			float confidence = 1.0 / (1.0 + exp(-decision));
			cout << "Confidence:" << confidence << endl;
		}
	}

	return regions;
}

void trainSVM::fsOperatorAndDraw(string img_path, vector<Plate>& posible_regions)
{

	//SVM date to read
	fs.open("SVM.xml", FileStorage::READ);
	fs["TrainingData"] >> SVM_TrainingData;
	fs["classes"] >> SVM_Classes;

	CvSVM svmClassifier(SVM_TrainingData, SVM_Classes, Mat(), Mat(), SVM_params);

	//read image
	Mat input_image = imread(img_path);

	//get the filename 
	filename = FileOperation::getFilename(img_path);

	//map the possible regions to vector
	map<string, vector<Plate>> regions;
	for (auto i = 0; i < posible_regions.size(); i++)
	{
		auto img = posible_regions[i].plateImg;
		auto p = img.reshape(1, 1);
		p.convertTo(p, CV_32FC1);
		auto response = static_cast<int>(svmClassifier.predict(p, false));
		if (response != 0)
		{
			stringstream ss;
			ss << response;
			cout << "================================================\n";
			//drow the possible regions in grün
			rectangle(input_image, posible_regions[i].position, Scalar(0, 255, 0), 5, 8);
			//show the tpye of regions
			putText(input_image, "Type" + ss.str(), Point(posible_regions[i].position.x, posible_regions[i].position.y),
			        CV_FONT_HERSHEY_SIMPLEX,
			        1,
			        Scalar(0, 0, 200), 2);
			cout << "Found Type:" << response << endl;
			regions["Type" + ss.str()].push_back(posible_regions[i]);
		}
	}

	if (_saveImages)
	{
		stringstream ss(stringstream::in | stringstream::out);
		ss << "ResultImages\\" << filename << "_" << ".jpg";
		imwrite(ss.str(), input_image);
	}
	fs.release();
}

	
