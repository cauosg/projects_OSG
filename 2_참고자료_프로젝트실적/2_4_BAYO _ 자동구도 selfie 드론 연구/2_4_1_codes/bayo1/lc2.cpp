#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
using namespace cv;
using namespace std;
class eq1 {
	public : 
	double m = 0;
	double c = 0;
	
	eq1(Vec4i a_line) {
		m = (double)(a_line.val[1] - a_line.val[3]) / (a_line.val[0] - a_line.val[2]);
		c = a_line.val[1];
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
		for (int j = 0; j < lines.size(); j++) {
			//eq1 y2(lines[j]);

			Point2i cp1 = Point2i(lines[j].val[0], lines[j].val[1]);
			Point2i cp2 = Point2i(lines[j].val[2], lines[j].val[3]);

			double diff1 = (y1.m*cp1.x - cp1.y + y1.c)*(y1.m*cp1.x - cp1.y + y1.c) / (y1.m*y1.m + 1);
			double diff2 = (y1.m*cp2.x - cp2.y + y1.c)*(y1.m*cp2.x - cp2.y + y1.c) / (y1.m*y1.m + 1);

			Point3i min;
			if (diff1 > diff2)
			{
				min = Point3i(2, 3, diff2);
			}
			else {
				min = Point3i(0, 1, diff1);
			}

			if (min.z < treshold) {
				double len = (lines[i].val[2] - lines[j].val[min.x])*(lines[i].val[2] - lines[j].val[min.x]) + (lines[i].val[3] - lines[j].val[min.y])*(lines[i].val[3] - lines[j].val[min.y]);
				if(len<treshold)
					out.push_back(Vec4i(lines[i].val[2], lines[i].val[3], lines[j].val[min.x], lines[j].val[min.y]));
			}
		}
	}
	return out;
}

int main(int argc, char** argv)
{
	const char* f_name = "con3.jpg";
	
	Mat f_src = imread(f_name);
	Mat f, f_edge, g, sub;

	Canny(f_src, f_edge, 50, 200, 3);
	cvtColor(f_edge, g, CV_GRAY2BGR);

	
	//GaussianBlur(g, g, Size(5, 5), 5, 5);
	f_src.copyTo(sub);
	
	vector<Vec4i> lines;
	HoughLinesP(f_edge, lines, 1, CV_PI / 180, 10, 20, 10);

	vector<Vec4i> bridge_lines = connect_line(lines, 1440);//treshold : 연결시킬 line간의 최소 거리의 제곱, 거리를 10으로 할려면 100을 넣으면 됨
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(f_src, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
		line(sub, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
	}
	for (size_t i = 0; i < bridge_lines.size(); i++) {
		Vec4i l = bridge_lines[i];
		line(sub, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 255), 3, CV_AA);
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
