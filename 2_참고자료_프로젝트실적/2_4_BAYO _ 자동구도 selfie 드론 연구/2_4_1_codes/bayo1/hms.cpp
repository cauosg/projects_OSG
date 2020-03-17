#include "opencv2/highgui/highgui.hpp" //header file that contains all the graphical interface functions
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

			Point2i min_1, min_2;
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

int main()
{
	//Mat srcImage = imread("ex3.jpg", IMREAD_GRAYSCALE);
	Mat srcImage, meanshift, colorImage;
	colorImage = imread("con2.jpg");
	pyrMeanShiftFiltering(colorImage, meanshift, 20, 45, 2);
	imshow("pyrMeanShift", meanshift);
	cvtColor(meanshift, srcImage, CV_RGB2GRAY);

	if (srcImage.empty())
		return -1;

	Mat edges;
	Canny(srcImage, edges, 100, 200);
	imshow("edges", edges);

	vector<Vec4i> lines;
	HoughLinesP(edges, lines, 1, CV_PI / 180.00, 10, 30, 10);
	/*probabilistic Hough ��ȯ�� �̿��Ͽ� ������ �� ������ �ִ� ����(line segment)�� �����Ѵ�. ����� lines ����� ������ �� ����(x1,y1,x2,y2)�� �����ϱ�
	���� CV_32SC4 �ڷ����� ����̴�. rho�� �������κ����� �Ÿ� �����̸�, theta�� x����� ������ ���� �����̰�, threshold�� ������ �����ϱ� ���� ��ť�ķ�������
	�Ӱ谪�̴�. minLineLength�� ������ �ּ� ������ �����̸�, maxLineGap�� ���� ������ ���������� �ִ� ��� �����̴�.   */
	vector<Vec4i> bridge_lines = connect_line(lines,1440);
	Mat sub;
	
	Mat dstImage(srcImage.size(), CV_8UC3);
	cvtColor(srcImage, dstImage, COLOR_GRAY2BGR);
	cout << "lines.size()=" << lines.size() << endl;
	dstImage.copyTo(sub);
	Vec4i params;
	int x1, y1, x2, y2;
	float slope;
	for (int k = 0; k < lines.size(); k++) {
		params = lines[k];
		x1 = params[0];
		y1 = params[1];
		x2 = params[2];
		y2 = params[3];
		slope = atan2f(y2 - y1, x2 - x1);
		printf("lines[%2d]=P1(%4d, %4d)=P2(%4d, %4d)\t slope= %10f \n", k, x1, y1, x2, y2, slope);

		//drawing a line segment
		Point pt1(x1, y1), pt2(x2, y2);
		line(dstImage, pt1, pt2, Scalar(0, 0, 255), 3);
		line(sub, pt1, pt2, Scalar(0, 0, 255), 3);
	}

	for (size_t i = 0; i < bridge_lines.size(); i++) {
		Vec4i l = bridge_lines[i];
		line(sub, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
	}

	//imshow("dstImage", dstImage);
	imshow("detected lines", dstImage);
	imshow("connected lines", sub);
	waitKey();
	return 0;
}