#include "opencv2/highgui/highgui.hpp" //header file that contains all the graphical interface functions
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <fstream>;


using namespace cv;
using namespace std;
#define inf 99999999;

Mat read_filter(string filename) {
	ifstream reader(filename);
	string a_line;
	Mat out = Mat(0, 20, CV_32F);

	while (getline(reader, a_line)) {
		vector<float> vals;
		stringstream temp(a_line);
		string a_val;
		while (getline(temp, a_val, ',')) {
			vals.push_back(atof(a_val.c_str()));
		}
		Mat a_row;// = (Mat_<float>(1, 10) << vals[0], vals[1]);
		for (int i = 0; i < 20; i++)
			a_row.push_back(vals[i]);
		out.push_back(a_row.t());
	}
	Mat f_out;
	out.convertTo(f_out, CV_32F);
	return f_out;
}

vector<Mat> load_ifilter(Mat f) {
	vector<Mat> out;
	for (int i = 0; i < 9; i++) {
		string fname = "filters/i_filter";
		fname.push_back('1' + i);
		fname = fname + ".csv";
		Mat c = read_filter(fname);
		int m = f.rows;
		int n = f.cols;
		Mat mon(m, n, CV_32F);
		int rowhalf = m / 2;

		for (int j = 0; j < rowhalf; j++)
			for (int k = 0; k < n; k++) {
				///double s = c.at<float>(j, k);;
				mon.at<float>(j, k) = c.at<float>(j, k);
				mon.at<float>(m - j - 1, k) = c.at<float>(m - j - 1, k);
			}

		if (m % 2 == 1) {
			for (int k = 0; k < n; k++) {
				mon.at<float>(rowhalf + 1, k) = 0;
				mon.at<float>(rowhalf + 1, k) = 0;
			}
		}
		out.push_back(mon);
	}
	
	
	return out;
}


vector<Mat> load_cfilter(Mat f) {
	vector<Mat> out;
	for (int i = 0; i < 9; i++) {
		string fname = "filters/c_filter";
		fname.push_back('1' + i);
		fname = fname + ".csv";
		Mat c = read_filter(fname);
		int m = f.rows;
		int n = f.cols;
		Mat mon(m, n, CV_32F);
		int rowhalf = m / 2;

		for (int j = 0; j < rowhalf; j++)
			for (int k = 0; k < n; k++) {
				///double s = c.at<float>(j, k);;
				mon.at<float>(j, k) = c.at<float>(j, k);
				mon.at<float>(m - j - 1, k) = c.at<float>(m - j - 1, k);
			}

		if (m % 2 == 1) {
			for (int k = 0; k < n; k++) {
				mon.at<float>(rowhalf + 1, k) = 0;
				mon.at<float>(rowhalf + 1, k) = 0;
			}
		}
		out.push_back(mon);
	}


	return out;
}

void e_max(Mat &x) {
	float dmax = 0;;
	Mat out;
	//cout << x << endl;
	for (int i = 0; i < x.rows; i++)
		for (int j = 0; j < x.cols; j++) {
			float now = x.at<float>(i, j);
			if (now > dmax) {
				dmax = now;
			}
		}
	x = x / dmax;
}

Mat bandpassfilter(Mat f, Mat filter) {
	Mat fdft;
	dft(f, fdft);
	Mat out = fdft.mul(filter);
	idft(out, out);
	return out;
}

void regionprops(Mat f, Mat f_src)
{
	int thresh = 100;
	RNG rng(12345);
	Mat f8uc = f*255;
	f8uc.convertTo(f8uc, CV_8UC1);
	Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	threshold(f8uc, threshold_output, thresh, 255, THRESH_BINARY);
	findContours(threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f>center(contours.size());
	vector<float>radius(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
		minEnclosingCircle(contours_poly[i], center[i], radius[i]);
	}
	Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
	for (size_t i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours_poly, (int)i, color, 1, 8, vector<Vec4i>(), 0, Point());
		rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		circle(drawing, center[i], (int)radius[i], color, 2, 8, 0);
	}
	Mat out = f_src + drawing;
	imshow("saliency locations", out);
}


void saliency(Mat f_src, vector<Mat> cfilter, vector<Mat> ifilter) {
	Mat f;
	f_src.copyTo(f);
	int orgx = f.cols, orgy = f.rows;
	resize(f, f, Size(20,f.cols / f.rows * 20));
	Mat fd;
	f.convertTo(fd, CV_32FC3);
	vector<Mat> rgby;
	split(fd, rgby);

	Mat fg,fc;
	cvtColor(f, fg, CV_BGR2GRAY);
	Canny(fg, fc, 100, 200);
	fc.convertTo(fc, CV_32F);

	Mat m_rg = (rgby[0] + rgby[1])/255/2;
	Mat m_by = (rgby[2] + m_rg / 2)/255/2;
	Mat out1, out2, out3 = Mat::ones(Size(fg.rows, fg.cols), CV_32F); out3.copyTo(out1); out3.copyTo(out2);
	
	for (int i = 0; i < 9; i++) {
		Mat f_rg = bandpassfilter(m_rg, cfilter[i]);
		Mat f_by = bandpassfilter(m_by, cfilter[i]);

		Mat diffmap = abs(f_rg - f_by);
		out1 = out1.mul(diffmap);

		//for (int i = 0; i < diffmap.rows; i++)
		//	for (int j = 0; j < diffmap.cols; j++)
		//		cout << diffmap.at<float>(i, j) << endl;

		Mat f_gray = bandpassfilter(fc, ifilter[i]);
		Mat imap = abs(f_gray);
		out2 = out2.mul(imap);
	}
	out3 = out1.mul(out2);
	e_max(out1); e_max(out2); e_max(out3);

	Mat toshow;
	resize(out3, toshow, Size(orgx, orgy));
	regionprops(toshow,f_src);
}

int main()
{
	Mat f_gray, meanshift, f_src;
	f_src = imread("imgs/0.jpg");
	pyrMeanShiftFiltering(f_src, meanshift, 20, 45, 2);
	//imshow("pyrMeanShift", meanshift);
	cvtColor(meanshift, f_gray, CV_RGB2GRAY);

	if (f_gray.empty())
		return -1;

	Mat fg20;
	resize(f_gray, fg20, Size(20, f_gray.cols/f_gray.rows*20));
	vector<Mat> cfilters = load_cfilter(fg20);
	vector<Mat> ifilters = load_ifilter(fg20);
	saliency(f_src, cfilters, ifilters);

	waitKey();
	return 0;
}