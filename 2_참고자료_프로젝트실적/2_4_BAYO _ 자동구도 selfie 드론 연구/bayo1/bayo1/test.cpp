#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/features2d/features2d.hpp"

#include <iostream>
#include <cstdlib>

using namespace cv;
int main(int argc, char *argv[])
{

	// -------------------------------------------------------------------------
	// webcam routine
	VideoCapture capture(0);

	if (!capture.isOpened()) {
		std::cerr << "Could not open camera" << std::endl;
		return 0;
	}

	// create a window
	namedWindow("webcam", 1);

	// -------------------------------------------------------------------------
	// SIFT configuration
	if (argc != 2) {
		std::cerr << "usage: ocv_cam_sift filename" << std::endl;
		return 0;
	}

	Mat db_original = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	Mat db;

	resize(db_original, db, Size(db_original.cols / 2,
		db_original.rows / 2), 0, 0, CV_INTER_NN);

	// SIFT feature detector and feature extractor
	SiftFeatureDetector detector(0.05, 5.0);
	SiftDescriptorExtractor extractor(3.0);

	// Feature detection
	std::vector<KeyPoint> kps_db;
	detector.detect(db, kps_db);

	// Feature description
	Mat dscr_db;
	extractor.compute(db, kps_db, dscr_db);

	while (true) {
		bool frame_valid = true;

		Mat frame_original;
		Mat frame;

		try {
			capture >> frame_original; // get a new frame from webcam
			resize(frame_original, frame, Size(frame_original.cols / 2,
				frame_original.rows / 2), 0, 0, CV_INTER_NN); // downsample 1/2x
		}
		catch (Exception& e) {
			std::cerr << "Exception occurred. Ignoring frame... " << e.err
				<< std::endl;
			frame_valid = false;
		}

		if (frame_valid) {
			try {
				// convert captured frame to gray scale & equalize
				Mat grayframe;
				cvtColor(frame, grayframe, CV_BGR2GRAY);
				equalizeHist(grayframe, grayframe);

				// -------------------------------------------------------------
				// face detection routine

				// keypoint detection
				std::vector<KeyPoint> kps_frame;
				detector.detect(grayframe, kps_frame);

				// keypoint description
				Mat dscr_frame;
				extractor.compute(grayframe, kps_frame, dscr_frame);

				// matching using FLANN matcher
				FlannBasedMatcher matcher;
				std::vector<DMatch> matches;
				matcher.match(dscr_db, dscr_frame, matches);

				double max_dist = 0.0, min_dist = 100.0;

				for (int i = 0; i<matches.size(); i++) {
					double dist = matches[i].distance;
					if (dist < min_dist) min_dist = dist;
					if (dist > max_dist) max_dist = dist;
				}

				// drawing only good matches (dist less than 2*min_dist)
				std::vector<DMatch> good_matches;

				for (int i = 0; i<matches.size(); i++) {
					if (matches[i].distance <= 2 * min_dist) {
						good_matches.push_back(matches[i]);
					}
				}

				Mat img_matches;
				drawMatches(db, kps_db, frame, kps_frame, good_matches,
					img_matches, Scalar::all(-1), Scalar::all(-1),
					std::vector<char>(),
					DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

				// print the output
				imshow("webcam", img_matches);

			}
			catch (Exception& e) {
				std::cerr << "Exception occurred. Ignoring frame... " << e.err
					<< std::endl;
			}
		}
		if (waitKey(30) >= 0) break;
	}

	// VideoCapture automatically deallocate camera object
	return 0;
}