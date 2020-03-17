#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
using namespace cv;
using namespace std;

vector<Vec4i> connect_line(vector<Vec4i> lines, int treshold) {
	vector<Vec4i> out;
	for (int i = 0; i < lines.size(); i++) {
		Vec4i a_line = lines[i];
		for (int j = i; j < lines.size(); j++) {
			vector<Point3i> x_diff, y_diff;

			x_diff.push_back(Point3i(0, 0, a_line.val[0] - lines[j].val[0]));
			x_diff.push_back(Point3i(0, 2, a_line.val[0] - lines[j].val[2]));
			x_diff.push_back(Point3i(2, 0, a_line.val[2] - lines[j].val[0]));
			x_diff.push_back(Point3i(2, 2, a_line.val[2] - lines[j].val[2]));

			y_diff.push_back(Point3i(1, 1, a_line.val[1] - lines[j].val[1]));
			y_diff.push_back(Point3i(1, 3, a_line.val[1] - lines[j].val[3]));
			y_diff.push_back(Point3i(3, 1, a_line.val[3] - lines[j].val[1]));
			y_diff.push_back(Point3i(3, 3, a_line.val[3] - lines[j].val[3]));

			Point2i min_1,min_2;
			int min = 50000;
			for (int k = 0; k < 4; k++)
			{
				int diff = x_diff[k].z * x_diff[k].z + y_diff[k].z * y_diff[k].z;
				if (diff < min) {
					min = diff;
					min_1 = Point2i(a_line.val[x_diff[k].x], a_line.val[y_diff[k].x]);
					min_2 = Point2i(lines[j].val[x_diff[k].y], lines[j].val[y_diff[k].y]);
				}
			}
			if (min < treshold)
				out.push_back(Vec4i(min_1.x, min_1.y, min_2.x, min_2.y));
				
		}
	}
	return out;
}

int main(int argc, char** argv)
{
	const char* f_name = "con4.jpg";

	Mat f_src = imread(f_name);
	Mat f, f_edge, g, sub;

	//int erosion_size = 10;
	//Mat element = getStructuringElement(cv::MORPH_DILATE, cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1), cv::Point(erosion_size, erosion_size));

	//dilate(f_src, f_src, Mat(erosion_size, erosion_size,CV_8UC1));

	Canny(f_src, f_edge, 50, 200, 3);
	cvtColor(f_edge, g, CV_GRAY2BGR);
	//GaussianBlur(g, g, Size(5, 5), 5, 5);
	f_src.copyTo(sub);

	vector<Vec4i> lines;
	HoughLinesP(f_edge, lines, 1, CV_PI / 180, 180, 20, 10);

	vector<Vec4i> bridge_lines = connect_line(lines,1200);//treshold : 연결시킬 line간의 최소 거리의 제곱, 거리를 10으로 할려면 100을 넣으면 됨
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(f_src, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
		line(sub, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
	}
	for (size_t i = 0; i <  bridge_lines.size(); i++) {
		Vec4i l = bridge_lines[i];
		line(sub, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
	}

		//imshow("source", f);
		imshow("detected lines", f_src);
		imshow("connected lines", sub);
		//imshow("detected main line", h);
		//imshow("detected sub line", m);
		//imshow("cropped1", crop_1);
		//imshow("crosspoint", cross_1);


	waitKey(0);

	return 0;
}
