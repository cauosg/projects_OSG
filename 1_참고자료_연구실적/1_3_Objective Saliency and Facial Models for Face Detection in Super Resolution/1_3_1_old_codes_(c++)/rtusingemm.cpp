#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/ml.hpp"
#include <iostream>
#define minrect 9
using namespace std;
using namespace cv;
using namespace ml;
double t = 0;
//string cascadeName = "haarcascades/haarcascade_frontalface_alt2.xml";
string cascadeName = "haarcascades/haarcascade_frontalface_alt.xml";
//string cascadeName = "lbpcascades/lbpcascade_frontalface_improved.xml";
int frcount = 0;
vector<double> msum;
vector<double> ssum;


void detectAndDraw(Mat& img, CascadeClassifier& cascade, Mat& sampler)
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

		/*if (frcount>150)
		{
		cout << "trained mean" << mean(msum).val[0]
		<< endl << "trained sigma" << mean(ssum).val[0];;
		cin >> t;
		}*/

		cascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(minrect * 16 / 9, minrect));
		t = (double)getTickCount() - t;
		cout << "detection time = " << t * 1000 / getTickFrequency() << "ms\n";;
		for (size_t i = 0; i < faces.size(); i++)
		{
			Rect r = faces[i];
			Scalar color = colors[i % 8];

			Scalar tmean, tsigma;

			meanStdDev(gray(r), tmean, tsigma);

			//Mat temp;
			//temp.push_back(tmean);
			//temp.push_back(tsigma);

			//sampler.push_back(temp.t());
			sampler.push_back(tmean[0]);

			msum.push_back(tmean[0]);
			ssum.push_back(tsigma[0]);

			rectangle(img, cvPoint(r.x, r.y),
				cvPoint((r.x + r.width), (r.y + r.height)),
				color, 3, 8, 0);
		}
	}
	imshow("result", img);

}

void main()
{
	VideoCapture capture("vids/test4.webm");
	Mat frame, image;


	CascadeClassifier cascade;

	if (!cascade.load(cascadeName))
	{
		cerr << "ERROR: Could not load classifier cascade" << endl;
		return;
	}

	cout << "Video capturing has been started ..." << endl;

	//em_model->setClustersNumber(N);
	//em_model->setCovarianceMatrixType(EM::COV_MAT_SPHERICAL);
	//em_model->setTermCriteria(TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 300, 0.1));
	//em_model->trainEM(samples, noArray(), labels, noArray());


	Ptr<EM> gmm = EM::create();
	gmm->setClustersNumber(5);
	gmm->setCovarianceMatrixType(EM::COV_MAT_SPHERICAL);
	gmm->setTermCriteria(TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 300, 0.1));
	Mat facialmeans;
	//gmm->train();


	Mat sampler, lh, labels, probs;


	for (;;)
	{

		capture >> frame;
		frcount++;
		t = (double)getTickCount();

		Mat frame1;

		resize(frame, frame1, Size(1280, 720), 0, 0, INTER_LINEAR);

		detectAndDraw(frame1, cascade, sampler);

		if (frcount > 150)
		{
			gmm->trainEM(sampler, noArray(), labels, noArray());
		}


		char c = (char)waitKey(10);
		if (c == 27 || c == 'q' || c == 'Q')
			break;
	}



	return;
}

