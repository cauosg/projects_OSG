
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/objdetect.hpp"
#define minrect 16

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

vector<Mat> bgr_to_opponent(Mat img) {
	vector<Mat> bgr, output;
	split(img, bgr);

	output.push_back(sqrtf(1.000f / 2.000f)*bgr[2] - sqrtf(1.000f / 2.000f)*bgr[1]);
	output.push_back(sqrtf(1.000f / 6.000f)*bgr[2] + sqrtf(1.000f / 6.000f)*bgr[1] - sqrtf(1.000f / 6.000f)*bgr[0] * 2);
	//output.push_back(sqrtf(1.000f / 3.000f)*bgr[2] + sqrtf(1.000f / 3.000f)*bgr[1] + sqrtf(1.000f / 3.000f)*bgr[0]);

	return output;
}

Size getXY(VideoCapture cap) {
	Mat temp;
	cap >> temp;
	return Size(temp.cols / minrect, temp.rows / minrect);

}
void regshow(String title, Mat img, Size psize) {

	Mat temp;

	resize(img, temp, psize, 0, 0, INTER_LINEAR);

	cv::imshow(title, temp);
}

Mat read_filter(string filename) {
	ifstream reader(filename);
	string a_line;
	Mat out = Mat(0, 100, CV_32F);

	while (getline(reader, a_line)) {
		vector<float> vals;
		stringstream temp(a_line);
		string a_val;
		while (getline(temp, a_val, ',')) {
			vals.push_back(atof(a_val.c_str()));
		}
		Mat a_row;// = (Mat_<float>(1, 10) << vals[0], vals[1]);
		for (int i = 0; i < 100; i++)
			a_row.push_back(vals[i]);
		out.push_back(a_row.t());
	}
	Mat f_out;
	out.convertTo(f_out, CV_32F);
	return f_out;
}

//Mat load_ifilter(Mat f) {
//	vector<Mat> out;
//	for (int i = 0; i < 9; i++) {
//		string fname = "filters/i_filter";
//		fname.push_back('1' + i);
//		fname = fname + ".csv";
//		Mat c = read_filter(fname);
//		int m = f.rows;
//		int n = f.cols;
//		Mat mon(m, n, CV_32F);
//		int rowhalf = m / 2;
//
//		for (int j = 0; j < rowhalf; j++)
//			for (int k = 0; k < n; k++) {
//				///double s = c.at<float>(j, k);;
//				mon.at<float>(j, k) = c.at<float>(j, k);
//				mon.at<float>(m - j - 1, k) = c.at<float>(m - j - 1, k);
//			}
//
//		if (m % 2 == 1) {
//			for (int k = 0; k < n; k++) {
//				mon.at<float>(rowhalf + 1, k) = 0;
//				mon.at<float>(rowhalf + 1, k) = 0;
//			}
//		}
//		out.push_back(mon);
//	}
//
//
//	return out;
//}

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
	Mat zposes = read_filter("trained/1.csv");
	zposes.convertTo(zposes, CV_32F);
	//Size procsize(4096,2160);
	while (1) {


		cap >> img;
		frcount++;
		//--------------------------------------------------------------------------------------------------------------
		t = (double)getTickCount();
		Mat eigen, p720;
		resize(img, p720, procsize, 0, 0, INTER_LINEAR);
		resize(p720, eigen, reduced, 0, 0, INTER_LINEAR);

		im2double(eigen);

		vector<Mat> oppo = bgr_to_opponent(eigen);

		Mat rst = Mat(0, 100, CV_32F);
		for (int i = 0; i < oppo[0].rows; i++) {
			Mat temp(0, 100, CV_32F);
			for (int j = 0; j < oppo[0].cols; j++)
			{
				int gx = (int)roundf(((oppo[0].at<float>(i, j) + 0.5) * 100));
				int gy = (int)roundf(((oppo[1].at<float>(i, j) + 0.5) * 100));
				//float tempf = zposes.at<float>(gx, gy);
				temp.push_back(zposes.at<float>(gx, gy));
			}
			rst.push_back(temp.t());
		}
		//threshold(sim, n_eigen, mean(sim).val[0], 1.0, THRESH_BINARY);
		////double adg = mean(sim).val[0];
		//cvtColor(n_eigen, n_bgr, CV_GRAY2BGR);
		////regshow("sim", sim, procsize);
		////시그마 커팅 필요, 스레시홀딩된 여러 이미지 합 차 연산
		////im2uint8(n_bgr);
		//n_bgr.convertTo(n_bgr, CV_8UC1);//instead of im2uint8(n_bgr);
		//resize(n_bgr, n_bgr, procsize, 0, 0, INTER_LINEAR);

		//Mat dst = n_bgr.mul(p720);// *0.00392;

		//double scale = 1.1;
		//detectAndDraw(frcount, dst, cascade, scale);
		//GaussianBlur(rst, rst, Size(3, 3), 0);
		threshold(rst,rst, 10, 1.0, THRESH_BINARY);
		cvtColor(rst, rst, CV_GRAY2BGR);
		rst.convertTo(rst, CV_8U);
		
		resize(rst,rst, procsize, 0, 0, INTER_LINEAR);


		Mat toshow = rst.mul(p720);
		imshow("1", toshow);
		waitKey(10);

	}
}



void detectAndDraw(int frcount, Mat& img, CascadeClassifier& cascade,	double scale)
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
		Mat gray;
		cvtColor(img, gray, COLOR_BGR2GRAY);


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
}
