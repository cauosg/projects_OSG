
#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/objdetect.hpp"
#define minrect 100

//float c_star[3] = { 244,188,125 };
//float c_star[3] = { 244,205,217 };
float c_star[3] = { 178,157,172 };
//float c_star[3] = { 0,244,0 };
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

string cascadeName = "lbpcascades/lbpcascade_frontalface_improved.xml";
void detectAndDraw(int frcount, Mat& img, Mat gray, CascadeClassifier& cascade, double scale);

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

void main()
{
	//bgr이 맞다
	//r : 2
	//g : 1
	//b : 0
	//푸쉬는 역순
	CascadeClassifier cascade;
	VideoCapture cap("vids/test1.webm");
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
	while (1) {

		cap >> img;
		frcount++;

		//--------------------------------------------------------------------------------------------------------------
		Mat eigen, p720;
		resize(img, p720, Size(1280, 720));
		resize(p720, eigen, reduced);

		im2double(eigen);

		vector<Mat> ibgr;
		split(eigen, ibgr);


		Mat one = Mat::ones(eigen.rows, eigen.cols, CV_32F);

		vector<Mat> bgr, sal;
		bgr.push_back(one * (c_star[2] / 256));
		bgr.push_back(one * (c_star[1] / 256));
		bgr.push_back(one * (c_star[0] / 256));

		Mat cs_mat;
		merge(bgr, cs_mat);

		vector<Mat> opcs = bgr_to_opponent(cs_mat);

		vector<Mat> oppo = bgr_to_opponent(eigen);

		Mat gb, gg, gr;
		float w0 = 5;
		exp(w0 - w0 * (oppo[0] - opcs[0]).mul(oppo[0] - opcs[0]), gb);
		exp(w0 - w0 * (oppo[1] - opcs[1]).mul(oppo[1] - opcs[1]), gg);
		exp(-w0 * (oppo[2] - opcs[2]).mul(oppo[2] - opcs[2]), gr);

		float w1 = 1;
		sal.push_back(gb * w1);
		sal.push_back(gg * w1);
		sal.push_back(gr * w1);

		Mat sim = sal[0].mul(sal[1]).mul(sal[2]);

		Mat n_eigen, n_bgr;

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		threshold(sim, n_eigen, mean(sim).val[0], 1.0, THRESH_BINARY);


		Mat ne2;
		n_eigen.copyTo(ne2);
		resize(ne2, ne2, Size(1280, 720), 0, 0, INTER_AREA);

		Mat dst;
		cvtColor(p720, dst, CV_BGR2GRAY);
		im2double(dst);
		dst = dst.mul(ne2);


		im2uint8(dst);
		//cvtColor(dst,dst, CV_BGR2GRAY);

		//dst = dst * 255;
		//dst.convertTo(dst, CV_8UC3);

		double scale = 1.1;
		detectAndDraw(frcount, n_bgr, dst, cascade, scale);

		/*for (int i = 1; i < contours.size(); i++) {
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(dst, contours, i, color, 1, 8, hierarchy, 0, Point());
		}*/

		//imshow("dst", dst);



		waitKey(10);
	}

}

void detectAndDraw(int frcount, Mat& img, Mat gray, CascadeClassifier& cascade,
	double scale)
{

	if (frcount > 140) {
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


		//double fx = 1 / scale;
		//resize(gray, smallImg, Size(), fx, fx, INTER_LINEAR_EXACT);
		//equalizeHist(smallImg, smallImg);
		t = (double)getTickCount();
		cascade.detectMultiScale(gray, faces, scale, 3, 0, Size(minrect, minrect));

		t = (double)getTickCount() - t;
		cout << "detection time = " << t * 1000 / getTickFrequency() << "%g ms\n";;
		for (size_t i = 0; i < faces.size(); i++)
		{
			Rect r = faces[i];

			Scalar color = colors[i % 8];


			rectangle(img, cvPoint(cvRound(r.x*scale - 1), cvRound(r.y*scale)),
				cvPoint(cvRound((r.x + r.width - 1)*scale), cvRound((r.y + r.height - 1)*scale)),
				color, 3, 8, 0);
		}
	}
	imshow("result", img);
}