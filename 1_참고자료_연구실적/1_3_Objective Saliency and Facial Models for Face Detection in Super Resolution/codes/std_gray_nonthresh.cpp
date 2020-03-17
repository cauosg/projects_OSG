
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
void detectAndDraw(int frcount, Mat &gray, Mat& img, CascadeClassifier& cascade, double scale);

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
	VideoCapture cap("vids/test4.webm");
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
	//Size procsize(4096,2160);
	while (1) {


		cap >> img;
		frcount++;
		//--------------------------------------------------------------------------------------------------------------
		t = (double)getTickCount();
		Mat eigen, p720;
		resize(img, p720, procsize, 0, 0, INTER_LINEAR);
		resize(p720, eigen, reduced, 0, 0, INTER_LINEAR);
		Mat g720 = rgb2gray(p720);



		//imshow("awef", g720);
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
		float w1 = sensitivity;//sensitivity, 0~1 : 4
		float w2 = 0.14285;//

		exp(w0 - w1 * (oppo[0] - opcs[0]).mul(oppo[0] - opcs[0]), sal1);
		exp(w0 - w1 * (oppo[1] - opcs[1]).mul(oppo[1] - opcs[1]), sal2);
		exp(w0 - w1 * (oppo[2] - opcs[2]).mul(oppo[2] - opcs[2]), sal0);

		sal1 = sal1 * w2;
		sal2 = sal2 * w2;
		sal0 = sal0 * w2;

		Mat sim = sal1.mul(sal2).mul(sal0);

		Mat n_eigen, n_bgr;

		GaussianBlur(sim, n_eigen, Size(3, 3), 0);
		//threshold(sim, n_eigen, mean(sim).val[0], 1.0, THRESH_BINARY);
		//double adg = mean(sim).val[0];
		//cvtColor(n_eigen, n_bgr, CV_GRAY2BGR);
		//regshow("sim", sim, procsize);
		//시그마 커팅 필요, 스레시홀딩된 여러 이미지 합 차 연산
		//im2uint8(n_bgr);
		//n_eigen.convertTo(n_eigen, CV_8UC1);//instead of im2uint8(n_bgr);

		n_eigen = n_eigen * 255;
		n_eigen.convertTo(n_eigen, CV_8U);
		resize(n_eigen, n_eigen, procsize, 0, 0, INTER_LINEAR);
		//g720.convertTo(g720,CV_32F);
		//1124024349 : cv32f
		//1124024344 : 8uc1

		//1124024325 : ?
		//1124024350 : 64f

		Mat dst = n_eigen.mul(g720)*0.00392 * 3;

		double scale = 1.1;
		detectAndDraw(frcount, dst, p720, cascade, scale);

		waitKey(10);
	}

}



//void detectAndDraw(int frcount, Mat& gray,Mat &img, CascadeClassifier& cascade,
//	double scale)
//{
//	if (frcount > 100) {
//		vector<Rect> faces;
//
//		cascade.detectMultiScale(gray, faces, scale, 3, 0, Size(minrect * 16 / 9, minrect));
//		t = (double)getTickCount() - t;
//		cout << "detection time = " << t * 1000 / getTickFrequency() << "%g ms\n";;
//		for (size_t i = 0; i < faces.size(); i++)
//		{
//			Rect r = faces[i];
//
//			Scalar color = { 255,255,255 };
//			//rectangle(img, cvPoint(r.x, r.y),
//			rectangle(gray, cvPoint(r.x, r.y),
//				cvPoint((r.x + r.width), (r.y + r.height)),
//				color, 3, 8, 0);
//		}
//	}
//	//imshow("result", img);
//	imshow("result", gray);
//}



void detectAndDraw(int frcount, Mat& gray, Mat &img, CascadeClassifier& cascade,
	double scale)
{
	if (frcount > 100) {
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
	imshow("smap", gray);
}