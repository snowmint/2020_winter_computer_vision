#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

vector<int> vec;
int count_point = 0;
bool bExit = false;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN && count_point < 4)
	{
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		vec.push_back(x);
		vec.push_back(y);
		count_point++;

		if (vec.size() == 8)
		{
			cout << "that's it" << endl;
			bool* exitflag = static_cast<bool*>(userdata);
			*exitflag = true;
		}
	}
	
	/*else if (event == EVENT_RBUTTONDOWN)
	{
		cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}*/
	/*else if (event == EVENT_MBUTTONDOWN)
	{
		cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}*/
	/*else if (event == EVENT_MOUSEMOVE)
	{
		cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
	}*/
}

int main()
{
	// Read image from file 
	Mat img = imread("pic.jpg");
	int nOffset = 200;
	

	//if fail to read the image
	if (img.empty())
	{
		cout << "Error loading the image" << endl;
		return -1;
	}

	//Create a window
	namedWindow("ImageDisplay", 1);
	imshow("ImageDisplay", img);


	setMouseCallback("ImageDisplay", CallBackFunc, static_cast<void*>(&bExit));

	while (!bExit)
	{
		waitKey(30);
	}

	cout << "Found " << "4" << " points... Exit" << endl;

	for (int i = 0; i<vec.size(); i++) {
		cout << vec[i] << endl;     // 輸出 0, 10, 20, 30, 40
	}
	if (img.empty()) return -1;
	// 設定變換[之前]與[之後]的坐標 (左上,左下,右下,右上)
	Point2f pts1[] = { Point2f(vec[0],vec[1]), Point2f(vec[2],vec[3]), Point2f(vec[4],vec[5]), Point2f(vec[6],vec[7]) };
	Point2f pts2[] = { Point2f(0,0), Point2f(0, nOffset), Point2f(nOffset, nOffset), Point2f(nOffset,0) };
	// 透視變換行列計算
	Mat perspective_matrix = cv::getPerspectiveTransform(pts1, pts2);
	Mat dst_img;
	// 變換
	warpPerspective(img, dst_img, perspective_matrix, img.size(), cv::INTER_LINEAR);
	// 繪製坐標變換[之前]與[之後]的示意圖
	line(img, pts1[0], pts1[1], cv::Scalar(255, 255, 0), 2, LINE_AA);
	line(img, pts1[1], pts1[2], cv::Scalar(255, 255, 0), 2, LINE_AA);
	line(img, pts1[2], pts1[3], cv::Scalar(255, 255, 0), 2, LINE_AA);
	line(img, pts1[3], pts1[0], cv::Scalar(255, 255, 0), 2, LINE_AA);
	line(img, pts2[0], pts2[1], cv::Scalar(255, 0, 255), 2, LINE_AA);
	line(img, pts2[1], pts2[2], cv::Scalar(255, 0, 255), 2, LINE_AA);
	line(img, pts2[2], pts2[3], cv::Scalar(255, 0, 255), 2, LINE_AA);
	line(img, pts2[3], pts2[0], cv::Scalar(255, 0, 255), 2, LINE_AA);

	Mat crop_img;
	dst_img(Rect(Point(0, 0), Point(200, 200))).copyTo(crop_img);
	imshow("cropped", crop_img);
	cout << "End" << endl;
	waitKey(0);

	return 0;

}