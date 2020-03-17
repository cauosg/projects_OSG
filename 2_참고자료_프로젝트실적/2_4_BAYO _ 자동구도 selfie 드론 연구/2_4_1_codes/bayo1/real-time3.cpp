#include "opencv2/highgui/highgui.hpp" //header file that contains all the graphical interface functions
#include "opencv2/imgproc/imgproc.hpp"
#include <time.h>
#include <iostream>

using namespace cv;
using namespace std;
#define inf 99999999
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
		//return 0.5f + 1 / (1 + cosn);
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
		//if (now.cluster == 0) {
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
			//props[min_index].cluster = props[i].cluster;

			//	}

	}
	num_of_cluster = cluster_index;
	return props;
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

Mat pic_crop(Mat f, int px, int py, int type) {
	int new_rows = 0;
	int new_cols = 0;
	if (type == 0) {
		new_rows = py * 1.5;
		new_cols = (float)new_rows / (float)f.rows * f.cols;

	}
	else
	{
		new_rows = py * 3;
		new_cols = (float)new_rows / (float)f.rows * f.cols;
	}
	//int beginpoint = px - new_cols * 2 / 3;
	Point2i bp(px - f.cols / 3, py + 2 * f.rows / 3);
	//if (beginpoint + new_cols > f.rows)
	//	beginpoint = f.cols - new_cols;//
	Mat k = Mat(f.rows, f.cols, CV_8UC3);
	Point2i gg = bp;
	for (int i = 0; i < f.rows; i++) {
		for (int j = 0; j < f.cols; j++)
		{
			if (bp.y + j > f.cols - 1 || bp.y + j<0 || i + bp.x> f.rows - 1 || bp.x + i<0)
				k.at<uchar>(i, j) = 128;
			else
				k.at<uchar>(i, j) = f.at<uchar>(i + bp.x, bp.y + j);
		}
	}
	return k;
}

Mat cropped(Mat f, vector<lin_props> lines, int ind) {

	lin_props maxct(Vec4i(0, 0, 0, 0));
	vector<Point2i> vals;
	for (int i = 0; i < ind; i++) {
		Point2i temp(0, 0);
		for (int j = 0; j < lines.size(); j++) {
			if (lines[j].cluster == i) {
				temp.x++;
				temp.y = j;
			}
		}
		vals.push_back(temp);
	}
	vector<lin_props> props;
	Point2i sums(0, 0);
	for (int i = 0; i < vals.size(); i++) {
		sums += Point2i(lines[vals[i].y].bp / 2 - lines[vals[i].y].ep / 2);

	}
	if (sums.x == 0 || sums.y == 0 || abs(sums.x)>5000 || abs(sums.y)>5000)
		return f;
	sums = Point2i(sums.x / vals.size(), sums.y / vals.size());

	Mat out = pic_crop(f, sums.x, sums.y, 0);
	return out;
}

Mat line_cluster(Mat f_src) {

	Mat f_gray, f, edges;

	resize(f_src, f, Size(640, (int)(640 * f_src.rows / f_src.cols)));

	static Mat retimg(f);

	if (!timer_set(7))
		return retimg;

	//f_src.copyTo(meanshift);
	//pyrMeanShiftFiltering(f, f, 20, 45, 2);
	erode(f, f, Mat());
	medianBlur(f, f, 3);
	cvtColor(f, f_gray, CV_RGB2GRAY);
	Canny(f_gray, edges, 100, 200);

	vector<Vec4i> lines;
	HoughLinesP(edges, lines, 1, CV_PI / 180.00, 10, 30, 10);

	//Mat dstImage(f_gray.size(), CV_8UC3);
	//cvtColor(f_gray, dstImage, COLOR_GRAY2BGR);
	Mat dstImage;
	f.copyTo(dstImage);

	int clusters = 0;
	vector<lin_props> props = lines_NN(lines, 100, 0.5, 10, clusters);

	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(dstImage, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(255 - 255 * props[i].cluster / (clusters + 1), 255 * props[i].cluster / (clusters + 1), 255), 3, CV_AA);
	}
	retimg = dstImage;

	return dstImage;
}

void main()
{

	//Mat srcImage = imread("ex3.jpg", IMREAD_GRAYSCALE);

	VideoCapture cap("test2.mp4");
	Mat f_src, dstImage;
	while (true) {
		cap >> f_src;
		if (f_src.empty())
			continue;

		dstImage = line_cluster(f_src);
		imshow("window", dstImage);
		if (waitKey(10) == 27) break;
	}
	//waitKey();
}