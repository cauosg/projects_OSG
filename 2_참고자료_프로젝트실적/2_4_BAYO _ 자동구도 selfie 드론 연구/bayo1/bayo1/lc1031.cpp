#include "opencv2/highgui/highgui.hpp" //header file that contains all the graphical interface functions
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;
#define inf 99999999;
class lin_props {
public:
	int cluster = -1;
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

	int sub(lin_props in) {
		int d[4];
		d[0] = (in.bp.x - bp.x)*(in.bp.x - bp.x) + (in.bp.y - bp.y)*(in.bp.y - bp.y);
		d[1] = (in.bp.x - ep.x)*(in.bp.x - ep.x) + (in.bp.y - ep.y)*(in.bp.y - ep.y);
		d[2] = (in.ep.x - bp.x)*(in.ep.x - bp.x) + (in.ep.y - bp.y)*(in.ep.y - bp.y);
		d[3] = (in.ep.x - ep.x)*(in.ep.x - ep.x) + (in.ep.y - ep.y)*(in.ep.y - ep.y);
		int min_val = inf;
		int ind = 0;
		for (int i = 0; i < 4; i++)
			if (d[i] < min_val)
				min_val = d[i];

		return min_val;
	}
};
vector<lin_props> line_cluster(vector<Vec4i> lines, float treshold, int dist_threshold, int& num_of_cluster) {
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
		float sec_dist = inf;
		int sec_index = 0;
		//if (now.cluster == -1) {
		for (int j = i + 1; j < props.size(); j++) {
			float dist = now.sub(props[j])*(treshold + now.dot_ang(props[j]));
			if (dist < min_dist) {
				min_dist = dist;
				min_index = j;
			}
		}
		if (props[min_index].cluster == -1)
		{
			if (now.cluster == -1) {
				props[min_index].cluster = ++cluster_index;
				props[i].cluster = cluster_index;
			}
			else
				props[min_index].cluster = now.cluster;
		}
		else
			props[i].cluster = props[min_index].cluster;

		//	}

	}
	num_of_cluster = cluster_index;
	return props;
}

int main()
{

	//Mat srcImage = imread("ex3.jpg", IMREAD_GRAYSCALE);
	Mat f_gray, meanshift, f_src;
	f_src = imread("con4.jpg");
	pyrMeanShiftFiltering(f_src, meanshift, 20, 45, 2);
	imshow("pyrMeanShift", meanshift);
	cvtColor(meanshift, f_gray, CV_RGB2GRAY);

	if (f_gray.empty())
		return -1;

	Mat edges;
	Canny(f_gray, edges, 100, 200);
	imshow("edges", edges);

	vector<Vec4i> lines;
	HoughLinesP(edges, lines, 1, CV_PI / 180.00, 10, 30, 10);

	Mat sub;

	Mat dstImage(f_gray.size(), CV_8UC3);
	cvtColor(f_gray, dstImage, COLOR_GRAY2BGR);
	dstImage.copyTo(sub);

	int clusters = 0;
	vector<lin_props> props = line_cluster(lines, 0.5, 1000, clusters);

	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(dstImage, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(255 - 255 * i / lines.size(), 255 * i / lines.size(), 255 * i / lines.size()), 3, CV_AA);
		line(sub, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(255 - 255 * props[i].cluster / clusters, 255 * props[i].cluster / clusters, 255), 3, CV_AA);
	}

	//Vec4i params;
	//int x1, y1, x2, y2;
	//float slope;
	//for (int k = 0; k < lines.size(); k++) {
	//	//params = lines[k];
	//	//x1 = params[0];
	//	//y1 = params[1];
	//	//x2 = params[2];
	//	//y2 = params[3];
	//	//slope = atan2f(y2 - y1, x2 - x1);
	//	//printf("lines[%2d]=P1(%4d, %4d)=P2(%4d, %4d)\t slope= %10f \n", k, x1, y1, x2, y2, slope);

	//	//drawing a line segment
	//	Point pt1(x1, y1), pt2(x2, y2);
	//	line(dstImage, pt1, pt2, Scalar(0, 0, 255), 3);
	//	line(sub, pt1, pt2, Scalar(0, 0, 255), 3);
	//}

	//imshow("dstImage", dstImage);
	imshow("detected lines", dstImage);
	imshow("clustered lines", sub);
	waitKey();
	return 0;
}