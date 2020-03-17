#include "opencv2/highgui/highgui.hpp" //header file that contains all the graphical interface functions
#include "opencv2/imgproc/imgproc.hpp"
#include <time.h>
#include <iostream>

using namespace cv;
using namespace std;

#include <opencv2/opencv.hpp>

using namespace cv;

void main()
{
	VideoCapture cap("videos/test3.mp4");

	if (!cap.isOpened())
		return;

	Mat img;
	namedWindow("opencv", CV_WINDOW_AUTOSIZE);
	CascadeClassifier upbody;
	string fname = "cas/haarcascade_fullbody.xml";
	upbody.load(fname);
	
	while (true)
	{
		cap >> img;
		if (img.empty())
			continue;

		///cvtColor(img, img, CV_8UC1);
		int res_size = 1280;
		resize(img, img, Size(res_size, img.rows*res_size / img.cols));

		vector<Rect> found, found_filtered;
		upbody.detectMultiScale(img, found, 1.1,1,0|1,Size(40,70));
		//upbody.detectMultiScale(img, found, 1.02, 2, 0 | 1, Size(40, 70));
		size_t i, j;
		for (i = 0; i<found.size(); i++)
		{
			Rect r = found[i];
			for (j = 0; j<found.size(); j++)
				if (j != i && (r & found[j]) == r)
					break;
			if (j == found.size())
				found_filtered.push_back(r);
		}

		for (i = 0; i<found_filtered.size(); i++)
		{
			Rect r = found_filtered[i];
			r.x += cvRound(r.width*0.1);
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8);
			rectangle(img, r.tl(), r.br(), Scalar(0, 255, 0), 3);
		}

		imshow("opencv", img);
		if (waitKey(10) == 27)
			break;
	}
}
