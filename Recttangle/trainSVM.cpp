#include "StdAfx.h"
#include "trainSVM.h"

trainSVM* trainSVM::inst_ = nullptr;

trainSVM::trainSVM(bool saveImages)
{
	//Set SVM params
	SVM_params.svm_type = CvSVM::C_SVC;
	SVM_params.kernel_type = CvSVM::LINEAR; //CvSVM::LINEAR;
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

trainSVM* trainSVM::getInstance()
{
	if (inst_ == nullptr)
	{
		inst_ = new trainSVM();
	}
	return inst_;
}


std::map<std::string, vector<Plate>> trainSVM::process(vector<Plate>& posible_regions)
{
	// read learned data
	fs.open("SVM.xml", FileStorage::READ);
	fs["TrainingData"] >> SVM_TrainingData;
	fs["classes"] >> SVM_Classes;
	fs.release();

	// classification
	CvSVM svmClassifier(SVM_TrainingData, SVM_Classes, Mat(), Mat(), SVM_params);

	// map to vector
	std::map<std::string, vector<Plate>> regions;
	for (auto i = 0; i < posible_regions.size(); i++)
	{
		auto img = posible_regions[i].plateImg;
		auto p = img.reshape(1, 1);
		p.convertTo(p, CV_32FC1);
		auto response = static_cast<int>(svmClassifier.predict(p, false));
		cout << "Found Type:" << response << endl;
		regions["Type" + response].push_back(posible_regions[i]);
		float decision = svmClassifier.predict(p, true);
		float confidence = 1.0 / (1.0 + exp(-decision));
		cout << "Confidence:" << confidence << endl;
	}

	return regions;
}

void trainSVM::fsOperatorAndDraw(Mat input_image, vector<Plate>& posible_regions, string filename)
{
	//fs
	fs.open("SVM.xml", FileStorage::READ);
	fs["TrainingData"] >> SVM_TrainingData;
	fs["classes"] >> SVM_Classes;

	CvSVM svmClassifier(SVM_TrainingData, SVM_Classes, Mat(), Mat(), SVM_params);
	vector<Plate> plates1, plates2, plates3;
	for (int i = 0; i < posible_regions.size(); i++)
	{
		Mat img = posible_regions[i].plateImg;
		Mat p = img.reshape(1, 1);
		p.convertTo(p, CV_32FC1);
		int response = (int)svmClassifier.predict(p, false);
		if (response == 1)
			plates1.push_back(posible_regions[i]);
		if (response == 2)
			plates2.push_back(posible_regions[i]);
		if (response == 3)
			plates3.push_back(posible_regions[i]);


		
	}
	cout << "Num plates detected: " << plates1.size() << "\n";

	for (int i = 0; i < plates1.size(); i++)
	{
		Plate plate = plates1[i];
		cout << "================================================\n";
		cout << "================================================\n";
		rectangle(input_image, plate.position, Scalar(0, 255, 0), 5, 8); // grün
		if (false)
		{
			imshow("Plate Detected seg", plate.plateImg);
			cvWaitKey(0);
		}
	}
	for (int i = 0; i < plates2.size(); i++)
	{
		Plate plate = plates2[i];
		cout << "================================================\n";
		cout << "================================================\n";
		rectangle(input_image, plate.position, Scalar(0, 0, 255), 5, 8); // rot
		if (false)
		{
			imshow("Plate Detected seg", plate.plateImg);
			cvWaitKey(0);
		}
	}
	for (int i = 0; i < plates3.size(); i++)
	{
		Plate plate = plates3[i];
		cout << "================================================\n";
		cout << "================================================\n";
		rectangle(input_image, plate.position, Scalar(255, 0, 0), 5, 8); // blau
		if (false)
		{
			imshow("Plate Detected seg", plate.plateImg);
			cvWaitKey(0);
		}
	}

	if (true)
	{
		stringstream ss(stringstream::in | stringstream::out);
		ss << "ResultImages\\" << filename << "_" << ".jpg";
		imwrite(ss.str(), input_image);
	}
	fs.release();
}
