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
	VideoCapture cap("test.mp4");

	if (!cap.isOpened())
		return ;

	Mat img;
	namedWindow("opencv", CV_WINDOW_AUTOSIZE);
	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

	while (true)
	{
		cap >> img;
		if (img.empty())
			continue;

		///cvtColor(img, img, CV_8UC1);
		int res_size = 640;
		resize(img, img, Size(res_size, img.rows*res_size/img.cols));

		vector<Rect> found, found_filtered;
		hog.detectMultiScale(img, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);
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
