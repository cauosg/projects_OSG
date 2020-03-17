#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#define minrect 50
using namespace std;
using namespace cv;
double t = 0;
void detectAndDraw(Mat& img, CascadeClassifier& cascade, double scale);
//string cascadeName = "haarcascades/haarcascade_frontalface_alt2.xml";
string cascadeName = "haarcascades/haarcascade_frontalface_alt.xml";
//string cascadeName = "lbpcascades/lbpcascade_frontalface_improved.xml";
int frcount = 0;
void main()
{
	VideoCapture capture("vids/test2.webm");
	Mat frame, image;
	string inputName;

	CascadeClassifier cascade;
	double scale = 1.1;

	if (!cascade.load(cascadeName))
	{
		cerr << "ERROR: Could not load classifier cascade" << endl;
		return;
	}

	cout << "Video capturing has been started ..." << endl;
	for (;;)
	{
		t = (double)getTickCount();
		capture >> frame;
		frcount++;
		if (frame.empty())
			break;
		Mat frame1;
		resize(frame, frame1, Size(1280, 720));
		detectAndDraw(frame1, cascade, scale);
		char c = (char)waitKey(10);
		if (c == 27 || c == 'q' || c == 'Q')
			break;
	}

	return;
}
void detectAndDraw(Mat& img, CascadeClassifier& cascade,
	double scale)
{
	//if (frcount > 70) {

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
	//double fx = 1 / scale;
	//resize(gray, smallImg, Size(), fx, fx, INTER_LINEAR_EXACT);
	//equalizeHist(smallImg, smallImg);
	
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
	//}
	imshow("result", img);
}