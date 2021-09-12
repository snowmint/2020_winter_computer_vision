#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include<opencv2/highgui/highgui_c.h>
#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

Mat src; Mat src_gray;
Mat src2, src_gray2;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);
int threshold_value = 0;
int threshold_type = 3;;
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;
/// Function header
void thresh_callback(int, void*);
void thresh_callback_hu(int, void*);

/** @function main */
int main(int argc, char** argv)
{
	/// Load source image and convert it to gray
	//src = imread("monster.jpg");

	/// Convert image to gray and blur it
	//cvtColor(src, src_gray, CV_BGR2GRAY);
	//blur(src_gray, src_gray, Size(3, 3));

	/// Create Window
	//namedWindow("Source", CV_WINDOW_AUTOSIZE);
	//imshow("Source", src);

	//createTrackbar(" Canny thresh:", "Source", &thresh, max_thresh, thresh_callback_hu);
	//thresh_callback(0, 0);

	src2 = imread("img225.png");
	
	/// Convert image to gray and blur it
	cvtColor(src2, src_gray2, CV_BGR2GRAY);
	blur(src_gray2, src_gray2, Size(3, 3));
	threshold(src_gray2, src_gray2, 0, 255, THRESH_OTSU);
	
	//threshold( src_gray, dst, threshold_value, max_BINARY_value,threshold_type );
	/// Create Window
	namedWindow("Source2", CV_WINDOW_AUTOSIZE);
	imshow("Source2", src2);

	createTrackbar(" Canny thresh:", "Source2", &thresh, max_thresh, thresh_callback_hu);
	thresh_callback_hu(0, 0);

	waitKey(0);
	return(0);
}

/** @function thresh_callback */
//void thresh_callback(int, void*)
//{
//	Mat canny_output;
//	vector<vector<Point> > contours;
//	vector<Vec4i> hierarchy;
//
//	/// Detect edges using canny
//	Canny(src_gray, canny_output, thresh, thresh * 2, 3);
//	/// Find contours
//	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
//
//	/// Get the moments
//	vector<Moments> mu(contours.size());
//	for (int i = 0; i < contours.size(); i++)
//	{
//		mu[i] = moments(contours[i], false);
//	}
//
//	///  Get the mass centers:
//	vector<Point2f> mc(contours.size());
//	for (int i = 0; i < contours.size(); i++)
//	{
//		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
//	}
//
//	/// Draw contours
//	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
//	for (int i = 0; i< contours.size(); i++)
//	{
//		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
//		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
//		circle(drawing, mc[i], 4, color, -1, 8, 0);
//	}
//
//	/// Show in a window
//	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
//	imshow("Contours", drawing);
//
//	/// Calculate the area with the moments 00 and compare with the result of the OpenCV function
//	printf("\t Info: Area and Contour Length \n");
//	for (int i = 0; i< contours.size(); i++)
//	{
//		printf(" * Contour[%d] - Area (M_00) = %.2f - Area OpenCV: %.2f - Length: %.2f \n", i, mu[i].m00, contourArea(contours[i]), arcLength(contours[i], true));
//		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
//		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
//		circle(drawing, mc[i], 4, color, -1, 8, 0);
//	}
//}

void thresh_callback_hu(int, void*)
{
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using canny
	Canny(src_gray2, canny_output, thresh, thresh * 2, 3);
	imshow("Source1", src_gray2);
	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Get the moments
	vector<Moments> mu(contours.size());
	
	for (int i = 0; i < contours.size(); i++)
	{
		cout <<"["<< i << "] : =========================================" << endl;
		Moments moment = moments(contours[i], false);
		double huMoments[7];
		HuMoments(moment, huMoments);
		// Log scale hu moments
		for (int i = 0; i < 7; i++)
		{
			huMoments[i] = -1 * copysign(1.0, huMoments[i]) * log10(abs(huMoments[i]));
			cout << " | HuMoment " << i << " : " << huMoments[i] << "| ";
			
		}
		cout << endl;
		
		mu[i] = moments(contours[i], false);
	}
	cout << "=========================================" << endl;
	///  Get the mass centers:
	vector<Point2f> mc(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}

	/// Draw contours
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		cout << "color [" << i << "] : " << color << endl;
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		circle(drawing, mc[i], 4, color, -1, 8, 0);
	}

	/// Show in a window
	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", drawing);

	/// Calculate the area with the moments 00 and compare with the result of the OpenCV function
	printf("\t Info: Area and Contour Length \n");
	for (int i = 0; i< contours.size(); i++)
	{
		printf(" * Contour[%d] - Area (M_00) = %.2f - Area OpenCV: %.2f - Length: %.2f \n", i, mu[i].m00, contourArea(contours[i]), arcLength(contours[i], true));
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		circle(drawing, mc[i], 4, color, -1, 8, 0);
	}
}