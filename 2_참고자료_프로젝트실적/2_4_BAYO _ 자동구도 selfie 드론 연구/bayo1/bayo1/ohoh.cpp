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
	Mat srcImage, meanshift, colorImage, horizImage;
	colorImage = imread("ex4.jpg");
	int width, height;
	width = colorImage.size().width;
	height = colorImage.size().height;
	cout << "IMAGE SIZE : " << width << "," << height << endl;

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
	/*probabilistic Hough 변환을 이용하여 직선의 양 끝점이 있는 선분(line segment)을 검출한다. 출력인 lines 행렬은 선분의 양 끝점(x1,y1,x2,y2)을 저장하기
	위한 CV_32SC4 자료형의 행렬이다. rho는 원점으로부터의 거리 간격이며, theta는 x축과의 각도로 라디안 간격이고, threshold는 직선을 검출하기 위한 어큐뮬레이터의
	임계값이다. minLineLength는 검출할 최소 직선의 길이이며, maxLineGap은 직선 위에서 에지점들의 최대 허용 간격이다.   */
	vector<Vec4i> bridge_lines = connect_line(lines, 1440);
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

		////horizontal line찾기
		//int rect_x, rect_y, rect_width, rect_height;
		//rect_x = 0; rect_y = 0; rect_width = srcImage.cols; rect_height = srcImage.rows / 8;
		//int arrCount[8] = { 0, };
		//for (int i=0; i < 8; i++) {
		//   if ((x1 > rect_x && x1 < rect_width) && (y1 > rect_y && y1 < rect_height)) {
		//      arrCount[i] += 1;
		//      cout << arrCount[i] << endl;
		//   }
		//   rect_y = i + rect_height;
		//   rect_height += rect_height;
		//}


		//drawing a line segment
		Point pt1(x1, y1), pt2(x2, y2);
		line(dstImage, pt1, pt2, Scalar(0, 0, 255), 2);
		horizImage = dstImage.clone();
		line(sub, pt1, pt2, Scalar(0, 0, 255), 2);
	}


	//lines[i].val[0]=x1, lines[i].val[1]=y1, lines[i].val[2]=x2, lines[i].val[3]=y2

	Vec4i horizRect;
	int rect_x1, rect_y1, rect_x2, rect_y2;
	int increment = (int)srcImage.size().height / 8;
	int regionCount[8] = { 0,0,0,0,0,0,0,0 };
	int count;
	rect_x1 = 0; rect_y1 = 0; rect_x2 = srcImage.size().width; rect_y2 = increment;
	//horizRect[rect_x1, rect_y1, rect_x2, rect_y2];

	horizRect[0] = rect_x1;
	horizRect[1] = rect_y1;
	horizRect[2] = rect_x2;
	horizRect[3] = rect_y2;

	for (int i = 0; i < 8; i++) {
		for (int k = 0; k < lines.size(); k++) {
			{
				//if ((lines[k].val[0] > horizRect[0]) && (lines[k].val[0] < horizRect[2]) && (lines[k].val[2] > horizRect[0]) && (lines[k].val[2] < horizRect[2])) { //x값들이 width 안의 값들인 경우
				if (lines[k].val[1] > horizRect[1] && lines[k].val[1] < horizRect[3] && lines[k].val[3] > horizRect[1] && lines[k].val[3] < horizRect[3]) { //y값들이 given 영역 안의 값들인 경우
					regionCount[i] += 1;
					//cout << regionCount[i] << endl;
				}//end of if
				 //}//end of if

			}//end of for
		}//end of for
		horizRect[1] += increment;
		horizRect[3] += increment;
	}
	for (int i = 0; i < 8; i++) {
		cout << regionCount[i] << endl;
	}

	//winning region selection
	for (int i = 0; i < 8; i++) {
		if (regionCount[i] > regionCount[i + 1])
			int horizon = i;
	}


	for (size_t i = 0; i < bridge_lines.size(); i++) {
		Vec4i l = bridge_lines[i];
		line(sub, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
	}





	//region of detected horizontal line
	Mat roi = horizImage(Rect(0, 0, width, height / 4));
	Mat color(roi.size(), CV_8UC3, Scalar(0, 125, 125));
	double alpha = 0.3;
	addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);

	imshow("dstImage", dstImage);
	imshow("detected lines", dstImage);
	imshow("horizontal line detection", horizImage);
	//imshow("connected lines", sub);
	waitKey();
	return 0;
}