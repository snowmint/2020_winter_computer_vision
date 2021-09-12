//#include <tesseract/baseapi.h>
//#include <leptonica/allheaders.h>
#include <leptonica/allheaders.h> // leptonica main header for image io
#include <tesseract/baseapi.h> // tesseract main header
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include<opencv2/highgui/highgui_c.h>
#include "opencv2/opencv.hpp"
using namespace cv;
int main()
{
	char *outText;

	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	Mat src = imread("./alphabet.jpg");
	imshow("src", src);
	/// Convert image to gray and blur it
	cvtColor(src, src, CV_BGR2GRAY);
	blur(src, src, Size(3, 3));
	threshold(src, src, 0, 255, THRESH_OTSU);
	
	// Open input image with leptonica library
	api->SetImage((uchar*)src.data, src.size().width, src.size().height, src.channels(), src.step1());
	//Pix *image = pixRead("./digits.png");
	//api->SetImage(image);
	// Get OCR result
	outText = api->GetUTF8Text();
	printf("OCR output:\n%s", outText);

	// Destroy used object and release memory
	api->End();
	delete api;
	delete[] outText;
	//pixDestroy(&image);
	getchar();
	return 0;
}