#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <windows.h>
#include <math.h>
using namespace std;
using namespace cv;
double alpha; /**< Simple contrast control */
int beta;  /**< Simple brightness control */

uchar table[256];

void gammaCorrection(const Mat &img, const double gamma)
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
	imshow("Gamma correction", img_gamma_corrected);
}

int main()
{
	int divideWith = 255; // convert our input string to number - C++ style

	Mat img = imread("imginput.jpg");
	gammaCorrection(img, 0.5);
	gammaCorrection(img, 0.3);
}