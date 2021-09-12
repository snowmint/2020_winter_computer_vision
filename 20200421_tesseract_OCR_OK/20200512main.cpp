#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include "opencv2/opencv.hpp"
#include <iostream>
#include <windows.h>
#include <math.h>
#include <random>
#include <stdlib.h>

using namespace cv;
using namespace std;

int computeMedian(vector<int> elements)
{
	nth_element(elements.begin(), elements.begin() + elements.size() / 2, elements.end());
	//sort(elements.begin(),elements.end());
	return elements[elements.size() / 2];
}

Mat compute_median(std::vector<cv::Mat> vec) {
	// Note: Expects the image to be CV_8UC3
	Mat medianImg = vec[0].clone();

	for (int row = 0; row<vec[0].rows; row++) {
		for (int col = 0; col<vec[0].cols; col++) {
			vector<int> elements_B;
			vector<int> elements_G;
			vector<int> elements_R;

			for (int imgNumber = 0; imgNumber<vec.size(); imgNumber++)
			{
				int B = vec[imgNumber].ptr<Vec3b>(row)[col].val[0];
				int G = vec[imgNumber].ptr<Vec3b>(row)[col].val[1];
				int R = vec[imgNumber].ptr<Vec3b>(row)[col].val[2];

				elements_B.push_back(B);
				elements_G.push_back(G);
				elements_R.push_back(R);
			}
			medianImg.ptr<Vec3b>(row)[col].val[0] = computeMedian(elements_B);
			medianImg.ptr<Vec3b>(row)[col].val[1] = computeMedian(elements_G);
			medianImg.ptr<Vec3b>(row)[col].val[2] = computeMedian(elements_R);
		}
		cout << "keep waiting" << row << endl;
	}
	return medianImg;
}

int main() {
	//VideoCapture capture("demo2.mp4");
	Mat source_A, source_B;
	source_A = imread("lytro-03-A.jpg");
	source_B = imread("lytro-03-B.jpg");
	vector<Mat> frames;
	frames.push_back(source_A);
	frames.push_back(source_B);
	// Calculate the median along the time axis
	cout << "caculating..." << endl;
	Mat medianFrame = source_A.clone();
	medianFrame = compute_median(frames);
	
	// Display median frame
	imshow("background image", medianFrame);

	Mat grayMedianFrame;
	cvtColor(medianFrame, grayMedianFrame, COLOR_BGR2GRAY);
	imshow("grayMedianFrame image", grayMedianFrame);

	Mat gradA, gradB;
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;
	int ddepth = CV_16S;
	int scale = 1;
	int delta = 0;
	/// Gradient X
	Mat source_A_gray, source_B_gray;
	cvtColor(source_A, source_A_gray, COLOR_BGR2GRAY);
	cvtColor(source_B, source_B_gray, COLOR_BGR2GRAY);
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel(source_A_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);
	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(source_A_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, gradA);

	imshow("sobelA", gradA);
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel(source_B_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);
	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(source_B_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, gradB);
	imshow("sobelB", gradB);
	Mat result = source_A.clone();
	for (int row = 0; row < source_A.rows; row++) {
		for (int col = 0; col < source_A.cols; col++) {
			if (row >= 0 && row < 520) {
				if (col >= 0 && col < 520) {
					if (static_cast<int>(gradB.at<uchar>(row, col)) <= static_cast<int>(gradA.at<uchar>(row, col))) {
						result.ptr<Vec3b>(row)[col].val[0] = source_A.ptr<Vec3b>(row)[col].val[0];
						result.ptr<Vec3b>(row)[col].val[1] = source_A.ptr<Vec3b>(row)[col].val[1];
						result.ptr<Vec3b>(row)[col].val[2] = source_A.ptr<Vec3b>(row)[col].val[2];
					}
					else {
						result.ptr<Vec3b>(row)[col].val[0] = source_B.ptr<Vec3b>(row)[col].val[0];
						result.ptr<Vec3b>(row)[col].val[1] = source_B.ptr<Vec3b>(row)[col].val[1];
						result.ptr<Vec3b>(row)[col].val[2] = source_B.ptr<Vec3b>(row)[col].val[2];
					}
				}
			}
		}
	}
	imshow("result", result);

	waitKey(0);
	return 0;
}