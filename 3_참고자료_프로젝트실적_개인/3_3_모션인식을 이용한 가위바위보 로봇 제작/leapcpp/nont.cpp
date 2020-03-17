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

#define framespersec 60
string f_name = "run";

const std::string fingerNames[] = { "Thumb", "Index", "Middle", "Ring", "Pinky" };
const std::string boneNames[] = { "Metacarpal", "Proximal", "Middle", "Distal" };
const std::string stateNames[] = { "STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END" };

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

class motionsets {//Ʈ���̴� �� ���� set Ŭ����
public:
	bool installed = false;//�����Ͱ� �ԷµǾ������� ���� ����
	Mat covmat;//���л� ���
	Mat meanmat;//��� ���
	string motionname;//���� �̸�

	motionsets(string mname) {//���� �̸��� �޾ƿ���, �ش� �̸��� ���� �н� set�� �����´�.
		motionname = mname;
		string fname = "trainingsets/" + mname + ".xml";
		FileStorage file(fname, FileStorage::READ);

		file["covmat"] >> covmat; file["mean"] >> meanmat;
		installed = true;
	}

	float calcdist(Mat gradient) {//�� �н� set�� ���� �̺з� �������� �Ÿ� ���� �Լ�
		Mat m32;
		meanmat.convertTo(m32, CV_32F);//ȣȯ�� ����
		return norm(gradient - m32);//������κ����� norm���� �̿��� �Ÿ� ��ȯ, ���л��� ���� ������� ����
	}

};


class params {//�Ķ���� Ŭ����
public:
	vector<float> param;//�������� 5���� float ������ ����


	inline void param_calibation() {
		param[0] = (param[0] - 0.7)/2;
		param[1] = (param[1] - 0.7)/20;
		param[2] = (param[2] - 1.5)/1.2;
		param[3] = (param[3] - 80)/100;
		param[4] = (param[4] - 10)/20;
		//param.push_back((acosf(Vec2f(0, 1).dot(p2v))-1.5)*2);//�Ķ���� 3�� ������ ������ �̷�� ���� 
		//param.push_back(zv.y/200);//�Ķ���� 4�� �չٴ� ���� ��ġ �Է�
		//param.push_back(sqrtf(zv.x*zv.x + zv.z*zv.z)/60);//�Ķ���� 5�� �չٴ� ���� �̵��� �Է�
	}

	params(Hand hand) {//�Ķ���� ��� �Լ�, �Է����� Hand Ŭ������ �����´�
		Arm arm = hand.arm();//�ո� ��ǥ�� ��� ���� arm���� ����
		const FingerList fingers = hand.fingers();//�հ��� ��� ����
		Mat b1, b2, b3;//basis ����, x,y,z

		b2 = (Mat_<float>(1, 3) << hand.palmNormal().x, hand.palmNormal().y, hand.palmNormal().z);
		//�չٴ��� ���� ���ͷ� y���� ����� ������ó�� �����Ѵ�.(Mat �� Vector �� ȣȯ�� ���� �ʱ⿡ ���� �����Ͽ� �Է�)
		
		FingerList::const_iterator ip, mp;
		ip = fingers.begin();
		ip++; mp = ip; mp++;
		const Finger index = *ip;//index : ����
		const Finger middle = *mp;//middle : ����

		Bone::Type zp = static_cast<Bone::Type>(0);
		Leap::Vector zv = middle.bone(zp).nextJoint();//nextjoint�� �ؾ��� �Ǽ�!
		b3 = (Mat_<float>(1, 3) << zv.x - arm.wristPosition().x, zv.y - arm.wristPosition().y, zv.z - arm.wristPosition().z);
		//������ ù ��° ���� ������ �о����, z���� ����� �������͸� ���Ѵ�.

		normalize(b3, b3);
		b1 = -b2.cross(b3);
		//b2�� �̹� unit�����̸�, b3�� unit���ͷ� ��ȯ���ش�. ������ �� ���� �������ʹ� �� ������ �������� ���� �� �ִ�.
		
		normalize(b1, b1);
		//������ ���͸� unit ���ͷ� ��ȯ

		Mat palmplane;
		palmplane.push_back(b1); palmplane.push_back(b2); palmplane.push_back(b3);
		palmplane = palmplane.t();
		//3���� �������͸� �̿��� �չٴ� ��ǥ��� ��ȯ��ų ����� ���Ѵ�.

		std::vector<Vec2f> index_2d, middle_2d;//�հ��� �� Bone���� ������������ ����

		for (int i = 0; i < 4; i++) {//���͸� ���ϰ� �չٴ� ��ǥ��� ��ȯ�ϸ�, ������ �����.

			Bone ibone = index.bone(static_cast<Bone::Type>(i));//������ i��° Bone ���� ȹ��
			Bone mbone = middle.bone(static_cast<Bone::Type>(i));//������ i��° Bone ���� ȹ��

			//����
			Leap::Vector itemp = ibone.nextJoint() - ibone.prevJoint();
			//�������� ���۰����� ��ǥ ���̸� ���� ���� ȹ��, ���� 3���� ������ ����

			Mat iv = (Mat_<float>(1, 3) << itemp.x, itemp.y, itemp.z);//Mat ������ ��ȯ
			iv = iv*palmplane;//�չٴ� ��ǥ��� ���ͷ� ��ȯ
			
			index_2d.push_back(Vec2f(iv.at<float>(0, 1), iv.at<float>(0, 2)));
			//�չٴ� ��ǥ�� �� x,y��������� ���翵�� ���� 2���� ���ͷ� ������ �����. �� �� ���������� ������ �Է�!
			//y,z������� �ؾ� ����! �����ʿ�!


			//����, ����� ����!
			Leap::Vector mtemp = mbone.nextJoint() - mbone.prevJoint();
			Mat mv = (Mat_<float>(1, 3) << mtemp.x, mtemp.y, mtemp.z);
			mv = mv*palmplane;
			
			middle_2d.push_back(Vec2f(mv.at<float>(0, 0), mv.at<float>(0, 1)));
		}

		float ia = 0, ma = 0;//���� �� ���� ����
		for (int i = 0; i < 3; i++) {//����� ���͵��� �̷�� ������ ���ϰ�, �� ���� ����Ѵ�.
			Vec2f t1, t2;
			normalize(index_2d[i], t1); normalize(index_2d[i + 1], t2);
			//���͵��� unit ���ͷ� �����. t1:���� ����, t2 : ��������

			ia += acosf(t1.dot(t2));//������ �ִ� �� unit������ �������� ���ڻ��� ������� ������ �˾Ƴ���, ���Ѵ�.

			normalize(middle_2d[i], t1); normalize(middle_2d[i + 1], t2);
			ma += acosf(t1.dot(t2));//������ ���, ��� ����!
		}

		param.push_back(ia); //�Ķ���� 1�� ���� ������ �� �Է�
		param.push_back(ma); //�Ķ���� 2�� ���� ������ �� �Է�

		Leap::Vector iev = index.bone(static_cast<Bone::Type>(3)).nextJoint();//������ �� ��ǥ
		Leap::Vector mev = middle.bone(static_cast<Bone::Type>(3)).nextJoint();//������ �� ��ǥ

		Mat p3v = (Mat_<float>(1, 3) << mev.x - iev.x, mev.y - iev.y, mev.z - iev.z);//�� ��ǥ�� �̷�� ���͸� Mat���� ��ȯ
		normalize(p3v, p3v);//unit���ͷ� ��ȯ, ��� normalize �� �ʿ� ����!
		p3v = p3v * palmplane;//�չٴ� ��ǥ��� ��ȯ

		Vec2f p2v(p3v.at<float>(0, 0), p3v.at<float>(0, 1));//�չٴ� ��ǥ�� �� x,y��������� ���翵�� ���� ���� ����,
		normalize(p2v, p2v);//���� ���� ���� ����ȭ
		param.push_back(acosf(Vec2f(0, 1).dot(p2v)));//�Ķ���� 3�� ������ ������ �̷�� ���� 
		param.push_back(zv.y);//�Ķ���� 4�� �չٴ� ���� ��ġ �Է�
		param.push_back(sqrtf(zv.x*zv.x + zv.z*zv.z));//�Ķ���� 5�� �չٴ� ���� �̵��� �Է�

		param_calibation();
	}

	void showparam() {//�Ķ���� ����Լ�
		std::cout << "index angle sum : " << param[0] << ", middle angle sum : " 
			<< param[1] << ", index & middle angle : " << param[2] << ", height : " << param[3] 
			<< ", horizontal pos length : " << param[4] << endl;
	}

	Mat cvtMat() {//Mat������ ��ȯ�Լ�, ũ�� 5�� ���͸� ��������.
		return Mat_<float>(1, 5) << param[0], param[1], param[2], param[3], param[4];
	}

};

void frameshow(int shownum) {
	static bool isrun = false;

	static int prev = 0;
	static int showing = 0;
	vector<string> vnames = { "anims/1.mp4", "anims/2.mp4", "anims/3.mp4","anims/4.mp4" ,"anims/5.mp4" ,"anims/6.mp4" };
	static VideoCapture cap;
	if (shownum != prev)
	{
		isrun = true;
		cap = VideoCapture(vnames[shownum]);
		showing = shownum;
	}
	if (isrun) {
		Mat frame;
		cap >> frame;
		if (frame.empty()) {
			isrun = false;
			cap = VideoCapture(vnames[showing]);
			return;
		}
		imshow("animation", frame);
		waitKey(1);
	}
	prev = shownum;
}


void classfier(params param, int fps) {//param : ���� �����ӿ� ���� �Ķ���� Ŭ���� , fps : �ʴ� ������ ��
	const vector<string> sets = { "stable","run","jump","left","right","stop" };// ���� set���� �̸���
	const int qsize = 30;//ť�� ũ��
	static deque<Mat> frames;//frames : ť ������ ��� ����, ���� ������ ���� ���� 30������ ������ ������ ��� �ִ�.
	static vector<motionsets> motions;//motions : ���� set���� Ŭ����

	if (motions.empty())//���� set���� �����Ͱ� ������� ���
		for (int i = 0; i < sets.size(); i++) {
			motions.push_back(motionsets(sets[i]));//sets[i]�� �ش��ϴ� �̸��� ���� ���� set���� �����͸� �ҷ��´�.
		}

	if (frames.size() < qsize)//ť�� ũ�Ⱑ 30���� ���� ���
	{
		frames.push_back(param.cvtMat());//pop����� ������ �ʰ� push���� �����Ѵ�.
	}
	else {
		frames.push_back(param.cvtMat());
		frames.pop_front();
		//ť�� 30���� ���ų� Ŭ ��� ���� ������ �����ӿ� ���� ������ �����ϰ� ���� �������� ������ �߰��Ѵ�.


		Mat a_set;
		for (int i = 0; i < frames.size(); i++) {
			a_set.push_back(frames[i]);
		}
		//30�� �����ӵ��� �Ķ���͵��� a_set���� ������ a_set�� 5 x 31 ũ���� ����� �ȴ�.
		Mat sorted;
		cv::sort(a_set, sorted, CV_SORT_EVERY_COLUMN + CV_SORT_DESCENDING);
		//�ּ� �ִ밪�� �˱� ���� ������������ ����, ����� sorted�� �Էµ�
		Mat gradient = sorted.row(0) - sorted.row(sorted.rows - 1);

		float mean_axis = 0;
		for (int i = 0; i < a_set.rows; i++) {
			mean_axis += a_set.row(i).at<float>(0, 2);
		}
		mean_axis /= a_set.rows;
		gradient.at<float>(0, 2) = mean_axis;
		//cout << endl <<gradient;
		//�ֻ�����(�ִ�) - ��������(�ּҰ�)�� ���翡���� ��ȭ�� ���ͷ� ����
		//�Ķ���� 5���� ��ȭ�� ������ ���� 1 x 5 ���
		Mat dists, idxes;

		for (int i = 0; i<motions.size(); i++) {
			dists.push_back(motions[i].calcdist(gradient));
		}//��� ������ŭ, �ش� ���� set�� �߽����� ���� ��ȭ�� ���Ϳ��� ���̸� ���Ѵ�.

		cv::sortIdx(dists, idxes, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
		//���̰� ���� ���� ���� set�� �ε����� ��� ���� ������������ ����

		//ù ��°�� �ִ� ���� �з��Ⱑ �Ǵ��� �� �ٰŸ� ��������, �ش� ���� �������� �Ǵ��Ѵ�.
		int itsclass = idxes.at<int>(0, 0);
	//	cout << "this motion is " << sets[itsclass] << endl;
		frameshow(itsclass);//�Ǵ��� ������ �ִϸ��̼� ����
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

	const int qsize = fps*2;//3�� ������ �����͸� �н��Ѵ�.
	static bool istraining = false;//�н� ������ ���¸� ǥ���ϴ� ����
	static Mat gradients;//��ȭ�������� �����ϴ� ���
	static int iter_num = 0;//�н� �ݺ� Ƚ��
	static Mat frames;

	if (!istraining&&trainbegin) {//�н� ����� �������� ��
		istraining = true;//�н� �� ������ �������� ����
		trainbegin = false;//��� ������ ������� �ǵ�����
		std::cout << "training begin" << endl;
	}

	if (istraining) {//�н� �� ������ ������ ���� ����
		if (frames.rows < qsize)
		{
			frames.push_back(param.cvtMat());
		}
		else {
			cout << "number : " << iter_num << ", training end" << endl;
			Mat sorted;
			cv::sort(frames, sorted, CV_SORT_EVERY_COLUMN + CV_SORT_DESCENDING);
			Mat gradient = sorted.row(0) - sorted.row(sorted.rows - 1);
			
			float mean_axis = 0;
			for (int i = 0; i < frames.rows; i++) {
				mean_axis += frames.row(i).at<float>(0, 2);
			}
			mean_axis /= frames.rows;
			gradient.at<float>(0, 2) = mean_axis;
			gradients.push_back(gradient);//��ȭ���� ����

			//���� ��ȭ�� ���
			std::cout << gradient;
			std::cout << endl;
			iter_num++;
			frames = Mat();//�����ߴ���
		}
		if (iter_num > training_sets)//31�� �н����� ���
		{
			Mat covmat, meanmat;//���л�, ��� ���
			calcCovarMatrix(gradients, covmat, meanmat, CV_COVAR_NORMAL | CV_COVAR_ROWS);
			//��ȭ�� ������ �̿��Ͽ� ���л�� �����İ� ���

			string str = "trainingsets/" + fname + ".xml";//���� �̸� ����

			FileStorage file(str, FileStorage::WRITE);
			if (!file.isOpened())//���� �� ���
				std::cout << "create file first" << endl;
			else {
				file << "covmat" << covmat;//���л� ����
				file << "mean" << meanmat;//��� ����
				file << "trainingsets" << gradients;//�̿��� �н� �����͵��� ����
				std::cout << "train end. it saved to " << str;

				istraining = false;//�н� �������·� ��ȯ
				iter_num = 0;//�н� �ݺ����� �ʱ�ȭ
				gradients = Mat();//��ȭ�� ���� ���� �ʱ�ȭ
			}
		}
		return true;//�н� ���� �� true�� ����, �н� ������ ���
	}
	return false;//�н� ���� �ƴ� �� false�� ����, �н� ���� �ƴ��� ���
}
bool check_run(Finger finger, Point3f pmax, int fps) {
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
		std::cout << "its run!, speed : " << indec[0] - indec[decs.size() - 1] << endl;
		return true;
	}
	else {
		//std::cout << tt << endl;
		return false;
	}
}

/*params hp(hand);
hp.showparam();*/

void SampleListener::onFrame(const Controller& controller) {
	int fps = framespersec;
	if (!set_fps(fps))
		return;
	const Frame frame = controller.frame();

	HandList hands = frame.hands();
	for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
		const Hand hand = *hl;

	/*	params hp(hand);
		hp.showparam();*/

		std::vector<Mat> covandmean;

		static bool runtrain = false;
		if (iftrain)
			runtrain = true;
		//�������� iftrain�� �����н� �Լ��� �н������ ���������� üũ
		if (!trainer(params(hand), covandmean, fps, runtrain, f_name)) {
		//if ��ȣ �ȿ� �ִ� ���� �н� �Լ�, �� ���̶� �н������ ������ Ʈ���̴��� ���� �� ���� true���� ��ȯ
		//�� �̿��� ��쿡��  false���� ��ȯ�ϹǷ� �н��� ���� �ʰ� �ִ� ��� classfier�Լ��� �����ϰ� �˴ϴ�.
			classfier(params(hand), fps);
		//�н��� �̷������ �ʰ� �ִ� ��� �з��⸦ �����մϴ�.
		}
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
