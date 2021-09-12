#include <opencv2/opencv.hpp>
#include <algorithm>
#include <iostream>
using namespace std;
int main()
{
	cv::Mat src_img, img_bool, labels, stats, centroids, img_color, img_gray;

	if ((src_img = cv::imread("building.jpg", 0)).empty())
	{
		cout << "load image error！";
		return -1;
	}
	cv::threshold(src_img, img_bool, 0, 255, cv::THRESH_OTSU);
	//連通區塊計算
	int nccomps = cv::connectedComponentsWithStats(
		img_bool, //binary 圖
		labels,     //和原圖一样大的 label 圖
		stats, //nccomps×5的矩陣 表示每個連通區塊的外接矩形和面積（pixel）
		centroids //nccomps×2的矩陣 表示每個連通區塊的質心
	);

	char title[1024];
	printf(title, "原圖連通區域數：%d\n", nccomps);
	cv::String num_connect(title);
	cv::imshow(num_connect, img_bool);

	//去除過小區塊，初始化颜色表
	vector<cv::Vec3b> colors(nccomps);
	colors[0] = cv::Vec3b(0, 0, 0); // background pixels remain black.
	for (int i = 1; i < nccomps; i++) {
		colors[i] = cv::Vec3b(rand() % 256, rand() % 256, rand() % 256);
		//去除面積小於100的連通塊
		if (stats.at<int>(i, cv::CC_STAT_AREA) < 100)
			colors[i] = cv::Vec3b(0, 0, 0); // small regions are painted with black too.
	}
	//按照label值，對不同區域上色
	img_color = cv::Mat::zeros(src_img.size(), CV_8UC3);
	for (int y = 0; y < img_color.rows; y++)
		for (int x = 0; x < img_color.cols; x++)
		{
			int label = labels.at<int>(y, x);
			CV_Assert(0 <= label && label <= nccomps);
			img_color.at<cv::Vec3b>(y, x) = colors[label];
		}

	//降噪後的連通區塊
	cv::cvtColor(img_color, img_gray, cv::COLOR_BGR2GRAY);
	cv::threshold(img_gray, img_gray, 1, 255, cv::THRESH_BINARY);
	nccomps = cv::connectedComponentsWithStats(img_gray, labels, stats, centroids);
	printf(title, "過濾過小區塊後的連通區塊數：%d\n", nccomps);
	num_connect = title;
	cv::imshow(num_connect, img_color);
	cv::waitKey();
	return 0;
}