
#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/objdetect.hpp"
#define minrect 30
#define sensitivity 50

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

double t = 0;
string cascadeName = "haarcascades/haarcascade_frontalface_alt.xml";
//string cascadeName = "lbpcascades/lbpcascade_frontalface_improved.xml";
void detectAndDraw(int frcount, Mat& img, CascadeClassifier& cascade, double scale);

void main()
{

	CascadeClassifier cascade;
	VideoCapture cap("vids/test3.webm");
	//VideoCapture cap(0);
	//cap.set(CAP_PROP_FRAME_WIDTH, 1280);
	//cap.set(CAP_PROP_FRAME_HEIGHT, 720);
	Mat img;
	int frcount = 0;

	if (!cascade.load(cascadeName))
	{
		cerr << "ERROR: Could not load classifier cascade" << endl;
		return;
	}
	Size procsize(1280, 720);
	Mat tem;
	cap>>tem;
	//Size procsize(4096,2160);
	while (1) {


		cap >> img;
		Mat cc = tem + img;
		frcount++;
		//--------------------------------------------------------------------------------------------------------------
		t = (double)getTickCount();
		double scale = 1.1;
		detectAndDraw(frcount, img, cascade, scale);

		waitKey(10);
	}

}


void detectAndDraw(int frcount, Mat &img, CascadeClassifier& cascade,
	double scale)
{
	static vector<double> tsum;

	if (frcount > 300) {
		Mat gray;
		cvtColor(img, gray, CV_BGR2GRAY);
		vector<Rect> faces;
		const static Scalar colors[] =
		{
			Scalar(255,0,0),
			Scalar(255,128,0),
			Scalar(255,255,0),
			Scalar(0,255,0),
			Scalar(0,128,255),
			Scalar(0,255,255),
			Scalar(0,0,255),
			Scalar(255,0,255)
		};

		cascade.detectMultiScale(gray, faces, scale, 3, 0);
		t = (double)getTickCount() - t;
		tsum.push_back(t);
		if (size(tsum) > 100)
		{
			cout << "average time = " << mean(tsum).val[0] * 1000 / getTickFrequency() << "%g ms\n";;
			cin >> t;
		}
		cout << "detection time = " << t * 1000 / getTickFrequency() << "%g ms\n";;
		for (size_t i = 0; i < faces.size(); i++)
		{
			Rect r = faces[i];

			Scalar color = colors[i % 8];

			rectangle(img, cvPoint(r.x, r.y),
				cvPoint((r.x + r.width), (r.y + r.height)),
				color, 3, 8, 0);
		}
	}
	imshow("result", img);
}