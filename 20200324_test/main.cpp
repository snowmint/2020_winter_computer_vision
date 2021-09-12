#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <windows.h>
#include <math.h>
using namespace std;
using namespace cv;
double alpha; /**< Simple contrast control */
int beta;  /**< Simple brightness control */

Mat& ScanImageAndReduceRandomAccess(Mat& I, const uchar* const table)
{
// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);
	const int channels = I.channels();
	switch (channels)
	{
	case 1:
	{
		for (int i = 0; i < I.rows; ++i)
			for (int j = 0; j < I.cols; ++j)
				I.at<uchar>(i, j) = table[I.at<uchar>(i, j)];
		break;
	}
	case 3:
	{
		Mat_<Vec3b> _I = I;
		for (int i = 0; i < I.rows; ++i)
			for (int j = 0; j < I.cols; ++j)
			{
				_I(i, j)[0] = table[_I(i, j)[0]];
				_I(i, j)[1] = table[_I(i, j)[1]];
				_I(i, j)[2] = table[_I(i, j)[2]];
			}
		I = _I;
		break;
	}
	}
	
	return I;
}

Mat& ScanImageAndReduceC(Mat& I, const uchar* const table)
{
	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);
	int channels = I.channels();
	int nRows = I.rows;
	int nCols = I.cols * channels;
	if (I.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	int i, j;
	uchar* p;
	for (i = 0; i < nRows; ++i)
	{
		p = I.ptr<uchar>(i);
		for (j = 0; j < nCols; ++j)
		{
			p[j] = table[p[j]];
		}
	}
	return I;
}
void gammaCorrection(const Mat &img, const double gamma, int num)
{
	CV_Assert(gamma >= 0);
	//! [changing-contrast-brightness-gamma-correction]
	Mat lookUpTable(1, 256, CV_8U);
	uchar* p = lookUpTable.ptr();
	for (int i = 0; i < 256; ++i)
		p[i] = saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);

	Mat res = img.clone();
	LUT(img, lookUpTable, res);
	//! [changing-contrast-brightness-gamma-correction]
	Mat img_gamma_corrected;
	hconcat(img, res, img_gamma_corrected);
	string named = "Gamma correction"+num;
	imshow(named, img_gamma_corrected);
}

int main()
{
	int divideWith = 255; // convert our input string to number - C++ style

	double t = (double)getTickCount();
	Mat image = imread("building.jpg");
	imshow("image", image);

	Mat image2 = imread("unnamed.jpg");
	imshow("image2", image2);

	gammaCorrection(image, 0.2, 1);
	gammaCorrection(image2, 3.0, 2);

	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "Times passed in seconds: " << t << endl;

	waitKey();
	return 0;
}