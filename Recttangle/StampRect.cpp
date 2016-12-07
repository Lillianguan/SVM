#include "StdAfx.h"
#pragma once
#include "StampRect.h"

StampRect::StampRect(bool saveRegions, bool showSteps): numPlates(0)
{
	//Set Path
	lern_path = "lernPlates\\";
	_plates_Typnum = FileOperation::GetDirNumber(lern_path);
	cout << _plates_Typnum << endl;
	for (int i = 0; i < _plates_Typnum; i++)
	{
		stringstream ss(stringstream::in | stringstream::out);
		ss << lern_path << "Plates" << i;
		path_Plates.push_back(ss.str());
	}

	_saveRegions = saveRegions;
	_showSteps = showSteps;
}

StampRect::~StampRect(void)
{
}

vector<Plate> StampRect::run(string imagePath, int x, int y) const
{
	// read image
	auto input = imread(imagePath);

	//get filename 
	string filename = FileOperation::getFilename(imagePath);

	vector<Plate> output;
	//convert image to gray
	Mat img_gray;
	cvtColor(input, img_gray, CV_BGR2GRAY);
	blur(img_gray, img_gray, Size(5, 5));

	//Finde vertical lines. Car plates have high density of vertical lines
	Mat img_sobel;
	Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	if (_showSteps)
	{
		namedWindow("Sobel", CV_WINDOW_NORMAL);
		imshow("Sobel", img_sobel);
		waitKey(0);
	}

	//threshold image
	Mat img_threshold;
	threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

	if (_showSteps)
	{
		namedWindow("Threshold", CV_WINDOW_NORMAL);
		imshow("Threshold", img_threshold);
		waitKey(0);
	}

	//Morphplogic operation close
	Mat close_1, close_2;
	Mat element_1 = getStructuringElement(MORPH_RECT, Size(x, y));
	morphologyEx(img_threshold, close_1, CV_MOP_CLOSE, element_1);
	if (_showSteps)
	{
		namedWindow("Close_1", CV_WINDOW_NORMAL);
		imshow("Close_1", close_1);
		waitKey(0);
	}

	//Find contours of possibles plates
	vector<vector<Point>> contours_1, contours_2;
	findContours(close_1,
	             contours_1, // a vector of contours
	             CV_RETR_EXTERNAL, // retrieve the external contours
	             CV_CHAIN_APPROX_NONE); // all pixels of each contours

	//Start to iterate to each contour founded
	vector<vector<Point>>::iterator itc_1 = contours_1.begin();
	vector<RotatedRect> rects;

	//Remove patch that are no inside limits of aspect ratio and area.    
	while (itc_1 != contours_1.end())
	{
		//Create bounding rect of object
		RotatedRect mr = minAreaRect(Mat(*itc_1));
		if (!verifySizes(mr))
		{
			itc_1 = contours_1.erase(itc_1);
		}
		else
		{
			++itc_1;
			rects.push_back(mr);
		}
	}

	// Draw blue contours on a white image
	Mat result;
	input.copyTo(result);
	drawContours(result, contours_1, -1, Scalar(255, 0, 0), 10); // in blue

	for (int i = 0; i < rects.size(); i++)
	{
		if (verifySizes(rects[i]))
		{
			// rotated rectangle drawing 
			Point2f rect_points[4];
			rects[i].points(rect_points);
			for (int j = 0; j < 4; j++)
			{
				line(result, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 0, 255), 1, 8);
			}

			//Get rotation matrix
			float r = static_cast<float>(rects[i].size.width) / static_cast<float>(rects[i].size.height);
			float angle = rects[i].angle;
			if (r < 1)
			{
				angle = 90 + angle;
			}
			Mat rotmat = getRotationMatrix2D(rects[i].center, angle, 1);

			//Create and rotate image
			Mat img_rotated;
			warpAffine(input, img_rotated, rotmat, input.size(), CV_INTER_CUBIC);

			//Crop image
			Size rect_size = rects[i].size;
			if (r < 1)
			{
				swap(rect_size.width, rect_size.height);
			}
			Mat img_crop;
			getRectSubPix(img_rotated, rect_size, rects[i].center, img_crop);

			Mat resultResized;
			resultResized.create(33, 144, CV_8UC3);
			resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);

			//Equalize croped image
			Mat grayResult;
			cvtColor(resultResized, grayResult, CV_BGR2GRAY);
			blur(grayResult, grayResult, Size(3, 3));
			namedWindow("grayResult", CV_WINDOW_NORMAL);
			imshow("grayResult", grayResult);

			cout << "There is your option:" << endl;
			cout << "-------" << "0------" << "Noplates..." << endl;
			cout << "-------" << "1------" << "Plates_Stamp...." << endl;
			cout << "-------" << "2------" << "Plates_Mut...." << endl;
			cout << "-------" << "3------" << "Plates_Barcode...." << endl;

			char option = waitKey(0);
			if (_saveRegions)
			{
				//TODO automate this: display image and push 1,2,3,0
				stringstream ss(stringstream::in | stringstream::out);
				ss << lern_path << "plates" << option << "\\" << filename << "_" << x << "_" << y << i << ".jpg";
				cout << ss.str() << endl;
				imwrite(ss.str(), grayResult);
			}
			output.push_back(Plate(grayResult, rects[i].boundingRect()));
		}
	}

	if (_showSteps)
	{
		namedWindow("Contours", CV_WINDOW_NORMAL);
		imshow("Contours", result);
		waitKey(0);
	}
	return output;
}

Mat StampRect::histeq(Mat in) const
{
	Mat out(in.size(), in.type());
	if (in.channels() == 3)
	{
		Mat hsv;
		vector<Mat> hsvSplit;
		cvtColor(in, hsv, CV_BGR2HSV);
		split(hsv, hsvSplit);
		equalizeHist(hsvSplit[2], hsvSplit[2]);
		merge(hsvSplit, hsv);
		cvtColor(hsv, out, CV_HSV2BGR);
	}
	else if (in.channels() == 1)
	{
		equalizeHist(in, out);
	}

	return out;
}

bool StampRect::verifySizes(RotatedRect mr)
{
	bool identify;
	float error = 0.3;
	float aspect = 3.8;
	int min = 70 * aspect * 70; // minimum area
	int max = 160 * aspect * 160; // maximum area
	float rmin = aspect - aspect * error;
	float rmax = aspect + aspect * error;

	int area = mr.size.height * mr.size.width;
	cout << "Area Size:" << area << endl;
	float r = static_cast<float>(mr.size.width) / static_cast<float>(mr.size.height);
	if (r < 1)
	{
		r = static_cast<float>(mr.size.height) / static_cast<float>(mr.size.width);
	}

	if (area < min || area > max || (r < rmin || r > rmax))
	{
		identify = false;
	}
	else
	{
		identify = true;
	}
	cout << "identify :" << identify << endl;
	return identify;
}

void StampRect::classificationImage()
{
	Mat classes;//(numPlates+numNoPlates, 1, CV_32FC1);
	Mat trainingData;//(numPlates+numNoPlates, imageWidth*imageHeight, CV_32FC1 );
	Mat trainingImages;
	vector<int> trainingLabels;
	vector<string> file;
	string format = ".jpg";
	for (int i = 0; i < _plates_Typnum; i++)
	{
		FileOperation::GetAllFormatFiles(path_Plates[i], file, format);
		numPlates = file.size();
		cout << numPlates << endl;
		for (int j = 0; j < numPlates; j++)
		{
			stringstream ss(stringstream::in | stringstream::out);
			ss << file[j];
			cout << ss.str() << endl;
			Mat img = imread(ss.str(), 0);
			if (img.empty())
			{
				cout << "There ist no picture here!" << endl;
			}
			img = img.reshape(1, 1);
			trainingImages.push_back(img);
			trainingLabels.push_back(i);
		}
		file.erase(file.begin(), file.end());
	}

	Mat(trainingImages).copyTo(trainingData);
	trainingData.convertTo(trainingData, CV_32FC1);
	Mat(trainingLabels).copyTo(classes);

	FileStorage fs("SVM.xml", FileStorage::WRITE);
	fs << "TrainingData" << trainingData;
	fs << "classes" << classes;
	fs.release();
}

