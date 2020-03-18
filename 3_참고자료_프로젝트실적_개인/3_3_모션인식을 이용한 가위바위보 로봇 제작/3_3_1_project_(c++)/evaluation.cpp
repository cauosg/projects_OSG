/******************************************************************************\
* Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml.hpp"
#include <deque>
#include <iostream>
#include <cstring>
#include "Leap.h"
#include <time.h>
using namespace Leap;
using namespace cv;
using namespace std;
using namespace ml;

class motionsets {
public:
	bool installed = false;
	Mat covmat;
	Mat meanmat;
	Mat trainingsets;
	string motionname;

	motionsets(string mname) {
		motionname = mname;
		string fname = "trainingsets/" + mname + ".xml";
		FileStorage file(fname, FileStorage::READ);

		file["covmat"] >> covmat; file["mean"] >> meanmat;
		file["trainingsets"] >> trainingsets;
		installed = true;
	}

	float calcdist(Mat gradient) {
		Mat m32;
		meanmat.convertTo(m32, CV_32F);
		return norm(gradient - m32);
	}

};

void evaluator(int fps) {
	const vector<string> sets = { "stable","run","jump","left", "right", "stop" };

	const int training_sets = 30;
	const int qsize = 30;
	static deque<Mat> frames;
	static vector<motionsets> motions;

	if (motions.empty())
		for (int i = 0; i < sets.size(); i++) {
			motions.push_back(motionsets(sets[i]));
		}

	cout << "evaluation begin" << endl;
	for (int q = 0; q < motions.size(); q++) {
		Mat dists;
		for (int p = 0; p < motions[q].trainingsets.rows; p++) {

			Mat distsrow, idxes;

			for (int i = 0; i < motions.size(); i++) {
				distsrow.push_back(motions[i].calcdist(motions[q].trainingsets.row(p)));
			}
			distsrow = distsrow;
			//cv::sort(dists, dists, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
			cv::sortIdx(distsrow, idxes, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
			/*cout << endl << dists;
			cout << endl << idxes;*/
			float itsclass = idxes.at<int>(0, 0);
			distsrow.push_back(itsclass);
			dists.push_back(distsrow.t());
			/*FileStorage file(str, FileStorage::WRITE);
			if (!file.isOpened())
			std::cout << "create file first" << endl;
			else {
			file << "covmat" << covmat;
			file << "mean" << meanmat;
			std::cout << "train end. it saved to " << str;*/
		}
		string fname = "evals/" + sets[q] + "_evaluation.xml";
		FileStorage file(fname, FileStorage::WRITE);
		if (!file.isOpened())
			std::cout << "create file first" << endl;
		else {
			file << "evaluations" << dists.t();
			std::cout << "train end. it saved to " << fname<<endl;
		}
	}
}

void main() {
	// Create a sample listener and controller

	evaluator(60);

	// Keep this process running until Enter is pressed
	std::cout << "Press Enter to quit..." << std::endl;
	std::cin.get();

}
