// Recttangle.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <fstream>
#include "StampRect.h"
#include "trainSVM.h" 
#include "FileOperation.h"


static string path = "testimages\\2_b_10_0024692__1NEU.bmp";

void detectRegion();
void classifyImage();
void findRegions();
map<string, vector<Plate>> server(const char* imagePath);

int main()
{
	const char* imagePath = path.c_str();
	map<string, vector<Plate>> result = server(imagePath);

	//ImageItem item = findRegions();
	cout << "done.." << endl;
	string a;
	cin >> a;
	return 0;
}

map<string, vector<Plate>> server(const char* imagePath)
{
	StampRect* obj = new StampRect();
	vector<Plate> possible_regions = obj->run(imagePath, 41, 30);
	delete obj;

	trainSVM* train_svm = new trainSVM(true);
	map<string, vector<Plate>> map = train_svm->process(possible_regions);
	train_svm->fsOperatorAndDraw(imagePath, possible_regions);
	delete train_svm;

	return map;
}

void detectRegion()
{
	static string img_path = "SVMtrain\\TrainImage";
	StampRect* obj = new StampRect(true, false);
	obj->run(img_path, 41, 30);
	vector<string> files;
	vector<string> filestif;
	string format = ".bmp";
	string formattif = ".tif";
	FileOperation::GetAllFormatFiles(img_path, files, format);
	FileOperation::GetAllFormatFiles(img_path, filestif, formattif);

	for (int j = 0; j < filestif.size(); j++)
	{
		files.push_back(filestif[j]);
	}
	cout << "Number" << files.size() << endl;
	int imageNumb = files.size();
	for (int i = 0; i < imageNumb; i++)
	{
		stringstream ss(stringstream::in | stringstream::out);
		ss << files[i];
		obj->run(files[i], 41, 30);
	}
	delete obj;
}

void classifyImage()
{
	StampRect* obj = new StampRect(true, false);
	obj->classificationImage();
	delete obj;
	cout << "classify all the plates......" << endl;
	cout << "CalssificationImage are done!!" << endl;
}

void findRegions()
{
	StampRect* obj = new StampRect();
	trainSVM* train_svm = new trainSVM(true);
	vector<string> files;
	string formattif = ".bmp";
	FileOperation::GetAllFormatFiles(path, files, formattif);
	cout << "Number" << files.size() << endl;
	int imageNumb = files.size();
	for (int i = 0; i < imageNumb; i++)
	{
		stringstream ss(stringstream::in | stringstream::out);
		ss << files[i];
		vector<Plate> possible_regions = obj->run(files[i], 41, 30);
		train_svm->fsOperatorAndDraw(files[i], possible_regions);
	}
	delete obj;
	delete train_svm;
	cout << "Done with find the region...." << endl;
}




