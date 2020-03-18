#include <stdio.h>
#include <iostream>
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>

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


void main()
{

	//VideoCapture cap("vids/test2.mp4");
	VideoCapture cap(0);
	Mat img;

	while (1) {

		cap >> img;
		/*if (img.empty())
			continue;*/

		im2double(img);
		//Mat img_1;
		//GaussianBlur(img, img, Size(3, 3), 0, 0);

		vector<Mat> gbr;
		split(img, gbr);
		//r : 2
		//g : 1
		//b : 0
		//Mat o1 = sqrtf(1.00f / 2.00f)*gbr[2] - sqrtf(1.00f / 2.00f)*gbr[1];
		//Mat o2 = sqrtf(1.00f / 6.00f)*gbr[2] + sqrtf(1.00f / 6.00f)*gbr[1] - sqrtf(1.00f / 6.00f)*gbr[0] * 2;
		//Mat o3 = sqrtf(1.00f / 3.00f)*gbr[2] + sqrtf(1.00f / 3.00f)*gbr[1] + sqrtf(1.00f / 3.00f)*gbr[0];

		Mat o1 = 0.707*gbr[2] - 0.707*gbr[1];
		Mat o2 = 0.408*gbr[2] + 0.408*gbr[1] - 0.816*gbr[0];
		Mat o3 = 0.577*gbr[2] + 0.577*gbr[1] + 0.577*gbr[0];


		o1 /= mean(o3).val[0];
		o2 /= mean(o3).val[0];
		//o3 /= mean(o3).val[0];

		o1 = o1 - mean(o1).val[0];
		o2 = o2 - mean(o2).val[0];
		o3 = o3 - mean(o3).val[0];



		Mat sm = o1.mul(o1) + o2.mul(o2) +o3.mul(o3);



		//normalize(o1);	normalize(o2);	normalize(o3);
		normalize(sm);
		imshow("sm", sm);
		imshow("img", img);

		//if (waitKey(10) == 27) break;
		waitKey(10);
	}


}