#include "StdAfx.h"
#include "CheckRect.h"


CheckRect::CheckRect(void)
{
}


CheckRect::~CheckRect(void)
{
}

Mat CheckRect::loadimage(string path)
{
	myimage = imread(path, 1);
	return myimage;
}


bool CheckRect::isRectangle(vector<cv::Point2f>& rectanglePoints)
{
	// check the validity of transformed rectangle shape
	// the sign of outer products of each edge vector must be the same
	bool returnThis = true;

	if (rectanglePoints.size() == 4)
	{
		float vector[4][2];
		int i;

		vector[0][0] = rectanglePoints[1].x - rectanglePoints[0].x;
		vector[0][1] = rectanglePoints[1].y - rectanglePoints[0].y;
		vector[1][0] = rectanglePoints[2].x - rectanglePoints[1].x;
		vector[1][1] = rectanglePoints[2].y - rectanglePoints[1].y;
		vector[2][0] = rectanglePoints[3].x - rectanglePoints[2].x;
		vector[2][1] = rectanglePoints[3].y - rectanglePoints[2].y;
		vector[3][0] = rectanglePoints[0].x - rectanglePoints[3].x;
		vector[3][1] = rectanglePoints[0].y - rectanglePoints[3].y;

		int multiplicator;
		float product = vector[3][0] * vector[0][1] - vector[3][1] * vector[0][0];
		if (product > 0)
		{
			multiplicator = 1;
		}
		else
		{
			multiplicator = -1;
		}

		for (i = 0; i < 3; i++)
		{
			product = vector[i][0] * vector[i + 1][1] - vector[i][1] * vector[i + 1][0];
			if (product * multiplicator <= 0)
			{
				returnThis = false;
				break;
			}
		}
	}
	else
	{
		returnThis = false;
	}

	return returnThis;
}

CvRect CheckRect::identifyRect(Mat& image, int mod, vector<cv::Point2f> p)
{
	Mat test = image.clone();
	drawContour(test, p, cv::Scalar(0, 255, 0));
	bool is = isRectangle(p);
	cout << is << endl;

	bool bigger = false;
	bool middle = false;
	bool smaller = false;
	switch (mod)
	{
	case 0:
		bigger = true;
		break;
	case 1:
		middle = true;
		break;
	case 2:
		smaller = true;
		break;
	default:
		break;;
	}

	if (bigger) //Rect as big as possible
	{
		if (p[0].y != p[3].y)
		{
			p[0].y = MIN(p[0].y,p[3].y);
			p[3].y = p[0].y;
		}
		else { cout << "This is a linear line ad" << endl; }

		if (p[1].y != p[2].y)
		{
			p[1].y = MAX(p[1].y , p[2].y);
			p[2].y = p[1].y;
		}
		else { cout << "This is a linear line bc" << endl; }

		if (p[0].x != p[1].x)
		{
			p[0].x = MIN(p[0].x , p[1].x);
			p[1].x = p[0].x;
		}
		else { cout << "This is a linear line ab" << endl; }

		if (p[2].x != p[3].x)
		{
			p[2].x = MAX(p[2].x ,p[3].x);
			p[3].x = p[2].x;
		}
		else { cout << "This is a linear line cd" << endl; }
	}

	if (middle) //Rect middle 
	{
		if (p[0].y != p[3].y)
		{
			p[0].y = (p[0].y + p[3].y) / 2;
			p[3].y = p[0].y;
		}
		else { cout << "This is a linear line ad" << endl; }

		if (p[1].y != p[2].y)
		{
			p[1].y = (p[1].y + p[2].y) / 2;
			p[2].y = p[1].y;
		}
		else { cout << "This is a linear line bc" << endl; }

		if (p[0].x != p[1].x)
		{
			p[0].x = (p[0].x + p[1].x) / 2;
			p[1].x = p[0].x;
		}
		else { cout << "This is a linear line ab" << endl; }

		if (p[2].x != p[3].x)
		{
			p[2].x = (p[2].x + p[3].x) / 2;
			p[3].x = p[2].x;
		}
		else { cout << "This is a linear line cd" << endl; }
	}
	if (smaller) //Rect as smaller as possible
	{
		if (p[0].y != p[3].y)
		{
			p[0].y = MAX(p[0].y,p[3].y);
			p[3].y = p[0].y;
		}
		else { cout << "This is a linear line ad" << endl; }

		if (p[1].y != p[2].y)
		{
			p[1].y = MIN(p[1].y , p[2].y);
			p[2].y = p[1].y;
		}
		else { cout << "This is a linear line bc" << endl; }

		if (p[0].x != p[1].x)
		{
			p[0].x = MAX(p[0].x , p[1].x);
			p[1].x = p[0].x;
		}
		else { cout << "This is a linear line ab" << endl; }

		if (p[2].x != p[3].x)
		{
			p[2].x = MIN(p[2].x ,p[3].x);
			p[3].x = p[2].x;
		}
		else { cout << "This is a linear line cd" << endl; }
	}


	drawContour(test, p, cv::Scalar(0, 0, 0));
	namedWindow("Test", CV_WINDOW_NORMAL);
	if (!test.empty()) { imshow("Test", test); }
	else { cout << "There is no picture" << endl; }
	cvWaitKey(0);
	return cvRect(p[0].x, p[0].y, p[3].x - p[0].x, p[1].y - p[0].y);
}

void CheckRect::drawContour(cv::Mat& image, std::vector<cv::Point2f> points2d, cv::Scalar color, int thickness, int lineType, int shift)
{
	// for all points
	for (size_t i = 0; i < points2d.size(); i++)
	{
		// rescale point a coordinates
		cv::Point2f a;
		a.x = (int) ((points2d[i].x) + 0.5);
		a.y = (int) ((points2d[i].y) + 0.5);

		// resale point b coordinates
		cv::Point2f b;
		b.x = (int) ((points2d[(i + 1) % points2d.size()].x) + 0.5);
		b.y = (int) ((points2d[(i + 1) % points2d.size()].y) + 0.5);

		// draw line
		cv::line(image, a, b, color, thickness, lineType, shift);
	}
}

Mat CheckRect::editRect(Mat& image, CvRect rect)
{
	Mat holdimage = image.clone();
	Mat imageROI = holdimage(rect);
	image(rect) = Scalar(255, 255, 255);
	namedWindow("Test1", CV_WINDOW_NORMAL);
	if (!image.empty()) { imshow("Test1", image); }
	else { cout << "No image hier!!" << endl; }
	cvWaitKey(0);
	return imageROI;
}

void CheckRect::writeImage(Mat& image, string path)
{
	imwrite(path, image);
}

