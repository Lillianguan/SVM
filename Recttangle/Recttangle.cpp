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

	map<string, vector<Plate>> map = trainSVM::getInstance()->process(possible_regions);

	Mat img = imread(imagePath);
	trainSVM::getInstance()->fsOperatorAndDraw(img, possible_regions, "test");

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
}

void classifyImage()
{
	StampRect* obj = new StampRect(true, false);
	obj->classificationImage();
	cout << "classify all the plates......" << endl;
	cout << "CalssificationImage are done!!" << endl;
}

void findRegions()
{
	vector<string> files;
	string formattif = ".bmp";
	FileOperation::GetAllFormatFiles(path, files, formattif);
	cout << "Number" << files.size() << endl;
	int imageNumb = files.size();
	for (int i = 0; i < imageNumb; i++)
	{
		stringstream ss(stringstream::in | stringstream::out);
		ss << files[i];
		string filename = FileOperation::getFilename(files[i]);
		Mat image = imread(files[i]);
		StampRect* obj = new StampRect();
		vector<Plate> possible_regions = obj->run(files[i], 41, 30);
		delete obj;
		trainSVM::getInstance()->fsOperatorAndDraw(image, possible_regions, filename);
	}
	cout << "Done with find the region...." << endl;
}




