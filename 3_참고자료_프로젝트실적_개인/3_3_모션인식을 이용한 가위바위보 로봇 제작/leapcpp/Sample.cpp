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
class SampleListener : public Listener {
  public:
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
    virtual void onDeviceChange(const Controller&);
    virtual void onServiceConnect(const Controller&);
    virtual void onServiceDisconnect(const Controller&);

  private:
};

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

bool iftrain = false;

void SampleListener::onInit(const Controller& controller) {
  std::cout << "Initialized" << std::endl;
}

void SampleListener::onConnect(const Controller& controller) {
  std::cout << "Connected" << std::endl;
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
}

void SampleListener::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  std::cout << "Disconnected" << std::endl;
}

void SampleListener::onExit(const Controller& controller) {
  std::cout << "Exited" << std::endl;
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

class motionsets {
public:
	bool installed = false;
	Mat covmat;
	Mat meanmat;
	string motionname;

	motionsets(string mname) {
		motionname = mname;
		string fname = "trainingsets/" + mname + ".xml";
		FileStorage file(fname, FileStorage::READ);

		file["covmat"] >> covmat; file["mean"] >> meanmat;
		installed = true;
	}

	float calcdist(Mat gradient) {
		Mat m32;
		meanmat.convertTo(m32, CV_32F);
		return norm(gradient - m32);
	}

};


class params {
public:
	float param[5];

	std::vector<Vec3f> vs;
	params(Hand hand) {
		Arm arm = hand.arm();
		const FingerList fingers = hand.fingers();
		Mat b1, b2, b3;//basis std::vectors
		
		b2 = (Mat_<float>(1, 3) << hand.palmNormal().x, hand.palmNormal().y, hand.palmNormal().z);
		FingerList::const_iterator ip,mp;
		ip = fingers.begin();
		ip++; mp = ip; mp++;
		const Finger index = *ip;
		const Finger middle = *mp;

		Bone::Type zp = static_cast<Bone::Type>(0);
		Leap::Vector zv = middle.bone(zp).prevJoint();
		b3 = (Mat_<float>(1, 3) << zv.x - arm.wristPosition().x, zv.y - arm.wristPosition().y, zv.z - arm.wristPosition().z);
		

		normalize(b3, b3);
		//b2 = norm(b2); norm(b3);
		b1 = b2.cross(b3);

		normalize(b1, b1);
		Mat palmplane;
		palmplane.push_back(b1); palmplane.push_back(b2); palmplane.push_back(b3);
		palmplane = palmplane.t();
;

		std::vector<Vec2f> is, ms;//차원내림벡터

		for (int i = 0; i < 4; i++) {
			Bone ibone = index.bone(static_cast<Bone::Type>(i));
			Bone mbone = middle.bone(static_cast<Bone::Type>(i));

			Leap::Vector itemp = ibone.nextJoint() - ibone.prevJoint();
			Mat iv = (Mat_<float>(1, 3) << itemp.x, itemp.y, itemp.z);
			iv = iv*palmplane;

			is.push_back(Vec2f(iv.at<float>(0, 0), iv.at<float>(0, 1)));
			Leap::Vector mtemp = mbone.nextJoint() - mbone.prevJoint();
			Mat mv = (Mat_<float>(1, 3) << mtemp.x, mtemp.y, mtemp.z);
			mv = mv*palmplane;

			ms.push_back(Vec2f(mv.at<float>(0, 0), mv.at<float>(0, 1)));
		}
		float ia = 0, ma = 0;
		for (int i = 0; i < 3; i++) {
			Vec2f t1, t2;
			normalize(is[i], t1); normalize(is[i + 1], t2);
			ia += acosf(t1.dot(t2));

			normalize(ms[i], t1); normalize(ms[i + 1], t2);
			ma += acosf(t1.dot(t2));
		}

		param[0] = ia; param[1] = ma;

		Leap::Vector iev = index.bone(static_cast<Bone::Type>(3)).nextJoint();
		Leap::Vector mev = middle.bone(static_cast<Bone::Type>(3)).nextJoint();

		Mat p3v = (Mat_<float>(1, 3) << mev.x - iev.x, mev.y - iev.y, mev.z - iev.z);
		normalize(p3v, p3v);
		p3v = p3v * palmplane;

		Vec2f p2v(p3v.at<float>(0, 0), p3v.at<float>(0, 1));
		normalize(p2v, p2v);
		param[2] = acosf(Vec2f(0,1).dot(p2v));
		param[3] = zv.y;
		param[4] = sqrtf(zv.x*zv.x + zv.z*zv.z);
	}
	void showparam() {
		std::cout << "index angle sum : " << param[0] << ", middle angle sum : " << param[1] << ", index & middle angle : " << param[2] << ", height : " << param[3] << ", horizontal pos length : " << param[4] << endl;
	}
	Mat cvtMat() {
		return Mat_<float>(1, 5) << param[0], param[1], param[2], param[3], param[4];
	}
};
void classfier(params param, int fps) {
	const vector<string> sets = { "stable","run","jump" };
	const int training_sets = 30;
	const int qsize = 2;
	static deque<Mat> frames;
	static vector<motionsets> motions;
	
	if(motions.empty())
		for (int i = 0; i < sets.size(); i++) {
			motions.push_back(motionsets(sets[i]));
		}

	if (frames.size() < qsize)
	{
		frames.push_back(param.cvtMat());
	}
	else {
		frames.push_back(param.cvtMat());
		frames.pop_front();

		Mat a_set;
		for (int i = 0; i < frames.size(); i++) {
			a_set.push_back(frames[i]);
		}
		Mat sorted;
		cv::sort(a_set, sorted, CV_SORT_EVERY_COLUMN + CV_SORT_DESCENDING);

		Mat gradient = sorted.row(0) - sorted.row(sorted.rows - 1);

		Mat dists,idxes;

		for (int i = 0; i<motions.size(); i++) {
			dists.push_back(motions[i].calcdist(gradient));
		}
		//cv::sort(dists, dists, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
		cv::sortIdx(dists, idxes, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
		cout << endl << dists;
		//cout << endl << idxes;*/

		int itsclass = idxes.at<int>(0,0);
		cout << "this motion is " << sets[itsclass] << endl;
	}
}

//void classfier(params param) {
//	static deque<Mat> frames;
//	int qsize = 30;
//
//	if (frames.size() > qsize)
//		frames.pop_front();
//	frames.push_back(param.cvtMat());
//
//	Mat now_info;
//	for (int i = 0; i < qsize; i++) {
//		now_info.push_back(frames[i].t());
//	}
//	now_info = now_info.t();
//	
//
//}



bool trainer(params param, std::vector<Mat> &output, int fps, bool &trainbegin, string fname) {
	const int training_sets = 30;

	const int qsize = fps*3;
	static bool istraining = false;
	static Mat a_set;
	static int iter_num = 0;
	static Mat frames;

	if (!istraining&&trainbegin) {
		istraining = true;
		trainbegin = false;
		std::cout << "training begin" << endl;
	}

	if (istraining) {
		if (frames.rows < qsize)
		{
			frames.push_back(param.cvtMat());
		}
		else {
			cout <<"number : "<<iter_num<< ", training end" << endl;
			Mat sorted;
			for (int i = 0; i < frames.rows - 1; i++) {
				frames.row(i) = frames.row(i + 1) - frames.row(i);
			}
			frames.pop_back();
			cv::sort(frames, sorted, CV_SORT_EVERY_COLUMN + CV_SORT_DESCENDING);
			
			Mat gradient = sorted.row(0) - sorted.row(sorted.rows - 1);


			a_set.push_back(gradient);
			std::cout << gradient;
			std::cout << endl;
			iter_num++;
			frames = Mat();//여기중단점
		}
		if (iter_num > training_sets)
		{
			istraining = false;
			iter_num = 0;


			Mat covmat, meanmat;
			calcCovarMatrix(a_set, covmat, meanmat, CV_COVAR_NORMAL | CV_COVAR_ROWS);

			Mat out; out.push_back(covmat); out.push_back(meanmat);
			string str = "trainingsets/" + fname + ".xml";

			FileStorage file(str, FileStorage::WRITE);
			if (!file.isOpened())
				std::cout << "create file first" << endl;
			else {
				file << "covmat" << covmat;
				file << "mean" << meanmat;
				std::cout << "train end. it saved to " << str<<endl;
				}
			}
		return true;
		}
	else
		return false;
	return false;
}
bool check_run(Finger finger, Point3f pmax, int fps){
	static deque<Point3f> angles;
	
	//Bone::Type boneType = static_cast<Bone::Type>(b);
	//Bone bone = finger.bone(boneType);
	//std::cout << std::string(6, ' ') << boneNames[boneType]
	//	<< " bone, start: " << bone.prevJoint()
	//	<< ", end: " << bone.nextJoint()
	//	<< ", direction: " << bone.direction() << std::endl;
	std::vector<Point2f> avs;
	for (int i = 0; i < 4; i++) {
		Bone bone = finger.bone(static_cast<Bone::Type>(i));
		
		Point2f av(bone.direction().y, bone.direction().z);
		norm(av);
		avs.push_back(av);
	}
	std::vector<float> as;
	for (int i = 0; i < 3; i++) {
		as.push_back(acosf((avs[i].x * avs[i + 1].x + avs[i].y * avs[i + 1].y)));
		//as.push_back(atan2f(avs[i + 1].y - avs[i].y, avs[i + 1].x - avs[i].x));
	}
	
	Point3f a_angle(as[0], as[1], as[2]);
	if (angles.size() < fps) {
		angles.push_back(a_angle);
		return false;
	}
	std::vector<float> decs;
	std::vector<int> indec;
	angles.pop_front(); angles.push_back(a_angle);
	for (int i = 0; i < angles.size(); i++) {
		decs.push_back(sqrtf(powf(angles[i].x, 2) + powf(angles[i].y, 2) + powf(angles[i].z, 2)));
	}
	sortIdx(decs, indec, CV_SORT_DESCENDING);
	Point3f tt = angles[indec[0]] - angles[indec[decs.size() - 1]];

	//std::cout<< decs[indec[0]] <<" : "<< angles[indec[0]] <<"," << decs[indec[decs.size() - 1]] << " : " << angles[indec[decs.size() - 1]]<<endl;
	float noww = decs[indec[0]] - decs[indec[decs.size() - 1]];
	std::cout << noww << endl;
	if (noww>0.5) {
		std::cout << "its run!, speed : "<< indec[0] - indec[decs.size()-1] << endl;
		return true;
	}
	else {
		//std::cout << tt << endl;
		return false;
	}
}


void SampleListener::onFrame(const Controller& controller) {
	int fps = 30;
	if (!set_fps(fps))
		return;
  const Frame frame = controller.frame();

  HandList hands = frame.hands();
  for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
    const Hand hand = *hl;

	//params hp(hand);t
	//hp.showparam();
	std::vector<Mat> covandmean;
	
	static bool runtrain = false;
	if (iftrain)
		runtrain = true;
	if (!trainer(params(hand), covandmean, fps, runtrain, "jump"))
		classfier(params(hand), fps);
	
    const Vector normal = hand.palmNormal();
    const Vector direction = hand.direction();

    Arm arm = hand.arm();
   
    const FingerList fingers = hand.fingers();
    for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
      const Finger finger = *fl;

      for (int b = 0; b < 4; ++b) {
        Bone::Type boneType = static_cast<Bone::Type>(b);
        Bone bone = finger.bone(boneType);
      
      }
    }
  }
}

void SampleListener::onFocusGained(const Controller& controller) {
  std::cout << "Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Controller& controller) {
  std::cout << "Focus Lost" << std::endl;
}

void SampleListener::onDeviceChange(const Controller& controller) {
  std::cout << "Device Changed" << std::endl;
  const DeviceList devices = controller.devices();

  for (int i = 0; i < devices.count(); ++i) {
    std::cout << "id: " << devices[i].toString() << std::endl;
    std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
  }
}

void SampleListener::onServiceConnect(const Controller& controller) {
  std::cout << "Service Connected" << std::endl;
}

void SampleListener::onServiceDisconnect(const Controller& controller) {
  std::cout << "Service Disconnected" << std::endl;
}

void main() {
  // Create a sample listener and controller
  SampleListener listener;
  Controller controller;

  // Have the sample listener receive events from the controller
  controller.addListener(listener);

  controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);


 

  while (1) {
	  char command;
	  std::cout << "enter command" << endl;
	  cin >> command;

	  if (command == 't') {
		  command = '0';
		  iftrain = true;
		  std::cout << endl;
	  }
  }

  // Keep this process running until Enter is pressed
  std::cout << "Press Enter to quit..." << std::endl;
  std::cin.get();

  // Remove the sample listener when done
  controller.removeListener(listener);
}
