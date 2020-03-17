#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
using namespace cv;
using namespace std;

class _linprop {
public:
	unsigned int x = 0;
	unsigned int y = 0;

	unsigned int length = 0;
	float m = 0;
	int n = 0;
	float degree = 0;

	_linprop(Vec4i l) {
		x = l[0];
		y = l[1];
		length = roundf(sqrtf(powf(l[2] - l[0], 2) + powf(l[3] - l[1], 2)));
		degree = atan2f(l[3] - l[1], l[2] - l[0]);
		m = (float)(float)(l[3] - l[1]) / (l[2] - l[0]);
		n = l[1] - m*l[0];
	}
};

unsigned int detect_mainline(vector<_linprop>lines) {
	unsigned int maxval = 0;
	unsigned int max_ind = 0;
	for (size_t i = 0; i < lines.size(); i++)
	{
		if (lines[i].length > maxval) {
			maxval = lines[i].length;
			max_ind = i;
		}
	}
	return max_ind;
}

vector<unsigned int> detect_subline(vector<_linprop>lines) {
	unsigned int maxval = 0;
	unsigned int max_ind = 0;
	unsigned int maxval2 = 0;
	unsigned int max_ind2 = 0;
	unsigned int maxval3 = 0;
	unsigned int max_ind3 = 0;

	vector<unsigned int>output;

	for (size_t i = 0; i < lines.size(); i++)
	{
		if (lines[i].length > maxval) {
			maxval = lines[i].length;
			max_ind = i;
		}
	}
	output.push_back(max_ind);
	for (size_t i = 0; i < lines.size(); i++)
	{
		if (lines[i].length > maxval2&&lines[i].length<maxval) {
			maxval2 = lines[i].length;
			max_ind2 = i;
		}
	}
	output.push_back(max_ind2);
	for (size_t i = 0; i < lines.size(); i++)
	{
		if (lines[i].length > maxval3&&lines[i].length<maxval2) {
			maxval3 = lines[i].length;
			max_ind3 = i;
		}
	}
	output.push_back(max_ind3);
	return output;
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
	int beginpoint = px - new_cols * 2 / 3;
	//if (beginpoint + new_cols > f.rows)
	//	beginpoint = f.cols - new_cols;//
	Mat k = Mat(new_rows, new_cols, CV_8UC1);


	for (int i = 0; i < new_rows; i++) {
		for (int j = 0; j < new_cols; j++)
		{
			if (beginpoint + j > f.cols - 1 || beginpoint + j<0 || i>f.rows - 1)
				k.at<uchar>(i, j) = 128;
			else
				k.at<uchar>(i, j) = f.at<uchar>(i, beginpoint + j);
		}
	}
	return k;
}

Mat pic_cross(Mat f, Point2i pt, Vec4i a, Vec4i b) {
	Mat k;
	int margin_y = 0;
	if (pt.y > 0)
		f.copyTo(k);
	else {
		int diff = -pt.y;
		margin_y = diff + 50;
		k = Mat(f.rows + margin_y, f.cols, CV_8UC1);
		for (int i = 0; i < f.rows; i++)
			for (int j = 0; j < f.cols; j++)
				k.at<uchar>(i + margin_y, j) = f.at<uchar>(i, j);
	}
	line(k, Point(a[0], a[1] + margin_y), Point(pt.x, pt.y + margin_y), Scalar(0, 0, 255), 1, CV_AA);
	line(k, Point(b[0], b[1] + margin_y), Point(pt.x, pt.y + margin_y), Scalar(0, 0, 255), 1, CV_AA);


	return k;
}


//_linprop detect_mainline(vector<_linprop>lines) {
//	int maxval = 0;
//	for (size_t i = 0; i < lines.size(); i++)
//	{
//		if (lines[i].length > maxval)
//			maxval = i;
//	}
//	return lines[maxval];
//}

Mat color_derivative(Mat f) {
	int xs = f.rows;
	int ys = f.cols;
	Mat f_hsv;
	//Mat f_gray;
	cvtColor(f, f_hsv, CV_BGR2HSV);
	//cvtColor(f, f_gray, CV_BGR2GRAY);
	Mat output(xs, ys, CV_8UC1);
	for (int i = 0; i < xs; i++)
		for (int j = 0; j < ys; j++) {
			double rad = f_hsv.at<Vec3b>(i, j)[0];
			rad = rad * 3.141592 / 180;
			output.at<uchar>(i, j) = 255 * cos(rad);
			/*int b_lv = f.at<Vec3b>(i, j)[0];
			int g_lv = f.at<Vec3b>(i, j)[1];
			int r_lv = f.at<Vec3b>(i, j)[2];

			output.at<uchar>(i, j) = (2 * r_lv / (g_lv+1) + r_lv / (b_lv+1))*100;*/
			//output.at<uchar>(i,j) = (f_gray.at<uchar>(i, j) + f_hsv.at<Vec3b>(i, j)[0])/2;
			//output.at<uchar>(i, j) = f_hsv.at<Vec3b>(i, j)[1];
		}
	return output;
}

int main(int argc, char** argv)
{
	const char* f_name = "ex4.jpg";
	/*
	ex1 : ideal1
	ex2 : ideal2
	ex3 : 지평선 검출 안되는 경우
	ex4 : 노이즈나 일정 패턴에 의해 바르지 않은 선이 검출되는 경우
	ex5 : ex4를 인위적으로 적용되게 수정
	*/
	Mat f_src = imread(f_name);

	Mat f, f_edge, g, h, m;
	//GaussianBlur(f_src, f, Size(3,3),0,0);
	//f_src.copyTo(f);
	//medianBlur(f_src, f, 3);
	//Mat f_hue = color_derivative(f_src);
	//medianBlur(f_hue, f_hue, 3);
	Mat f_hue = color_derivative(f_src);

	//adaptiveThreshold(f_src, f_hue,255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 3, 0.5);
	//adaptiveThreshold(f_hue, f_hue, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 11, 2);
	imshow("hsv", f_hue);
	//f_hue = 255 - f_hue;

	Canny(f_hue, f_edge, 50, 200, 3);
	cvtColor(f_edge, g, CV_GRAY2BGR);
	g.copyTo(h);
	g.copyTo(m);
	vector<Vec4i> lines;
	vector<_linprop>lines_info;
	HoughLinesP(f_edge, lines, 1, CV_PI / 180, 180, 20, 10);

	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		lines_info.push_back(_linprop(l));
		line(g, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, CV_AA);
	}

	if (lines.size() > 0) {
		vector<unsigned int> sublines = detect_subline(lines_info);

		unsigned int li[3];
		for (int i = 0; i < sublines.size(); i++)
			li[i] = sublines[i];

		Vec4i l = lines[li[0]];
		line(h, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, CV_AA);

		int point_x = f.cols / 2;
		int point_y = lines_info[li[0]].y;

		Vec4i l2 = lines[li[1]];
		Vec4i l3 = lines[li[2]];
		line(m, Point(l2[0], l2[1]), Point(l2[2], l2[3]), Scalar(0, 0, 255), 1, CV_AA);
		line(m, Point(l3[0], l3[1]), Point(l3[2], l3[3]), Scalar(0, 0, 255), 1, CV_AA);
		cout << lines_info[li[1]].degree << endl << lines_info[li[2]].degree << endl << lines_info[li[0]].degree;


		int cpx = (lines_info[li[2]].n - lines_info[li[1]].n) / (lines_info[li[1]].m - lines_info[li[2]].m);
		Point2i crosspoint = Point2i(cpx, cpx*lines_info[li[2]].m + lines_info[li[2]].n);

		cout << endl << endl << "crosspoint : " << crosspoint.x << " , " << crosspoint.y;
		Mat crop_1 = pic_crop(f_src, crosspoint.x, point_y, 0);
		Mat cross_1 = pic_cross(f_src, crosspoint, l2, l3);
		//Mat crop_2 = pic_crop(f_src, point_x, point_y, 1);

		//imshow("source", f);
		imshow("detected lines", g);
		imshow("detected main line", h);
		imshow("detected sub line", m);
		//imshow("cropped1", crop_1);
		//imshow("crosspoint", cross_1);
	}


	waitKey();

	return 0;
}
