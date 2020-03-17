#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#define minrect 8
using namespace std;
using namespace cv;
double t = 0;
void detectAndDraw(Mat& img, CascadeClassifier& cascade, double scale);
//string cascadeName = "haarcascades/haarcascade_frontalface_alt2.xml";
string cascadeName = "haarcascades/haarcascade_frontalface_alt.xml";
//string cascadeName = "lbpcascades/lbpcascade_frontalface_improved.xml";
int frcount = 0;
vector<double> msum;
vector<double> ssum;
void main()
{
	VideoCapture capture("vids/test4.webm");
	Mat frame, image;


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

		capture >> frame;
		frcount++;
		t = (double)getTickCount();

		Mat frame1;

		resize(frame, frame1, Size(1280, 720), 0, 0, INTER_LINEAR);

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

		if (frcount>150)
		{
			cout << "trained mean" << mean(msum).val[0]
				<< endl << "trained sigma" << mean(ssum).val[0];;
			cin >> t;
		}

		cascade.detectMultiScale(gray, faces, scale, 3, 0, Size(minrect * 16 / 9, minrect));
		t = (double)getTickCount() - t;
		cout << "detection time = " << t * 1000 / getTickFrequency() << "%g ms\n";;
		for (size_t i = 0; i < faces.size(); i++)
		{
			Rect r = faces[i];
			Scalar color = colors[i % 8];

			Scalar tmean, tsigma;

			meanStdDev(gray(r), tmean, tsigma);

			msum.push_back(tmean[0]);
			ssum.push_back(tsigma[0]);

			rectangle(img, cvPoint(r.x, r.y),
				cvPoint((r.x + r.width), (r.y + r.height)),
				color, 3, 8, 0);
		}
	}
	imshow("result", img);

}