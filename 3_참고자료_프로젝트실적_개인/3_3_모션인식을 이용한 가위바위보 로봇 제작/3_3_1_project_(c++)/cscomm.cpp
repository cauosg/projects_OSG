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
#include <fstream>
#include <cstring>
#include "Leap.h"
#include <time.h>

#include <stdio.h>
#include <windows.h>
#define BUFFER_SIZE 1024
#pragma comment (lib,"ws2_32.lib")

SOCKET   ClientSocket;
SOCKADDR_IN  ToServer;   SOCKADDR_IN  FromServer;

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
HANDLE hDevice = CreateFile(L"COM5", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);

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

class motionsets {//트레이닝 된 동작 set 클래스
public:
	bool installed = false;//데이터가 입력되었는지에 대한 변수
	Mat covmat;//공분산 행렬
	Mat meanmat;//평균 행렬
	string motionname;//동작 이름

	motionsets(string mname) {//동작 이름을 받아오고, 해당 이름을 가진 학습 set을 가져온다.
		motionname = mname;
		string fname = "trainingsets/" + mname + ".xml";
		FileStorage file(fname, FileStorage::READ);

		file["covmat"] >> covmat; file["mean"] >> meanmat;
		installed = true;
	}

	float calcdist(Mat gradient) {//본 학습 set에 대한 미분류 데이터의 거리 측정 함수
		Mat m32;
		meanmat.convertTo(m32, CV_32F);//호환성 조정
		return norm(gradient - m32);//평균으로부터의 norm값을 이용해 거리 반환, 공분산은 현재 사용하지 않음
	}

};


class params {//파라미터 클래스
public:
	vector<float> param;//성분으로 5개의 float 변수를 가짐


	inline void param_calibation() {
		param[0] = (param[0] - 0.7) / 2;
		param[1] = (param[1] - 0.7) / 20;
		param[2] = (param[2] - 1.5) / 1.2;
		param[3] = (param[3] - 80) / 100;
		param[4] = (param[4] - 10) / 20;
		//param.push_back((acosf(Vec2f(0, 1).dot(p2v))-1.5)*2);//파라미터 3에 검지와 중지가 이루는 각도 
		//param.push_back(zv.y/200);//파라미터 4에 손바닥 수직 위치 입력
		//param.push_back(sqrtf(zv.x*zv.x + zv.z*zv.z)/60);//파라미터 5에 손바닥 수평 이동량 입력
	}

	params(Hand hand) {//파라미터 계산 함수, 입력으로 Hand 클래스를 가져온다
		Arm arm = hand.arm();//손목 좌표를 얻기 위해 arm변수 지정
		const FingerList fingers = hand.fingers();//손가락 목록 변수
		Mat b1, b2, b3;//basis 벡터, x,y,z

		b2 = (Mat_<float>(1, 3) << hand.palmNormal().x, hand.palmNormal().y, hand.palmNormal().z);
		//손바닥의 방향 벡터로 y축을 대신할 기저벡처로 설정한다.(Mat 과 Vector 간 호환이 되지 않기에 직접 접근하여 입력)

		FingerList::const_iterator ip, mp;
		ip = fingers.begin();
		ip++; mp = ip; mp++;
		const Finger index = *ip;//index : 검지
		const Finger middle = *mp;//middle : 중지

		Bone::Type zp = static_cast<Bone::Type>(0);
		Leap::Vector zv = middle.bone(zp).nextJoint();//nextjoint로 해야함 실수!
		b3 = (Mat_<float>(1, 3) << zv.x - arm.wristPosition().x, zv.y - arm.wristPosition().y, zv.z - arm.wristPosition().z);
		//중지의 첫 번째 관절 정보를 읽어오고, z축을 대신할 기저벡터를 구한다.

		normalize(b3, b3);
		b1 = -b2.cross(b3);
		//b2는 이미 unit벡터이며, b3는 unit벡터로 변환해준다. 나머지 한 축의 기저벡터는 두 벡터의 외적으로 구할 수 있다.

		normalize(b1, b1);
		//구해진 벡터를 unit 벡터로 변환

		Mat palmplane;
		palmplane.push_back(b1); palmplane.push_back(b2); palmplane.push_back(b3);
		palmplane = palmplane.t();
		//3개의 기저벡터를 이용해 손바닥 좌표계로 변환시킬 행렬을 구한다.

		std::vector<Vec2f> index_2d, middle_2d;//손가락 내 Bone들의 차원내림벡터 집합

		for (int i = 0; i < 4; i++) {//벡터를 구하고 손바닥 좌표계로 변환하며, 차원을 낮춘다.

			Bone ibone = index.bone(static_cast<Bone::Type>(i));//검지의 i번째 Bone 정보 획득
			Bone mbone = middle.bone(static_cast<Bone::Type>(i));//중지의 i번째 Bone 정보 획득

																 //검지
			Leap::Vector itemp = ibone.nextJoint() - ibone.prevJoint();
			//끝관절과 시작관절의 좌표 차이를 통해 벡터 획득, 현재 3차원 벡터인 상태

			Mat iv = (Mat_<float>(1, 3) << itemp.x, itemp.y, itemp.z);//Mat 변수로 변환
			iv = iv*palmplane;//손바닥 좌표계로 벡터로 변환

			index_2d.push_back(Vec2f(iv.at<float>(0, 1), iv.at<float>(0, 2)));
			//손바닥 좌표계 내 x,y평면으로의 정사영을 통해 2차원 벡터로 차원을 낮춘다. 그 후 집합으로의 데이터 입력!
			//y,z평면으로 해야 했음! 수정필요!


			//중지, 방식은 동일!
			Leap::Vector mtemp = mbone.nextJoint() - mbone.prevJoint();
			Mat mv = (Mat_<float>(1, 3) << mtemp.x, mtemp.y, mtemp.z);
			mv = mv*palmplane;

			middle_2d.push_back(Vec2f(mv.at<float>(0, 0), mv.at<float>(0, 1)));
		}

		float ia = 0, ma = 0;//각도 합 저장 변수
		for (int i = 0; i < 3; i++) {//연결된 벡터들이 이루는 각도를 구하고, 그 합을 계산한다.
			Vec2f t1, t2;
			normalize(index_2d[i], t1); normalize(index_2d[i + 1], t2);
			//벡터들을 unit 벡터로 만든다. t1:이전 마디, t2 : 다음마디

			ia += acosf(t1.dot(t2));//검지에 있는 두 unit벡터의 내적값을 역코사인 계산으로 각도를 알아내고, 더한다.

			normalize(middle_2d[i], t1); normalize(middle_2d[i + 1], t2);
			ma += acosf(t1.dot(t2));//중지의 경우, 방식 동일!
		}

		param.push_back(ia); //파라미터 1에 검지 각도의 합 입력
		param.push_back(ma); //파라미터 2에 중지 각도의 합 입력

		Leap::Vector iev = index.bone(static_cast<Bone::Type>(3)).nextJoint();//검지의 끝 좌표
		Leap::Vector mev = middle.bone(static_cast<Bone::Type>(3)).nextJoint();//중지의 끝 좌표

		Mat p3v = (Mat_<float>(1, 3) << mev.x - iev.x, mev.y - iev.y, mev.z - iev.z);//두 좌표가 이루는 벡터를 Mat으로 변환
		normalize(p3v, p3v);//unit벡터로 변환, 사실 normalize 할 필요 없음!
		p3v = p3v * palmplane;//손바닥 좌표계로 변환

		Vec2f p2v(p3v.at<float>(0, 0), p3v.at<float>(0, 1));//손바닥 좌표계 내 x,y평면으로의 정사영을 통해 차원 내림,
		normalize(p2v, p2v);//차원 내림 벡터 정규화
		param.push_back(acosf(Vec2f(0, 1).dot(p2v)));//파라미터 3에 검지와 중지가 이루는 각도 
		param.push_back(zv.y);//파라미터 4에 손바닥 수직 위치 입력
		param.push_back(sqrtf(zv.x*zv.x + zv.z*zv.z));//파라미터 5에 손바닥 수평 이동량 입력

		param_calibation();
	}

	void showparam() {//파라미터 출력함수
		std::cout << "index angle sum : " << param[0] << ", middle angle sum : "
			<< param[1] << ", index & middle angle : " << param[2] << ", height : " << param[3]
			<< ", horizontal pos length : " << param[4] << endl;
	}

	Mat cvtMat() {//Mat으로의 변환함수, 크기 5의 벡터를 내보낸다.
		return Mat_<float>(1, 5) << param[0], param[1], param[2], param[3], param[4];
	}

};

void frameshow(int shownum) {
	static bool isrun = false;
		
	static int prev = 0;
	static int showing = 0;
	vector<string> vnames = { "anims/1.mp4", "anims/2.mp4", "anims/3.mp4","anims/4.mp4" ,"anims/5.mp4" ,"anims/6.mp4" };

	if (shownum != prev)
	{
		isrun = true;
		
		showing = shownum;
	}
	char sbuffer[BUFFER_SIZE] = { "0" };
	sbuffer[0] = '0' + shownum;

	//------------------- 패킷송신 (26바이트의 알파벳대문자들~)
	int Send_Size = sendto(ClientSocket, sbuffer, 26, 0,
		(struct sockaddr*) &ToServer, sizeof(ToServer));
	cout << "data sent" << sbuffer << endl;

	if (isrun) {

	}
	prev = shownum;
}


void classfier(params param, int fps) {//param : 현재 프레임에 대한 파라미터 클래스 , fps : 초당 프레임 수
	const vector<string> sets = { "r,s,p" };// 동작 set들의 이름들
	const int qsize = 30;//큐의 크기
	static deque<Mat> frames;//frames : 큐 형태의 행렬 변수, 현재 프레임 기준 과거 30프레임 동안의 정보를 담고 있다.
	static vector<motionsets> motions;//motions : 동작 set들의 클래스
	static int prev_state = 0;
	if (motions.empty())//동작 set들의 데이터가 비어있을 경우
		for (int i = 0; i < sets.size(); i++) {
			motions.push_back(motionsets(sets[i]));//sets[i]에 해당하는 이름을 가진 동작 set들의 데이터를 불러온다.
		}

	if (frames.size() < qsize)//큐의 크기가 30보다 작을 경우
	{
		frames.push_back(param.cvtMat());//pop명령은 내리지 않고 push만을 시행한다.
	}
	else {
		frames.push_back(param.cvtMat());
		frames.pop_front();
		//큐의 30보다 같거나 클 경우 가장 오래된 프레임에 대한 정보를 제거하고 현재 프레임의 정보를 추가한다.


		Mat a_set;
		for (int i = 0; i < frames.size(); i++) {
			a_set.push_back(frames[i]);
		}
		//30개 프레임들의 파라미터들을 a_set으로 모은다 a_set은 5 x 31 크기의 행렬이 된다.
		Mat sorted;
		cv::sort(a_set, sorted, CV_SORT_EVERY_COLUMN + CV_SORT_DESCENDING);
		//최소 최대값을 알기 위해 내림차순으로 정리, 결과는 sorted에 입력됨
		Mat gradient = sorted.row(0) - sorted.row(sorted.rows - 1);

		float mean_axis = 0;
		for (int i = 0; i < a_set.rows; i++) {
			mean_axis += a_set.row(i).at<float>(0, 2);
		}
		mean_axis /= a_set.rows;
		gradient.at<float>(0, 2) = mean_axis;
		//cout << endl <<gradient;
		//최상위값(최댓값) - 최하위값(최소값)을 현재에서의 변화량 벡터로 지정
		//파라미터 5개의 변화량 정보를 담은 1 x 5 행렬
		Mat dists, idxes;

		for (int i = 0; i<motions.size(); i++) {
			dists.push_back(motions[i].calcdist(gradient));
		}//모션 갯수만큼, 해당 동작 set의 중심점과 현재 변화량 벡터와의 차이를 구한다.

		cv::sortIdx(dists, idxes, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
		//차이가 가장 작은 동작 set의 인덱스를 얻기 위해 오름차순으로 정렬

		//첫 번째에 있는 값이 분류기가 판단한 최 근거리 집합으로, 해당 현재 동작으로 판단한다.
		int itsclass = idxes.at<int>(0, 0);
		//	cout << "this motion is " << sets[itsclass] << endl;
		frameshow(itsclass);//판단한 동작의 애니메이션 실행
		if (prev_state != itsclass) {
			DWORD btsI;
			WriteFile(hDevice, sets[itsclass], 9, &btsI, NULL);
		}
		prev_state = itsclass;
	}
}

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

	WSADATA   wsaData;

	//SOCKET   ClientSocket;
	//SOCKADDR_IN  ToServer;   SOCKADDR_IN  FromServer;

	int   FromServer_Size;
	int   Recv_Size;   int   Send_Size;

	char   Buffer[BUFFER_SIZE] = { "Message~" };
	USHORT   ServerPort = 9050;

	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
	{
		printf("WinSock error");
		WSACleanup();
		//exit(0);
	}

	memset(&ToServer, 0, sizeof(ToServer));
	memset(&FromServer, 0, sizeof(FromServer));

	ToServer.sin_family = AF_INET;
	ToServer.sin_addr.s_addr = htonl(INADDR_LOOPBACK);//inet_addr("127.0.0.1");
	ToServer.sin_port = htons(ServerPort); // 포트번호

	ClientSocket = socket(AF_INET, SOCK_DGRAM, 0);//

	if (ClientSocket == INVALID_SOCKET)
	{
		printf("lost socket");
		closesocket(ClientSocket);
		WSACleanup();
		//exit(0);
	}
	//##########################################################################

	// Create a sample listener and controller
	SampleListener listener;
	Controller controller;

	// Have the sample listener receive events from the controller
	controller.addListener(listener);

	controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

	//serial통신
	
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		printf("Port opened! \n");
		DCB lpTest;
		GetCommState(hDevice, &lpTest);
		lpTest.BaudRate = CBR_9600;
		lpTest.ByteSize = 8;
		lpTest.Parity = NOPARITY;
		lpTest.StopBits = ONESTOPBIT;
		SetCommState(hDevice, &lpTest);

		DWORD btsIO;

		WriteFile(hDevice, "083150090", 9, &btsIO, NULL);
	}


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

	closesocket(ClientSocket); //소켓을 닫습니다.
	WSACleanup();
	CloseHandle(hDevice);
}
