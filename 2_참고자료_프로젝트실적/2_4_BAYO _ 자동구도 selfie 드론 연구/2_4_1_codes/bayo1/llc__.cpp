#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
using namespace cv;
using namespace std;
class eq1 {
public:
	double m = 0;
	double c = 0;

	eq1(Vec4i a_line) {
		m = (double)(a_line.val[1] - a_line.val[3]) / (a_line.val[0] - a_line.val[2]);
		c = a_line.val[0];
	}
	Point2i solve_eq_2(eq1 &e1) {
		int x1 = (double)(e1.c - c) / (e1.m - m);
		int y1 = m*x1 + c;
		return Point2i(x1, y1);
	}
};
vector<Vec4i> connect_line(vector<Vec4i> lines, int treshold) {
	vector<Vec4i> out;
	for (int i = 0; i < lines.size(); i++) {
		Vec4i a_line = lines[i];
		eq1 y1(lines[i]);
		for (int j = i + 1; j < lines.size(); j++) {
			eq1 y2(lines[j]);

			Point2i cp = y1.solve_eq_2(y2);

			vector<Point2i> x_diff, y_diff;

			x_diff.push_back(Point2i(0, a_line.val[0] - cp.x));
			x_diff.push_back(Point2i(2, a_line.val[2] - cp.x));
			y_diff.push_back(Point2i(1, a_line.val[1] - cp.y));
			y_diff.push_back(Point2i(3, a_line.val[3] - cp.y));

			Point2i min_1, min_2;
			int min = 5000000;
			for (int k = 0; k < 2; k++)
			{
				int diff = x_diff[k].y * x_diff[k].y + y_diff[k].y * y_diff[k].y;
				if (diff < min) {
					min = diff;
					min_1 = Point2i(a_line.val[x_diff[k].x], a_line.val[y_diff[k].x]);
					min_2 = Point2i(lines[j].val[2], lines[j].val[3]);
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
	const char* f_name = "con2.jpg";

	Mat f_src = imread(f_name);
	Mat f, f_edge, g, sub;

	Canny(f_src, f_edge, 50, 200, 3);
	cvtColor(f_edge, g, CV_GRAY2BGR);


	//GaussianBlur(g, g, Size(5, 5), 5, 5);
	f_src.copyTo(sub);

	vector<Vec4i> lines;
	HoughLinesP(f_edge, lines, 1, CV_PI / 180, 10, 20, 10);

	vector<Vec4i> bridge_lines = connect_line(lines, 10);//treshold : 연결시킬 line간의 최소 거리의 제곱, 거리를 10으로 할려면 100을 넣으면 됨
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(f_src, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
		line(sub, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
	}
	for (size_t i = 0; i < bridge_lines.size(); i++) {
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
