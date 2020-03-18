
#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>

#define minrect 10

//float c_star[3] = { 244,188,125 };
float c_star[3] = { 244,205,217 };
//float c_star[3] = { 0,244,0 };
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

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
	return Size(temp.cols/minrect, temp.rows/minrect);

}

void main()
{
	//bgr이 맞다
	//r : 2
	//g : 1
	//b : 0
	//푸쉬는 역순
	VideoCapture cap("vids/test4.webm");
	Mat img;
	//int capcount = 0;
	//int fps = 10;

	Size reduced = getXY(cap);

	while (1) {

		cap >> img;
		Mat eigen;

		resize(img, eigen, reduced);

		//capcount++;
		//if (capcount%fps < fps-1)
	//	continue;

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
		exp(1-(oppo[0] - opcs[0]).mul(oppo[0] - opcs[0]), gb);
		exp(1-(oppo[1] - opcs[1]).mul(oppo[1] - opcs[1]), gg);
		//exp(-(oppo[2] - opcs[2]).mul(oppo[2] - opcs[2]), gr);

		float w = 1;
		sal.push_back(gb * w);
		sal.push_back(gg * w);
		//sal.push_back(gr * w);

		Mat sim = sal[0].mul(sal[1]);
		Mat n_eigen, n_bgr;

		//sim = sim.mul(sim);
		//sim = sim.mul(sim);
		//sim = sim.mul(sim);
		//sim = sim.mul(sim);
		//adaptiveThreshold(sim, nom, 1.0, ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,11,2);
		threshold(sim, n_eigen, mean(sim).val[0], 1.0, THRESH_BINARY);
		cvtColor(n_eigen, n_bgr, CV_GRAY2BGR);
		n_bgr = n_bgr * 255;
		im2double(n_bgr);

		Mat dst = n_bgr.mul(eigen);

		//imshow("similarity", sim);

		resize(cs_mat, cs_mat, Size(1280, 720),0,0, INTER_AREA);
		resize(n_eigen, n_eigen, Size(1280, 720), 0, 0, INTER_AREA);
		resize(dst, dst, Size(1280, 720), 0, 0, INTER_AREA);
		resize(sim, sim, Size(1280, 720), 0, 0, INTER_AREA);
		imshow("cs", sim);
		imshow("thres", n_eigen);
		imshow("dst", dst);



		waitKey(10);
	}

}