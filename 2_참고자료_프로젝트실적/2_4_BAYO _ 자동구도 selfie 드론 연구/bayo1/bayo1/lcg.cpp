#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <fstream>;
#include <time.h>;


using namespace cv;
using namespace std;
#define inf 99999999;

class lin_props {
public:
	int cluster = 0;
	Point2f bp;
	Point2f ep;
	Point2f vec;
	float length;
	lin_props(Vec4i lines) {
		bp = Point2i(lines.val[0], lines.val[1]);
		ep = Point2i(lines.val[2], lines.val[3]);
		vec = Point2i(lines.val[2] - lines.val[0], lines.val[3] - lines.val[1]);
		length = sqrtf((lines.val[0] - lines.val[2])*(lines.val[0] - lines.val[2]) + (lines.val[1] - lines.val[3])*(lines.val[1] - lines.val[3]));
	}

	float dot_ang(lin_props in) {
		float cosn = (vec.x * in.vec.x + vec.y * in.vec.y) / (length * in.length);
		return sqrtf(1 - cosn*cosn);
	}

	int sub(lin_props in, int vert_weight) {
		int d[4];
		d[0] = (in.bp.x - bp.x)*(in.bp.x - bp.x) + vert_weight*(in.bp.y - bp.y)*(in.bp.y - bp.y);
		d[1] = (in.bp.x - ep.x)*(in.bp.x - ep.x) + vert_weight*(in.bp.y - ep.y)*(in.bp.y - ep.y);
		d[2] = (in.ep.x - bp.x)*(in.ep.x - bp.x) + vert_weight*(in.ep.y - bp.y)*(in.ep.y - bp.y);
		d[3] = (in.ep.x - ep.x)*(in.ep.x - ep.x) + vert_weight*(in.ep.y - ep.y)*(in.ep.y - ep.y);
		int min_val = inf;
		int ind = 0;
		for (int i = 0; i < 4; i++) {
			if (d[i] < min_val)
				min_val = d[i];
		}

		return min_val;
	}
};
vector<lin_props> lines_NN(vector<Vec4i> lines, float dist_treshold, float uc_weight, int vert_weight, int& num_of_cluster) {
	vector<RotatedRect> region;
	vector<lin_props> props;
	int cluster_index = 0;
	for (int i = 0; i < lines.size(); i++) {
		props.push_back(lin_props(lines[i]));
	}
	for (int i = 0; i < props.size(); i++) {
		lin_props now = props[i];
		float min_dist = inf;
		int min_index = 0;
		for (int j = 0; j < props.size(); j++) {
			if (i != j)
				if ((props[j].cluster != now.cluster) || (props[j].cluster == 0 && now.cluster == 0))
				{
					float dist = now.sub(props[j], vert_weight)*(uc_weight + now.dot_ang(props[j]));
					if (dist < min_dist) {
						min_dist = dist;
						min_index = j;
					}
				}
		}
		if (min_dist<dist_treshold)
			if (props[min_index].cluster == 0)
			{
				if (now.cluster == 0) {//XX
					cluster_index += 1;
					props[min_index].cluster = cluster_index;
					props[i].cluster = cluster_index;
				}
				else//OX
					props[min_index].cluster = now.cluster;
			}
			else if (now.cluster == 0)//XO
				props[i].cluster = props[min_index].cluster;
			else {//OO
				int inst = props[min_index].cluster;
				props[min_index].cluster = now.cluster;
				if (inst>0)
					for (int k = 0; k < props.size(); k++) {
						if (props[k].cluster == inst)
							props[k].cluster = now.cluster;
					}
			}

	}
	num_of_cluster = cluster_index;
	return props;
}

inline void resize(Mat& f, int x, int y) {
	resize(f, f, Size(x, y));
}

inline void im2double(Mat &f) {
	f.convertTo(f, CV_32F);
	f = f / 255;
}

inline void im2uint8(Mat &f) {
	f = f * 255;
	f.convertTo(f, CV_8UC1);
}

inline void remove_pad(Mat &f, int padsize) {
	Rect roi(padsize - 1, padsize - 1, 640 - padsize * 2, 360 - padsize * 2);
	f = f(roi);
	resize(f, 640, 360);
}

inline void normalize(Mat &f) {
	normalize(f, f, 0, 1, CV_MINMAX);
}


void lines_training(vector<lin_props> props, int num_of_clusters) {
	for (int i = 0; i < props.size(); i++) {

	}
}

bool timer_set(int t_delay) {
	static bool swi = false;
	static clock_t prevtime = clock();
	if (!swi) {
		prevtime = clock();
		swi = true;
	}
	else
		if (clock() - prevtime > t_delay)
		{
			swi = false;
			return true;
		}
	return false;
}

Mat line_cluster(Mat f_gray, Mat edges) {
	vector<Vec4i> lines;
	HoughLinesP(edges, lines, 1, CV_PI / 180.00, 10, 30, 10);

	Mat dstImage = Mat::zeros(360,640,CV_8UC3);

	int clusters = 0;
	vector<lin_props> props = lines_NN(lines, 100, 0.5, 10, clusters);

	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(dstImage, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(255 - 255 * props[i].cluster / (clusters + 1), 255 * props[i].cluster / (clusters + 1), 255), 3, CV_AA);
	}
	return dstImage;
}


Mat read_filters(string filename, int rows) {
	ifstream reader(filename);
	string a_line;
	Mat out = Mat(0, rows, CV_32F);

	while (getline(reader, a_line)) {
		Mat a_row;
		stringstream temp(a_line);
		string a_val;
		while (getline(temp, a_val, ',')) {
			a_row.push_back(atof(a_val.c_str()));
		}
		out.push_back(a_row.t());
	}
	Mat f_out;
	out.convertTo(f_out, CV_32F);
	return f_out;
}

Mat load_a_filter(string fn, int rows) {
	string fname = "filters/";
	fname = fname + fn;
	fname = fname + ".csv";

	ifstream reader(fname);
	string a_line;
	Mat out = Mat(0, rows, CV_32F);

	while (getline(reader, a_line)) {
		Mat a_row;
		stringstream temp(a_line);
		string a_val;
		while (getline(temp, a_val, ',')) {
			a_row.push_back(atof(a_val.c_str()));
		}
		out.push_back(a_row.t());
	}
	Mat f_out;
	out.convertTo(f_out, CV_32F);
	return f_out;
}

vector<Mat> load_filters(string fn, int rows) {
	vector<Mat> out;
	for (int i = 0; i < 9; i++) {
		string fname = "filters/";
		fname = fname + fn;
		fname.push_back('1' + i);
		fname = fname + ".csv";
		Mat c = read_filters(fname, rows);
		
		out.push_back(c);
	}
	return out;
}

void e_max(Mat &x) {
	float dmax = 0;;
	Mat out;
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
	Mat fdft,fidft;
	dft(f, fdft);
	Mat out = fdft.mul(filter);
	idft(out, fidft);
	normalize(fidft);
	return fidft;
}

void regionprops(Mat f, Mat f_src)
{
	int thresh = 100;
	RNG rng(12345);
	Mat f8uc = f * 255;
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


void saliency(Mat f_src, vector<Mat> cfilter) {
	Mat f,fd, fg, fc;
	vector<Mat> bgry;

	f_src.copyTo(f);
	int orgx = f.cols, orgy = f.rows;

	resize(f, f, Size(100,100));
	f.convertTo(fd, CV_32FC3);

	split(fd, bgry);

	cvtColor(f, fg, CV_BGR2GRAY);
	//Canny(fg, fc, 16,40);
	fc.convertTo(fc, CV_32F);

	Mat m_rg = (bgry[0] / 2 + bgry[1] / 2);
	Mat m_by = (bgry[2] / 2 + m_rg / 2);

	Mat fred = bgry[2]; Mat fgreen = bgry[1]; Mat fblue = bgry[0];
	im2double(fred); im2double(fgreen); im2double(fblue);

	Mat out = Mat::ones(100,100, CV_32F);

	for (int i = 0; i < 5; i++) {
		Mat g_r = bandpassfilter(fred, cfilter[i]);
		Mat g_g = bandpassfilter(fgreen, cfilter[i]);
		Mat g_b = bandpassfilter(fblue, cfilter[i]);

		Mat f_gray = (g_r + g_g + g_b)/3 + 0.01;

		Mat kout = abs(0.375*fred + 0.375*fgreen - 0.5*fblue);
		Mat ns = Mat::ones(100, 100, CV_32F);
		Mat diffmap = ns.mul(1 / f_gray);
		kout = kout.mul(diffmap);

		out = out.mul(kout);
	}
	normalize(out);
	threshold(out * 255, out, 128, 255, cv::THRESH_BINARY);

	Mat toshow;
	resize(out, toshow, Size(orgx, orgy));
	imshow("saliency", out);
	regionprops(toshow, f_src);
}

void main()
{
	int padsize = 20;
	Mat bp30 = load_a_filter("bp30",360);
	vector<Mat> bp10 = load_filters("cfilters/c_filter",100);
	//vector<Mat> ifilters = load_filters("i_filter",9);
	//vector<Mat> cfilters = load_filters("c_filter",9);
	VideoCapture cap("test2.mp4");

	while (waitKey(10) != 27) {
		//if (timer_set(10)) 
		{
			Mat f_gray, f_src;
			cap >> f_src;
			resize(f_src, 640, 360);
			cvtColor(f_src, f_gray, CV_RGB2GRAY);
			im2double(f_gray);

			if (f_gray.empty())
				continue;

			//Mat fg20;
			//resize(f_gray, fg20, Size(20, f_gray.cols / f_gray.rows * 20));
			//vector<Mat> cfilters = load_cfilter(fg20);
			//vector<Mat> ifilters = load_ifilter(fg20);

			saliency(f_src, bp10);


			Mat f_freq10 = bandpassfilter(f_gray, bp30);
			im2uint8(f_freq10);
			///
			remove_pad(f_freq10, padsize);
			remove_pad(f_src, padsize);

			Mat edges;
			float s = 1.5;//sensitivity;
			Canny(f_freq10, edges, 16 * s, 40 * s);//16,40
			Mat line_draw = line_cluster(f_freq10, edges);

			Mat out = f_src + line_draw;
			imshow("f", out);
		}
	}
}