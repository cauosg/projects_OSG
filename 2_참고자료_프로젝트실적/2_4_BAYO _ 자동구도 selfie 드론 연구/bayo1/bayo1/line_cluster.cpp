#include "opencv2/highgui/highgui.hpp" //header file that contains all the graphical interface functions
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <deque>

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
	float angle;
	lin_props(Vec4i lines) {
		bp = Point2i(lines.val[0], lines.val[1]);
		ep = Point2i(lines.val[2], lines.val[3]);
		vec = Point2i(lines.val[2] - lines.val[0], lines.val[3] - lines.val[1]);
		length = sqrtf((lines.val[0] - lines.val[2])*(lines.val[0] - lines.val[2]) + (lines.val[1] - lines.val[3])*(lines.val[1] - lines.val[3]));
		angle = atan2f(ep.y - bp.y, ep.x - bp.x);
	}

	lin_props(Point2f a, Point2f b) {
		bp = a;
		ep = b;
	}


	float dot_ang(lin_props in) {
		float cosn = (vec.x * in.vec.x + vec.y * in.vec.y) / (length * in.length);

		//return 0.5f + 1 / (1 + cosn); 
		return sinf(acosf(cosn));
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




class present_line {
public:
	int cluster = 0;
	vector<Point2f> bps;
	vector<Point2f> eps;
	Point2f bp;
	Point2f ep;
	float length;
	//vector<Vec4f> pline;

	float weight;
	present_line(int cluster_order) {
		cluster = cluster_order;
		weight = 0;
		length = 0;
	}

	void add_line(lin_props props) {
		bps.push_back(props.bp);
		eps.push_back(props.ep);
		weight += (1.57 - abs(props.angle))*props.length;
		length += props.length;
	}

	lin_props descion_line() {
		Point2f bpt(0, 0), ept(0, 0);
		for (int i = 0; i < bps.size(); i++) {
			bpt += bps[i];
			ept += eps[i];
		}
		bpt /= (float)bps.size();
		ept /= (float)eps.size();

		float midx = bpt.x + ept.x;
		bpt.x = midx / 2 - length / 2;
		ept.x = midx / 2 + length / 2;
		return lin_props(bpt, ept);
	}
	lin_props descion_line2() {
		float m = 0, c = 0;
		float bsize = bps.size();
		for (int i = 0; i < bsize; i++) {
			float mt = (eps[i].y - bps[i].y) / (eps[i].x - bps[i].x);
			m += mt;
			c += eps[i].y - eps[i].x*mt;
		}
		m /= bsize;
		c /= bsize;
		//c -= 360;
		return lin_props(Point2f(0,c),Point2f(640,m*640+c));
	}
};

inline void draw_linprops(Mat &f, vector<lin_props> props, int clusters) {
	if (clusters < 1)
		return;
	for (size_t i = 0; i < props.size(); i++)
	{
		Vec4i l(props[i].bp.x, props[i].bp.y, props[i].ep.x, props[i].ep.y);
		line(f, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(255 - 255 * props[i].cluster / (clusters + 1), 255 * props[i].cluster / (clusters + 1), 255), 3, CV_AA);
	}

	//Scalar color = Scalar(0, 0, 255);
	//circle(drawing, center_m, (int)rad_m, color, 2, 8, 0);
}
inline void draw_linprops(Mat &f, lin_props a_line) {
	Vec4i l(a_line.bp.x, a_line.bp.y, a_line.ep.x, a_line.ep.y);
	line(f, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 0), 3, CV_AA);
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

bool crosspoint(vector<lin_props> plines, Point2f &cp) {
	float mcx = 0;
	float mcy = 0;
	float limit = 0.01;
	float psize = plines.size() - 1;
	float cxs = 0;
	if (plines.empty())
		return false;

	for (int i = 0; i < psize; i++) {
		float m1 = (plines[i].ep.y - plines[i].bp.y) / (plines[i].ep.x - plines[i].bp.x);
		float m2 = (plines[i + 1].ep.y - plines[i + 1].bp.y) / (plines[i + 1].ep.x - plines[i + 1].bp.x);

		float c1 = plines[i].ep.y - plines[i].ep.x*m1;
		float c2 = plines[i + 1].ep.y - plines[i + 1].ep.x*m2;

		if (abs(c1) < limit || abs(c2) < limit)
			continue;
		float cx = (c2 - c1 + 0.0001f) / (m1 - m2 + 0.0001f);
		float cy = cx*m1 + c1;
		if (cx > 0 && cx < 640 && cy>0 && cy<360) {
			mcx += cx;
			mcy += cy;
			cxs++;
		}
	}
	if (cxs < 1)
		return false;
	else {
		mcx /= cxs; mcy /= cxs;
	}
	cp = Point2f(mcx, mcy);
	return true;
}

bool lines_training(vector<lin_props> props, lin_props &p_line, vector<lin_props> &plines) {
	//chk clst	
	vector<lin_props> out;
	if (props.empty())
		return false;
	vector<int> cluster_indexs;
	cluster_indexs.push_back(props[0].cluster);
	vector<present_line> pls; pls.push_back(present_line(props[0].cluster));

	for (int i = 0; i < props.size(); i++) {
		bool isext = false;
		for (int j = 0; j < cluster_indexs.size(); j++) {
			if (props[i].cluster == cluster_indexs[j]) {
				isext = true;
				pls[j].add_line(props[i]);
			}
		}
		if (!isext) {
			cluster_indexs.push_back(props[i].cluster);
			pls.push_back(present_line(props[i].cluster));
			pls[pls.size() - 1].add_line(props[i]);
		}
	}

	vector<float> weights;
	vector<int> indeces;
	for (int i = 0; i < pls.size(); i++) {
		out.push_back(pls[i].descion_line2());
		weights.push_back(pls[i].weight);
	}
	sortIdx(weights, indeces, CV_SORT_EVERY_ROW + CV_SORT_DESCENDING);

	p_line = out[indeces[0]];
	plines = out;
	return true;
}


bool line_cluster(Mat edges, vector<lin_props> &props) {
	vector<Vec4i> lines;
	HoughLinesP(edges, lines, 1, CV_PI / 180.00, 10, 30, 10);
	if (lines.empty())
		return false;
	int clusters = 0;
	props = lines_NN(lines, 100, 0.5, 10, clusters);
	return true;
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
	Mat fdft, fidft;
	dft(f, fdft);
	Mat out = fdft.mul(filter);
	idft(out, fidft);
	normalize(fidft);
	return fidft;
}

void draw_props(Mat &f, lin_props a_pline, int iters) {
	static deque<lin_props> plines;

	float psize = plines.size();

	if (psize < iters + 1) {
		plines.push_back(a_pline);
	}
	else {
		plines.pop_front();
		plines.push_back(a_pline);
	}

	Point2f bp(0, 0), ep(0, 0); Point3f sd(0, 0, 0);

	for (int i = 0; i < psize; i++) {
		bp += plines[i].bp / psize;
		ep += plines[i].ep / psize;
	}
	line(f, bp, ep, Scalar(0, 255, 0), 2);
}

void draw_sds(Mat &f, Point3f a_sd, int iters) {
	static deque<Point3f> sds;

	float psize = sds.size();

	if (psize < iters + 1) {
		sds.push_back(a_sd);
	}
	else {

		sds.pop_front();
		sds.push_back(a_sd);
	}

	Point2f bp(0, 0), ep(0, 0); Point3f sd(0, 0, 0);

	for (int i = 0; i < psize; i++) {
		sd += sds[i] / psize;
	}
	circle(f, Point2i(sd.x, sd.y), sd.z, Scalar(0, 0, 255), 2);
}

void draw_cps(Mat &f, Point2f cp, int iters) {
	static deque<Point2f> cps;

	float psize = cps.size();

	if (psize < iters + 1) {
		cps.push_back(cp);
	}
	else {

		cps.pop_front();
		cps.push_back(cp);
	}

	Point2f bp(0, 0), ep(0, 0); Point2f sd(0, 0);

	for (int i = 0; i < psize; i++) {
		sd += cps[i] / psize;
	}
	circle(f, Point2i(sd.x, sd.y), 5, Scalar(255, 0, 0), 2);
}

bool regionprops(Mat f, Point3f &cp)
{
	int thresh = 20;
	RNG rng(12345);
	Mat f8uc = f * 255;
	f8uc.convertTo(f8uc, CV_8UC1);
	Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	threshold(f8uc, threshold_output, thresh, 255, THRESH_BINARY);
	findContours(threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<vector<Point> > contours_poly(contours.size());
	vector<Point2f>center(contours.size());
	vector<float>radius(contours.size());
	if (contours.empty())
		return false;
	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		minEnclosingCircle(contours_poly[i], center[i], radius[i]);
	}

	Point2f center_m(0, 0);
	float rad_m = 0;
	for (size_t i = 0; i< contours.size(); i++)
	{
		center_m += center[i] / (float)contours.size();
		rad_m += radius[i] / (float)contours.size();
	}

	cp = Point3f(center_m.x, center_m.y, rad_m);
	return true;
}


int main()
{
	int padsize = 20;
	//Mat srcImage = imread("ex3.jpg", IMREAD_GRAYSCALE);
	Mat f_gray, meanshift, f_src;
	Mat bp30 = load_a_filter("bp30", 360);

	f_src = imread("forrep.jpg");
	resize(f_src, f_src, Size(640, 360));
	pyrMeanShiftFiltering(f_src, meanshift, 20, 45, 2);

	Mat mgray, mcanny;
	cvtColor(meanshift, mgray, CV_BGR2GRAY);
	Canny(mgray, mcanny, 25, 60);
	//imshow("pyrmeanshift edges", mcanny);
	cvtColor(f_src, f_gray, CV_RGB2GRAY);
	Mat showgray;
	f_gray.copyTo(showgray);
	cvtColor(showgray, showgray, CV_GRAY2BGR);
	if (f_gray.empty())
		return -1;

	Mat showcan;
	Canny(f_gray, showcan, 25, 60);
	//imshow("edges without preprocess", showcan);

	im2double(f_gray);

	Mat f_freq10 = bandpassfilter(f_gray, bp30);
	im2uint8(f_freq10);
	remove_pad(f_freq10, padsize);

	Mat edges;
	//imshow("grayimg with sparse process", f_freq10);
	Canny(f_freq10, edges, 15, 40);
	remove_pad(f_src, padsize);
	//imshow("edges with sparse process", edges);

	vector<Vec4i> lines;
	HoughLinesP(edges, lines, 1, CV_PI / 180.00, 10, 30, 10);

	Mat sub;

	Mat dstImage;
	remove_pad(showgray, padsize);
	//f_src.copyTo(dstImage);
	showgray.copyTo(dstImage);
	//cvtColor(f_gray, dstImage, COLOR_GRAY2BGR);
	dstImage.copyTo(sub);

	int clusters = 0;
	vector<lin_props> props = lines_NN(lines, 500, 0.5, 1, clusters);
	lin_props pline(Point2f(0,0),Point2f(0,0));
	vector<lin_props> plines;
	lines_training(props, pline, plines);
	RNG rng(12345);
	Mat ps; f_src.copyTo(ps);
	for (size_t i = 0; i < plines.size(); i++)
	{

		Vec4i l = Vec4i(plines[i].bp.x, plines[i].bp.y, plines[i].ep.x, plines[i].ep.y);
		line(ps, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 2, CV_AA);
	}
	vector<Scalar> colors;
	for (int i = 0; i < clusters + 1; i++) {
		colors.push_back(Scalar(rng.uniform(70, 255), rng.uniform(70, 255), rng.uniform(128, 255)));
	}
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(dstImage, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 2, CV_AA);
		line(sub, Point(l[.0], l[1]), Point(l[2], l[3]), colors[props[i].cluster], 2, CV_AA);
	}

	//cout << "num of clusters : " << clusters;
	//imshow("dstImage", dstImage);
	imshow("detected lines", dstImage);
	imshow("clustered lines", sub);
	imshow("present lines", ps);
	waitKey();
	return 0;
}