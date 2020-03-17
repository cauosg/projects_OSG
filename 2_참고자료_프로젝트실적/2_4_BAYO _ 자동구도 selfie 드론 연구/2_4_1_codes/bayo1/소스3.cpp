#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <iostream>
#define FAST_TRESHHOLD 64
using namespace cv;
using namespace std;

Rect gb(0, 0, 10, 10);
bool rect_slc = false;
bool train = false;
void mouse_event(Mat f) {

}

void onMouse(int evt, int x, int y, int flags, void* param) {
	static Point2i prev(0, 0);
	static Point2i now(0, 0);

	if (evt == CV_EVENT_LBUTTONDOWN)
	{
		if (!rect_slc) {
			rect_slc = true;
			prev.x = x;
			prev.y = y;
		}
	}
	if (evt == CV_EVENT_LBUTTONUP && rect_slc) {
		rect_slc = false;
		train = true;
	}
	if (rect_slc) {
		now.x = x;
		now.y = y;
		gb = Rect(prev.x, prev.y, now.x - prev.x, now.y - prev.y);
	}

	//prev = Point2i(0, 0);

}

int main(int argc, char** argv)
{
	Mat f_src;
	Mat gray, p_gray, out, t_crop;
	vector<KeyPoint> Keys, t_Keys;
	VideoCapture cap("test.mp4");

	while (1) {
		if (waitKey(5) > 0) break;
		//imshow("ex", f_src);
		Mat f_prev;

		f_src.copyTo(f_prev);
		cap >> f_src;
		setMouseCallback("ex", onMouse);

		cvtColor(f_src, gray, COLOR_BGR2GRAY);

		medianBlur(gray, gray, 3);

		Keys.clear();
		FAST(gray, Keys, FAST_TRESHHOLD, FastFeatureDetector::TYPE_9_16);

		drawKeypoints(f_src, Keys, out, Scalar(0, 0, 255), DrawMatchesFlags::DEFAULT);
		rectangle(out, gb, Scalar(0, 255, 0));

		if (train) {
			t_crop = gray(gb);
			t_Keys.clear();
			FAST(t_crop, t_Keys, FAST_TRESHHOLD, FastFeatureDetector::TYPE_9_16);
			//t_Keys = Keys;
			train = false;
		}

		imshow("ex", out);
		//imshow("match", img_matches);
	}
	//waitKey();

	return 0;
}
