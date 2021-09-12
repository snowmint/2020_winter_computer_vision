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

int avg_cb = 120;//YCbCr顏色空間膚色cb的平均值
int avg_cr = 155;//YCbCr顏色空間膚色cr的平均值
int skinRange = 22;//YCbCr顏色空間膚色的範圍 22


int main(int argc, const char** argv) {
	VideoCapture cam(0);
	if (!cam.isOpened()) {
		cout << "ERROR not opened " << endl;
		return -1;
	}
	Mat img;
	Mat img_threshold;
	Mat img_gray;
	Mat img_roi;
	namedWindow("Original_image", CV_WINDOW_AUTOSIZE);
	//namedWindow("Gray_image", CV_WINDOW_AUTOSIZE);
	//namedWindow("Thresholded_image", CV_WINDOW_AUTOSIZE);
	namedWindow("ROI", CV_WINDOW_AUTOSIZE);
	char a[40];
	int count = 0;
	while (1) {
		bool b = cam.read(img);
		if (!b) {
			cout << "ERROR : cannot read" << endl;
			return -1;
		}
		//img_color = img;
		Mat brightYCB = img;
		Mat ycbimg = img;
		cvtColor(ycbimg, ycbimg, COLOR_BGR2YCrCb);
		//int lower_skin[3] = {100, 150, 0};
		//int upper_skin[3] = {140, 255, 255};
		//int skinRange = 22;
		Vec3b scalarImg;
		double cb, cr;
		for (int i = 0; i < img.cols; i++) {
			for (int j = 0; j < img.rows; j++)
			{
				scalarImg = img.ptr<Vec3b>(j)[i];
				cr = scalarImg.val[1];
				cb = scalarImg.val[2];
				if ((cb > avg_cb - skinRange && cb < avg_cb + skinRange) &&
					(cr > avg_cr - skinRange && cr < avg_cr + skinRange)) {
					ycbimg.ptr<Vec3b>(j)[i].val[0] = saturate_cast<uchar>(255);
					ycbimg.ptr<Vec3b>(j)[i].val[1] = saturate_cast<uchar>(255);
					ycbimg.ptr<Vec3b>(j)[i].val[2] = saturate_cast<uchar>(255);
				}
				else {
					ycbimg.ptr<Vec3b>(j)[i].val[0] = saturate_cast<uchar>(0);
					ycbimg.ptr<Vec3b>(j)[i].val[1] = saturate_cast<uchar>(0);
					ycbimg.ptr<Vec3b>(j)[i].val[2] = saturate_cast<uchar>(0);
				}
			}
		}
		//imshow("Result YCB", ycbimg);

		Mat opening;
		Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
		morphologyEx(ycbimg, opening, MORPH_OPEN, kernel);//CV_MOP_OPEN
														  //imshow("opening", opening);
		Mat closing;
		Mat kernel2 = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
		morphologyEx(opening, closing, MORPH_CLOSE, kernel2);
		/////////////////////////////////////////////////////////////
		Mat img_bool, labels, stats, centroids, img_color, img_gray;
		Mat newinput = closing;
		cvtColor(newinput, newinput, CV_YCrCb2BGR);
		cvtColor(newinput, newinput, CV_BGR2GRAY);
		//一定要轉成灰階!!!!!!!!!!!!超重要!!!!!!!!!!!!
		threshold(newinput, img_bool, 0, 255, THRESH_OTSU);
		//imshow("img_bool", img_bool);
		//threshold(preprocess, img_bool, 0, 255, THRESH_OTSU);
		//連通域計算
		int nccomps = cv::connectedComponentsWithStats(
			img_bool, //二值圖像
			labels, //和原圖一樣大的標記圖
			stats, //nccomps×5的矩陣 表示每個連通區域的外接矩形和面積（pixel）
			centroids //nccomps×2的矩陣 表示每個連通區域的質心
		);
		//顯示原圖統計結果
		char title[5] = "num1";
		String num_connect(title);
		//printf(title, "原圖中連通區域數：%d\n", nccomps);
		//imshow(num_connect, img_bool);

		//去除過小區域，初始化顏色表
		vector<Vec3b> colors(nccomps);
		colors[0] = Vec3b(0, 0, 0); // background pixels remain black.
		for (int i = 1; i < nccomps; i++) {
			colors[i] = Vec3b(rand() % 256, rand() % 256, rand() % 256);
			//去除面積小於100的連通域
			if (stats.at<int>(i, CC_STAT_AREA) < 300)
				colors[i] = cv::Vec3b(0, 0, 0); // small regions are painted with black too.
		}
		//按照label值，對不同的連通域進行著色
		img_color = Mat::zeros(ycbimg.size(), CV_8UC3);
		for (int y = 0; y < img_color.rows; y++) {
			for (int x = 0; x < img_color.cols; x++)
			{
				int label = labels.at<int>(y, x);
				CV_Assert(0 <= label && label <= nccomps);
				img_color.at<Vec3b>(y, x) = colors[label];
			}
		}
		//統計降噪後的連通區域
		cvtColor(img_color, img_gray, COLOR_BGR2GRAY);
		threshold(img_gray, img_gray, 1, 255, THRESH_BINARY);
		nccomps = cv::connectedComponentsWithStats(img_gray, labels, stats, centroids);
		char title2[5] = "num2";
		printf("過濾小目標後的連通區域數量：%d\n", nccomps);
		num_connect = title2;
		imshow(num_connect, img_color);

		//imshow("img_bool", closing);*/

		Rect roi(340, 100, 270, 270);
		img_roi = img(roi);
		cvtColor(img_roi, img_gray, CV_RGB2GRAY);

		GaussianBlur(img_gray, img_gray, Size(19, 19), 0.0, 0);
		threshold(img_gray, img_threshold, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);

		vector<vector<Point> >contours;
		vector<Vec4i>hierarchy;
		findContours(img_threshold, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point());//CV_RETR_TREE //CV_RETR_EXTERNAL
		if (contours.size()>0) {
			size_t indexOfBiggestContour = -1;
			size_t sizeOfBiggestContour = 0;

			for (size_t i = 0; i < contours.size(); i++) {
				if (contours[i].size() > sizeOfBiggestContour) {
					sizeOfBiggestContour = contours[i].size();
					indexOfBiggestContour = i;
				}
			}
			vector<vector<int> >hull(contours.size());
			vector<vector<Point> >hullPoint(contours.size());
			vector<vector<Vec4i> >defects(contours.size());
			vector<vector<Point> >defectPoint(contours.size());
			vector<vector<Point> >contours_poly(contours.size());
			Point2f rect_point[4];
			vector<RotatedRect>minRect(contours.size());
			vector<Rect> boundRect(contours.size());
			for (size_t i = 0; i<contours.size(); i++) {
				if (contourArea(contours[i])>5000) {
					convexHull(contours[i], hull[i], true);
					convexityDefects(contours[i], hull[i], defects[i]);
					if (indexOfBiggestContour == i) {
						minRect[i] = minAreaRect(contours[i]);
						for (size_t k = 0; k<hull[i].size(); k++) {
							int ind = hull[i][k];
							hullPoint[i].push_back(contours[i][ind]);
						}
						count = 0;

						for (size_t k = 0; k<defects[i].size(); k++) {
							if (defects[i][k][3]>13 * 256) {
								int p_end = defects[i][k][1];
								int p_far = defects[i][k][2];
								defectPoint[i].push_back(contours[i][p_far]);
								circle(img_roi, contours[i][p_end], 3, Scalar(0, 255, 0), 2);
								count++;
							}
						}

						if (count == 1)
							strcpy_s(a, "1");
						else if (count == 2)
							strcpy_s(a, "2");
						else if (count == 3)
							strcpy_s(a, "3");
						else if (count == 4)
							strcpy_s(a, "4");
						else if (count == 5)
							strcpy_s(a, "5");
						else
							strcpy_s(a, "else");

						putText(img, a, Point(70, 70), CV_FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 0, 0), 2, 8, false);
						drawContours(img_threshold, contours, i, Scalar(255, 255, 0), 2, 8, vector<Vec4i>(), 0, Point());
						drawContours(img_threshold, hullPoint, i, Scalar(255, 255, 0), 1, 8, vector<Vec4i>(), 0, Point());
						drawContours(img_roi, hullPoint, i, Scalar(0, 0, 255), 2, 8, vector<Vec4i>(), 0, Point());
						approxPolyDP(contours[i], contours_poly[i], 3, false);
						boundRect[i] = boundingRect(contours_poly[i]);
						//tl 返回rect的左上頂點的座標
						//br 返回rect的右下頂點的座標
						printf("Rect (%d, %d)", boundRect[i].tl(), boundRect[i].br());
						rectangle(img_roi, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 2, 8, 0);
						minRect[i].points(rect_point);
						for (size_t k = 0; k<4; k++) {
							line(img_roi, rect_point[k], rect_point[(k + 1) % 4], Scalar(0, 0, 255), 2, 8);
						}
					}
				}
			}
			imshow("Original_image", img);
			//imshow("Gray_image", img_gray);
			//imshow("Thresholded_image", img_threshold);
			imshow("ROI", img_roi);
			if (waitKey(30) == 27) {
				return -1;
			}
		}
	}
	return 0;
}