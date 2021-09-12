#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace cv;

#define PAI   3.1415926
#define WSIZE 27

void useAt(Mat img, unsigned char *lut);
void usePtr(Mat img, unsigned char *lut);
void effect_3_2(Mat input, Mat output);
void effect_3_6(Mat input, Mat output, double scale, double th);
void setMaskValue(double *G, int msize, int i, int j, double val);
void makeGauss(double scale, int msize, double *Gx, double *Gy, double *Gxy, double *Gxx, double *Gyy);

int main() {
	Mat src, res1, res2;
	src = imread("lytro-03-A.jpg"); //test image
	imshow("src", src);

	//Mat ans1, ans2;
	//ans1 = imread("cap3_02c.bmp"); //cap3_02c.bmp: 3-2
	//ans2 = imread("cap3_02g.bmp"); //cap3_02g.bmp: 3-6

	res1 = src.clone();
	//res2 = src.clone();

	effect_3_2(src, res1);
	//effect_3_6(src, res2, 3, 5.0);

	//imshow("Ans1", ans1);
	//imshow("Ans2", ans2);
	imshow("sobel", res1);
	//imshow("Canny", res2);

	waitKey(0);
	return 0;
}

void useAt(Mat img, unsigned char *lut) {
	TickMeter tm;
	tm.start();
	int widthLimit = img.channels() * img.cols;
	for (int height = 0; height < img.rows; height++) {
		for (int width = 0; width < widthLimit; width++) {
			img.at<uchar>(height, width) = lut[img.at<uchar>(height, width)];
		}
	}
	tm.stop();
	cout << "at time:" << tm.getTimeSec() << endl;
	imshow("at", img);
}
void usePtr(Mat img, unsigned char *lut) {
	TickMeter tm;
	tm.start();

	int widthLimit = img.channels() * img.cols;
	for (int height = 0; height < img.rows; height++) {
		uchar *data = img.ptr<uchar>(height);
		for (int width = 0; width < widthLimit; width++) {
			img.ptr<uchar>(height)[width] = lut[data[width]];
		}
	}
	tm.stop();
	cout << "ptr time:" << tm.getTimeSec() << endl;
	imshow("ptr", img);
}

void setMaskValue(double *G, int msize, int i, int j, double val)
{
	G[i*msize + j] = val;
}
void makeGauss(double scale, int msize, double *Gx, double *Gy, double *Gxy, double *Gxx, double *Gyy)
{
	int    i, j, k, m, skip;
	int    x0, y0, value;
	int    vmin, vmax, range, val;
	double vx, vy, vxy, vxx, vyy, r0, exponent, dvalue;

	x0 = y0 = WSIZE / 2;
	for (i = 0; i<msize; i++) {
		for (j = 0; j < msize; j++) {
			r0 = (double)((i - y0)*(i - y0) + (j - x0)*(j - x0));

			exponent = 1.0 / (2.0*PAI*scale) * exp(-r0 / (2.0*scale));
			vx = (-(double)(j - x0) / scale) * exponent;
			vy = (-(double)(i - y0) / scale) * exponent;
			vxy = ((double)((i - y0)*(j - x0)) / (scale*scale)) * exponent;
			vxx = ((double)(j - x0)*(j - x0) / (scale*scale) - 1.0 / scale) * exponent;
			vyy = ((double)(i - y0)*(i - y0) / (scale*scale) - 1.0 / scale) * exponent;

			setMaskValue(Gx, msize, i, j, vx);
			setMaskValue(Gy, msize, i, j, vy);
			setMaskValue(Gxy, msize, i, j, vxy);
			setMaskValue(Gxx, msize, i, j, vxx);
			setMaskValue(Gyy, msize, i, j, vyy);
		}
	}
}


void effect_3_2(Mat input, Mat output)
{
	unsigned char *lut;
	int fil1[9] = {
		1, 0,-1,
		2, 0,-2,
		1, 0,-1 };
	int fil2[9] = {
		1, 2, 1,
		0, 0, 0,
		-1,-2,-1 };

	int x, y;
	int i;
	int val;
	int xx, yy;
	int rrx, ggx, bbx;
	int rry, ggy, bby;
	Vec3b pix, col;
	int sadr;
	int x1, y1, x2, y2;

	x1 = 0;
	y1 = 0;
	x2 = input.cols - 1;
	y2 = input.rows - 1;

	int widthLimit = input.channels() * input.cols;

	for (y = 0; y <= y2; y++) {
		Vec3b *data2 = output.ptr<Vec3b>(y);
		for (x = 0; x <= widthLimit; x++) {
			rrx = ggx = bbx = 0;
			rry = ggy = bby = 0;
			sadr = 0;
			for (yy = -1; yy <= 1; yy++) {
				for (xx = -1; xx <= 1; xx++) {
					if (y + yy >= 0 && y + yy < input.rows) {
						if (x + xx >= 0 && x + xx < input.cols) {
							Vec3b *data = input.ptr<Vec3b>(y + yy);
							rrx += data[x + xx].val[2] * fil1[sadr];
							ggx += data[x + xx].val[1] * fil1[sadr];
							bbx += data[x + xx].val[0] * fil1[sadr];
							rry += data[x + xx].val[2] * fil2[sadr];
							ggy += data[x + xx].val[1] * fil2[sadr];
							bby += data[x + xx].val[0] * fil2[sadr];
						}
					}
					sadr++;
				}
			}
			data2[x].val[2] = (int)(sqrt((double)(rrx*rrx + rry * rry)) / 8.0);
			data2[x].val[1] = (int)(sqrt((double)(ggx*ggx + ggy * ggy)) / 8.0);
			data2[x].val[0] = (int)(sqrt((double)(bbx*bbx + bby * bby)) / 8.0);
		}
	}
}
void effect_3_6(Mat input, Mat output, double scale, double th)
{
	int x, y;
	int i;
	int val;
	int xx, yy;
	int wt, ht;
	int xo, yo;
	int rrx, ggx, bbx;
	int rry, ggy, bby;
	int rr, gg, bb, gray;
	Vec3b col;
	int sadr, adr;
	int x1, y1, x2, y2;

	double Lx, Ly, Lxy, Lxx, Lyy;
	double *lap, *edge;
	double *Gx, *Gy, *Gxy, *Gxx, *Gyy;

	Gx = (double*)malloc(sizeof(double)*WSIZE*WSIZE);
	Gy = (double*)malloc(sizeof(double)*WSIZE*WSIZE);
	Gxy = (double*)malloc(sizeof(double)*WSIZE*WSIZE);
	Gxx = (double*)malloc(sizeof(double)*WSIZE*WSIZE);
	Gyy = (double*)malloc(sizeof(double)*WSIZE*WSIZE);

	makeGauss(scale, WSIZE, Gx, Gy, Gxy, Gxx, Gyy);

	x1 = 0;
	y1 = 0;
	x2 = input.cols - 1;
	y2 = input.rows - 1;
	xo = yo = WSIZE / 2;

	wt = x2 - x1 + 1;
	ht = y2 - y1 + 1;

	lap = (double *)malloc(wt*ht * sizeof(double));
	edge = (double *)malloc(wt*ht * sizeof(double));

	for (y = y1; y <= y2; y++) {
		//uchar *data = input.ptr<uchar>(y);
		for (x = x1; x <= x2; x++) {
			Lx = Ly = Lxy = Lxx = Lyy = 0.0;
			for (yy = 0; yy<WSIZE; yy++) {
				for (xx = 0; xx<WSIZE; xx++) {
					if (y + yy >= 0 && y + yy < input.rows) {
						if (x + xx >= 0 && x + xx < input.cols) {
							Vec3b *data = input.ptr<Vec3b>(y + yy);
							rr = data[x + xx].val[2];
							gg = data[x + xx].val[1];
							bb = data[x + xx].val[0];
							gray = (bb * 28 + 77 * rr + gg * 151) / 256;

							Lx += (double)gray * Gx[yy*WSIZE + xx];
							Ly += (double)gray * Gy[yy*WSIZE + xx];
							Lxy += (double)gray * Gxy[yy*WSIZE + xx];
							Lxx += (double)gray * Gxx[yy*WSIZE + xx];
							Lyy += (double)gray * Gyy[yy*WSIZE + xx];
						}
					}
				}
			}
			lap[x + y * wt] = (Lx*Lx*Lxx + 2.0*Lxy*Lx*Ly + Ly * Ly*Lyy);
			edge[x + y * wt] = sqrt(Lx*Lx + Ly * Ly);
		}
	}
	for (y = y1; y <= y2 - 1; y++) {
		Vec3b *data2 = output.ptr<Vec3b>(y);
		for (x = x1; x <= x2 - 1; x++) {
			adr = x + y * wt;
			col.val[2] = 0;
			col.val[1] = 0;
			col.val[0] = 0;

			if (edge[adr] >= th && (lap[adr] * lap[adr + 1]<0.0 || lap[adr] * lap[adr + wt] < 0.0)) {
				col.val[2] = 255;
				col.val[1] = 255;
				col.val[0] = 255;
			}

			data2[x].val[2] = col.val[2];
			data2[x].val[1] = col.val[1];
			data2[x].val[0] = col.val[0];
		}
	}
	free(lap);
	free(edge);
}