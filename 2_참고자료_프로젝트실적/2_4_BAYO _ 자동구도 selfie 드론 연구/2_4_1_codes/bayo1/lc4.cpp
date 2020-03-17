#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <fstream>
#include <time.h>
#include <deque>


using namespace cv;
using namespace std;
#define inf 99999999
#define PI 3.141592
class lin_props {//라인들의 정보를 담기 위한 클래스
public:
	int cluster = 0;//현재 라인이 속해있는 클러스터
	Point2f bp;//라인의 시작점
	Point2f ep;//인의 끝점
	Point2f vec;//ep - bp 로 정의되는 라인의 벡터
	float length;//라인의 길이
	float angle;// 라인의 x축과 이루는 각도

	lin_props(Vec4i lines) {//클래스 초기화 함수
		bp = Point2i(lines.val[0], lines.val[1]);//시작점 획득
		ep = Point2i(lines.val[2], lines.val[3]);//끝점 획득
		vec = Point2i(lines.val[2] - lines.val[0], lines.val[3] - lines.val[1]);//벡터 획득
		length = sqrtf((lines.val[0] - lines.val[2])*(lines.val[0] - lines.val[2]) + (lines.val[1] - lines.val[3])*(lines.val[1] - lines.val[3]));//라인의 길이 획득
		angle = atan2f(ep.y - bp.y, ep.x - bp.x);//역탄젠트를 이용하여 라인의 각도 획득
	}

	lin_props(Point2f a, Point2f b) {//클래스 초기 함수, 위의 함수와 오버로딩된 함수
		bp = a;
		ep = b;
	}


	float dot_ang(lin_props in) {//입력되는 라인 in 과의 사인값 반환
		float cosn = (vec.x * in.vec.x + vec.y * in.vec.y) / (length * in.length);//내적을 통해 코사인값 획득
		return sqrtf(1 - cosn*cosn);//사인 변환식, 이 식을 이용했기 때문에 출력되는 sin의 범위는 범위는 sin(0 ~ 0.5phi) 
	}

	int sub(lin_props in, int vert_weight) {//입력되는 라인 in과의 최단 거리 구함
		int d[4];
		d[0] = (in.bp.x - bp.x)*(in.bp.x - bp.x) + vert_weight*(in.bp.y - bp.y)*(in.bp.y - bp.y);//1과 2의 시작점 간의 거리
		d[1] = (in.bp.x - ep.x)*(in.bp.x - ep.x) + vert_weight*(in.bp.y - ep.y)*(in.bp.y - ep.y);//1의 시작점과 2의 끝점간의 거리
		d[2] = (in.ep.x - bp.x)*(in.ep.x - bp.x) + vert_weight*(in.ep.y - bp.y)*(in.ep.y - bp.y);//1의 끝점과 2의 시작점간의 거리
		d[3] = (in.ep.x - ep.x)*(in.ep.x - ep.x) + vert_weight*(in.ep.y - ep.y)*(in.ep.y - ep.y);//1과 2의 끝점 간의 거리
		int min_val = inf;
		int ind = 0;
		for (int i = 0; i < 4; i++) {
			if (d[i] < min_val)
				min_val = d[i];
		}//4개의 거리들 중 최솟값을 획득한다.

		return min_val;
	}
};


class present_line {//라인 클러스터의 정보를 담는 클래스
public:
	int cluster = 0;//현재 클러스터의 번호
	vector<Point2f> bps;//라인의 시작점들의 집합
	vector<Point2f> eps;//라인의 끝점들의 집합

	//클러스터의 대표직선에 관한 변수!
	Point2f bp;//대표직선의 시작점
	Point2f ep;//대표직선의 끝점
	float length;//대표직선의 길이
	//vector<Vec4f> pline;

	float weight;//클러스터 내 라인들의 수평 weight 총 합
	present_line(int cluster_order) {//클러스터가 선언되었을 때 번호 부여 및 초기화
		cluster = cluster_order;
		weight = 0;
		length = 0;
	}

	void add_line(lin_props props) {//클러스터로의 라인 추가
		bps.push_back(props.bp);//시작점 집합에 입력 라인 시작점 추가
		eps.push_back(props.ep);//끝점 집합에 입력 라인 끝점 추가
		weight += (1.57 - abs(props.angle));//입력 라인의 수평 weight를 추가, 수평 weight는 라인이 수평에 가까울 수록 높아짐! 1.57 : 1/2 phi (90도, 라인이 이룰 수 있는 최대 각도값), props.angle : 입력 라인의 각도
		length += props.length;//입력 라인의 길이 weight를 추가
	}

	lin_props descion_line() {//대표직선1 출력함수, 대표직선은 클러스터 내의 평균 라인이 된다.
		Point2f bpt(0, 0), ept(0, 0);
		for (int i = 0; i < bps.size(); i++) {
			bpt += bps[i];
			ept += eps[i];
		}
		bpt /= (float)bps.size();//시작점들의 평균
		ept /= (float)eps.size();//끝점들의 평균

		float midx = bpt.x + ept.x;//클러스터의 중심점
		bpt.x = midx / 2 - length / 2;
		ept.x = midx / 2 + length / 2;
		return lin_props(bpt, ept);//라인 식을 출력한다.
	}

	lin_props descion_line2() {//대표직선2 출력함수, 대표직선은 클러스터 내의 평균 직선의 연장선이 된다.
		float m = 0, c = 0;
		float bsize = bps.size();//bsize는 클러스터내 시작 점들의 갯수 = 클러스터 내의 라인들의 갯수
		for (int i = 0; i < bsize; i++) {//모든 라인들의 1차 직선 방정식을 구한다. y = mx + c에서 m값과 c값을 구함
			float mtotal = (eps[i].y - bps[i].y + 0.001) / (eps[i].x - bps[i].x + 0.001);//m값, y변화량/x변화량
			m += mtotal;
			c += eps[i].y - eps[i].x*mtotal;//c값 계산
		}
		m /= bsize;//m의 평균값 계산
		c /= bsize;//c의 평균값 게산
		//c -= 360;
		return lin_props(Point2f(0, c), Point2f(640, m * 640 + c));// 화면 테두리와의 교점을 구함으로써 직선 식을 출력한다.
	}
};

inline void draw_linprops(Mat &f, vector<lin_props> props, int clusters) {//props 내에 존재하는 모든 라인들을 그린다.
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
inline void draw_linprops(Mat &f, lin_props a_line) {//한 개의 라인을 그린다.
	Vec4i l(a_line.bp.x, a_line.bp.y, a_line.ep.x, a_line.ep.y);
	line(f, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 0), 3, CV_AA);
}

//편의 상 Matlab과 같은 문법을 쓰기 위해 만든 인라인 함수들
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

//---------------------
inline void remove_pad(Mat &f, int padsize) {//가우시안 필터 처리를 할 때 패딩을 하지 않기 때문에(처리 속도 향샹을 위해) 패딩되지 않아 간섭이 발생한 이미지의 부분을 제거한다.
	Rect roi(padsize - 1, padsize - 1, 640 - padsize * 2, 360 - padsize * 2);
	f = f(roi);//f내 roi의 영역만큼을 크롭한다.
	resize(f, 640, 360);
}

inline void normalize(Mat &f) {//f를 unit matrix로의 변환
	normalize(f, f, 0, 1, CV_MINMAX);
}



vector<lin_props> lines_NN(vector<Vec4i> lines, float dist_treshold, float uc_weight, int vert_weight, int& num_of_cluster) {//라인 분류를 위한 최근접 이웃 알고리즘
	vector<lin_props> props;//라인 클래스의 벡터 배열
	int cluster_index = 0;//검출된 들러스터의 갯수

	for (int i = 0; i < lines.size(); i++) {
		props.push_back(lin_props(lines[i]));//모든 Vec4i 형의 라인 변수들을 lin_props 클래스로 변하여 저장한다.
	}
	for (int i = 0; i < props.size(); i++) {//라인들의 갯수만큼 반복
		lin_props now = props[i];//현재 라인
		float min_dist = inf;//최솟값
		int min_index = 0;//최소값을 가지는 라인의 인덱스
		for (int j = 0; j < props.size(); j++) {//현재 라인과 나머지 모든 라인들에 대해서 실행
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
	int limit = 1000;
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

	//if (abs(out[0].bp.y)>limit || abs(out[0].ep.y > limit))
	//	return false;
	int maxsize = 5;
	int ppsize = pls.size();
	if (ppsize > maxsize)
		ppsize = maxsize;

	p_line = out[indeces[0]];
	if (abs(atan2f(p_line.ep.y - p_line.bp.y, p_line.ep.x - p_line.bp.x)) >PI / 6)
		return false;

	plines.clear();
	for (int i = 0; i < ppsize; i++)
		plines.push_back(out[i]);
	//plines = out;
	return true;
}


bool line_cluster(Mat &f, Mat edges, vector<lin_props> &props) {
	vector<Vec4i> lines;
	HoughLinesP(edges, lines, 1, CV_PI / 180.00, 10, 30, 10);

	if (lines.empty())
		return false;


	RNG rng(12345);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(f, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 2, CV_AA);
	}

	int clusters = 0;
	props = lines_NN(lines, 500, 0.5, 1, clusters);

	RNG rng(12345);
	for (size_t i = 0; i < props.size(); i++)
	{

		Vec4i l = Vec4i(props[i].bp.x, props[i].bp.y, props[i].ep.x, props[i].ep.y);
		line(f, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 2, CV_AA);
	}
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
		else if (a_pline.bp.y>1 && a_pline.ep.y > 1) {
			plines.pop_front();
			plines.push_back(a_pline);
		}

	Point2f bp(0, 0), ep(0, 0); Point3f sd(0, 0, 0);

	for (int i = 0; i < psize; i++) {
		bp += plines[i].bp / psize;
		ep += plines[i].ep / psize;
	}
	line(f, bp, ep, Scalar(0, 255, 0), 2);
	rectangle(f, bp, ep, Scalar(0, 255, 0), 1);
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
void draw_vanishing(Mat &f, vector<lin_props> plines) {
	RNG rng(12345);
	for (size_t i = 0; i < plines.size(); i++)
	{

		Vec4i l = Vec4i(plines[i].bp.x, plines[i].bp.y, plines[i].ep.x, plines[i].ep.y);
		line(f, Point(l[.0], l[1]), Point(l[2], l[3]), Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 2, CV_AA);
	}
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
		rad_m += radius[i];
	}

	cp = Point3f(center_m.x, center_m.y, rad_m);
	return true;
}


bool set_fps(int fps) {
	static bool swi = false;
	static clock_t prevtime = clock();

	int t_delay = 1000 / fps;

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

bool saliency(Mat f_src, vector<Mat> cfilter, Point3f &cp) {
	Mat f, fd, fg, fc;
	vector<Mat> bgry;

	f_src.copyTo(f);
	int orgx = f.cols, orgy = f.rows;

	resize(f, f, Size(100, 100));
	f.convertTo(fd, CV_32FC3);

	split(fd, bgry);

	cvtColor(f, fg, CV_BGR2GRAY);
	//Canny(fg, fc, 16,40);
	fc.convertTo(fc, CV_32F);

	Mat m_rg = (bgry[0] / 2 + bgry[1] / 2);
	Mat m_by = (bgry[2] / 2 + m_rg / 2);

	Mat fred = bgry[2]; Mat fgreen = bgry[1]; Mat fblue = bgry[0];
	im2double(fred); im2double(fgreen); im2double(fblue);

	Mat out = Mat::ones(100, 100, CV_32F);

	for (int i = 0; i < 5; i++) {
		Mat g_r = bandpassfilter(fred, cfilter[i]);
		Mat g_g = bandpassfilter(fgreen, cfilter[i]);
		Mat g_b = bandpassfilter(fblue, cfilter[i]);

		Mat f_gray = (g_r + g_g + g_b) / 3 + 0.01;

		Mat kout = abs(0.375*fred + 0.375*fgreen - 0.5*fblue);
		Mat ns = Mat::ones(100, 100, CV_32F);
		Mat diffmap = ns.mul(1 / f_gray);
		kout = kout.mul(diffmap);

		out = out.mul(kout);
	}
	normalize(out);

	Mat toshow;
	threshold(out * 255, toshow, 128, 255, cv::THRESH_BINARY);

	resize(toshow, toshow, Size(orgx, orgy));
	resize(out, out, Size(orgx, orgy));

	imshow("saliency", out);
	regionprops(toshow, cp);
	return true;
}

void main()
{

	int padsize = 20;
	int iter_nums = 30;
	Mat bp30 = load_a_filter("bp30", 360);
	vector<Mat> bp10 = load_filters("cfilters/c_filter", 100);
	Mat orgf;
	VideoCapture cap("test3.mp4");//9,10,11,15
	cap >> orgf;
	int rows = orgf.rows;
	int cols = orgf.cols;
	
	while (waitKey(10) != 27) {
		//if (set_fps(30)) 
		{

			Mat f_gray, f_src, out_props, out_van, out_sal;
			cap >> f_src;
			


			if (f_src.empty())
				continue;

			resize(f_src, 640, 360);
			f_src.copyTo(out_props);
			f_src.copyTo(out_van);
			f_src.copyTo(out_sal);
			cvtColor(f_src, f_gray, CV_RGB2GRAY);
			im2double(f_gray);

			Mat f_freq10 = bandpassfilter(f_gray, bp30);
			im2uint8(f_freq10);
			remove_pad(f_freq10, padsize);
			Mat edges;
			float s = 1.5;//sensitivity;
			Canny(f_freq10, edges, 16 * s, 40 * s);//16,40



			remove_pad(f_src, padsize);
			vector<lin_props> props;

			if (line_cluster(edges, props)); {
				lin_props pline(Vec4i(0, 0, 0, 0));
				vector<lin_props> plines;
				if (lines_training(props, pline, plines)) {
					draw_props(out_props, pline, iter_nums);
					draw_vanishing(out_van, plines);
				}
			}

			Point3f sd;
			if (saliency(f_src, bp10, sd))
				draw_sds(out_sal, sd, iter_nums);

			saliency(f_src, bp10, sd);
			//resize(out, out, Size(cols, rows));
			imshow("horizontal line", out_props);
			imshow("vanishing lines", out_van);
			imshow("saliency location", out_sal);
		}
		/*else {
		Mat dummy;
		cap >> dummy;
		}*/
	}

}