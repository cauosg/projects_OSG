
#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/objdetect.hpp"
#define minrect 50

//float c_star[3] = { 244,188,125 };
//float c_star[3] = { 244,205,217 };
//float c_star[3] = { 178,157,172 };
float c_star[3] = { 167,128,116 };
//float c_star[3] = { 0,244,0 };
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

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
void regshow(String title, Mat img, Size psize) {

	Mat temp;

	resize(img, temp, psize);

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
	VideoCapture cap("vids/test2.webm");
	Mat img;
	int frcount = 0;

	//int capcount = 0;
	//int fps = 10;

	Size reduced = getXY(cap);
	if (!cascade.load(cascadeName))
	{
		cerr << "ERROR: Could not load classifier cascade" << endl;
		return;
	}
	Size procsize(1280, 720);
	while (1) {

		cap >> img;
		frcount++;

		//--------------------------------------------------------------------------------------------------------------
		Mat eigen, p720;
		resize(img, p720, procsize, 0, 0, INTER_AREA);
		resize(p720, eigen, reduced, 0, 0, INTER_AREA);

		im2double(eigen);

		vector<Mat> ibgr;
		split(eigen, ibgr);


		Mat one = Mat::ones(eigen.rows, eigen.cols, CV_32F);

		vector<Mat> bgr;
		bgr.push_back(one * (c_star[2] / 256));
		bgr.push_back(one * (c_star[1] / 256));
		bgr.push_back(one * (c_star[0] / 256));

		Mat cs_mat;
		merge(bgr, cs_mat);

		vector<Mat> opcs = bgr_to_opponent(cs_mat);

		vector<Mat> oppo = bgr_to_opponent(eigen);

		Mat sal1, sal2, sal0;
		float w0 = 2;//
		float w1 = 50;//sensitivity, 0~1 : 4
		float w2 = 0.14285;//

		exp(w0 - w1 * (oppo[0] - opcs[0]).mul(oppo[0] - opcs[0]), sal1);
		exp(w0 - w1 * (oppo[1] - opcs[1]).mul(oppo[1] - opcs[1]), sal2);
		exp(w0 - w1 * (oppo[2] - opcs[2]).mul(oppo[2] - opcs[2]), sal0);

		sal1 = sal1 * w2;
		sal2 = sal2 * w2;
		sal0 = sal0 * w2;

		Mat sim = sal1.mul(sal2).mul(sal0);

		//Mat n_eigen, n_bgr;
		regshow("sim", sim, procsize);
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		//threshold(sim, n_eigen, mean(sim).val[0], 1.0, THRESH_BINARY);


		//Mat ne2;
		//n_eigen.copyTo(ne2);
		//im2uint8(ne2);
		//resize(ne2, ne2, procsize);
		resize(sim, sim, procsize);
		//findContours(ne2, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		//--------------------------------------------------------------------------------------------------------------
		RNG rng(12345);





		cvtColor(sim, sim, CV_GRAY2BGR);

		//resize(n_bgr, n_bgr, procsize);
		//resize(n_bgr, n_bgr, procsize, 0, 0, INTER_AREA);
		im2double(p720);

		Mat dst = sim.mul(p720);


		im2uint8(dst);

		double scale = 1.10000000000000089;
		detectAndDraw(frcount, dst, cascade, scale);


		waitKey(10);
	}

}

void detectAndDraw(int frcount, Mat& img, CascadeClassifier& cascade,
	double scale)
{

	if (frcount > 70) {
		double t = 0;
		vector<Rect> faces, faces2;
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
		Mat gray, smallImg;
		cvtColor(img, gray, COLOR_BGR2GRAY);

		t = (double)getTickCount();
		cascade.detectMultiScale(gray, faces, scale, 3, 0, Size(minrect, minrect));
		//imshow("gg", gray);
		t = (double)getTickCount() - t;
		cout << "detection time = " << t * 1000 / getTickFrequency() << "%g ms\n";;
		for (size_t i = 0; i < faces.size(); i++)
		{
			Rect r = faces[i];

			Scalar color = colors[i % 8];


			/*rectangle(img, cvPoint(cvRound(r.x*scale - 1), cvRound(r.y*scale)),
			cvPoint(cvRound((r.x + r.width - 1)*scale), cvRound((r.y + r.height - 1)*scale)),
			color, 3, 8, 0);*/
			//int interpx = minrect, interpy = minrect;
			//int interpx = 80, interpy = 45;
			int interpx = 0, interpy = 0;
			rectangle(img, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),
				cvPoint(cvRound((r.x + r.width - interpx)*scale), cvRound((r.y + r.height - interpy)*scale)),
				color, 3, 8, 0);
		}
	}
	imshow("result", img);
}