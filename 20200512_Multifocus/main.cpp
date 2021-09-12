#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include "opencv2/opencv.hpp"
#include <iostream>
#include <windows.h>
#include <math.h>
#include <random>

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
	Mat medianImg(vec[0].rows, vec[0].cols, CV_8UC3, cv::Scalar(0, 0, 0));

	for (int row = 0; row<vec[0].rows; row++) {
		for (int col = 0; col<vec[0].cols; col++) {
			vector<int> elements_B;
			vector<int> elements_G;
			vector<int> elements_R;

			for (int imgNumber = 0; imgNumber<vec.size(); imgNumber++)
			{
				/*int B = vec[imgNumber].at<cv::Vec3b>(row, col)[0];
				int G = vec[imgNumber].at<cv::Vec3b>(row, col)[1];
				int R = vec[imgNumber].at<cv::Vec3b>(row, col)[2];*/
				int B = vec[imgNumber].ptr<Vec3b>(row)[col].val[0];
				int G = vec[imgNumber].ptr<Vec3b>(row)[col].val[1];
				int R = vec[imgNumber].ptr<Vec3b>(row)[col].val[2];

				elements_B.push_back(B);
				elements_G.push_back(G);
				elements_R.push_back(R);
			}
			/*medianImg.at<cv::Vec3b>(row, col)[0] = computeMedian(elements_B);
			medianImg.at<cv::Vec3b>(row, col)[1] = computeMedian(elements_G);
			medianImg.at<cv::Vec3b>(row, col)[2] = computeMedian(elements_R);*/
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

	imshow("A", source_A);
	imshow("B", source_B);
	//Size size(800, 800);
	//resize(source_A, source_A, size);
	//resize(source_B, source_B, size);
	/*cout << "Width1 : " << source_A.cols << endl;
	cout << "Height1: " << source_A.rows << endl;

	cout << "Width2 : " << source_B.cols << endl;
	cout << "Height2: " << source_B.rows << endl;
	vector<Mat> frames;
	frames.push_back(source_A);
	frames.push_back(source_B);*/
	
	

	// Calculate the median along the time axis
	//cout << "caculating..." << endl;
	//Mat medianFrame = compute_median(frames);
	//cout << "result" << endl;

	//cout << "Width3 : " << medianFrame.cols << endl;
	//cout << "Height3: " << medianFrame.rows << endl;
	// Display median frame
	//imshow("background image", medianFrame);

	//Mat grayMedianFrame;
	//cvtColor(medianFrame, grayMedianFrame, COLOR_BGR2GRAY);

	waitKey(0);
	return 0;
}