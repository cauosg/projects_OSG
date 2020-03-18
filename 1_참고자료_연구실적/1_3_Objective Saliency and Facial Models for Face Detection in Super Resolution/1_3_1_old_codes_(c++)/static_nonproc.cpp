
#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/objdetect.hpp"
#define minrect 20
#define sensitivity 50

//float c_star[3] = { 244,188,125 };
//float c_star[3] = { 244,205,217 };
//float c_star[3] = { 178,157,172 };
//float c_star[3] = { 167,128,116 };
float c_star[3] = { 165,81,69 };
//float c_star[3] = { 0,244,0 };
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

double t = 0;
string cascadeName = "haarcascades/haarcascade_frontalface_alt.xml";
//string cascadeName = "lbpcascades/lbpcascade_frontalface_improved.xml";
void detectAndDraw(int frcount, Mat& img, CascadeClassifier& cascade, double scale);

inline void im2double(Mat &f) {
	f.convertTo(f, CV_32F);
	f = f / 255;
}

inline void normalize(Mat &f) {
	normalize(f, f, 0, 1, CV_MINMAX);
}

inline void im2uint8(Mat &f) {
	f = f * 255;
	f.convertTo(f, CV_8UC1);
}

inline Mat rgb2gray(Mat &f) {
	Mat temp;
	cvtColor(f, temp, CV_BGR2GRAY);
	return temp;
}

vector<Mat> bgr_to_opponent(Mat img) {
	vector<Mat> bgr, output;
	split(img, bgr);

	output.push_back(sqrtf(1.000f / 2.000f)*bgr[2] - sqrtf(1.000f / 2.000f)*bgr[1]);
	output.push_back(sqrtf(1.000f / 6.000f)*bgr[2] + sqrtf(1.000f / 6.000f)*bgr[1] - sqrtf(1.000f / 6.000f)*bgr[0] * 2);
	output.push_back(sqrtf(1.000f / 3.000f)*bgr[2] + sqrtf(1.000f / 3.000f)*bgr[1] + sqrtf(1.000f / 3.000f)*bgr[0]);

	return output;
}

Size getXY(VideoCapture cap) {
	Mat temp;
	cap >> temp;
	return Size(temp.cols / minrect, temp.rows / minrect);

}
inline void regshow(String title, Mat img, Size psize) {

	Mat temp;

	resize(img, temp, psize, 0, 0, INTER_LINEAR);

	imshow(title, temp);
}

void main()
{
	//bgr이 맞다
	//r : 2
	//g : 1
	//b : 0
	//푸쉬는 역순
	CascadeClassifier cascade;
	//VideoCapture cap(0);
	//cap.set(CAP_PROP_FRAME_WIDTH, 1280);
	//cap.set(CAP_PROP_FRAME_HEIGHT, 720);
	Mat img = imread("evalas/5.jpg");
	int frcount = 0;

	//int capcount = 0;
	//int fps = 10;

	Size reduced = Size(img.cols / minrect, img.rows / minrect);
	if (!cascade.load(cascadeName))
	{
		cerr << "ERROR: Could not load classifier cascade" << endl;
		return;
	}
	Size procsize(1280, 720);
	//Size procsize(4096,2160);
	//--------------------------------------------------------------------------------------------------------------
	t = (double)getTickCount();
	Mat eigen, p720, dst;
	resize(img, dst, procsize, 0, 0, INTER_LINEAR);

	double scale = 1.1;
	detectAndDraw(frcount, dst, cascade, scale);

	waitKey(0);

}


void detectAndDraw(int frcount, Mat &img, CascadeClassifier& cascade,
	double scale)
{
	static vector<double> tsum;

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

	cascade.detectMultiScale(gray, faces, scale, 3, 0, Size(minrect * 16 / 9, minrect));
	t = (double)getTickCount() - t;
	tsum.push_back(t);
	if (size(tsum) > -1)
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
	imshow("result", img);

}