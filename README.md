1-Line Description
직접 작성한 script, code만을 참고자료에 첨부하였습니다.

1.	참고자료_연구실적
1.1	Facial Expression by Regional Weight with Approximated Q learning
1.1.1	주요 MATLAB script, main: ‘ex_f_ck_norm_wmargi_512_for_profile.m’
1.1.2	action 선택에 따른 Grid Map의 변화, 23rd에서 98.47% by 10-fold
1.1.3	저널 accept 과정 중의 comments & replies + minor comments
1.1.4	사용한 모든 scripts로써, 주요 scripts는 1.1.2로 이미 분리했습니다.
1.1.5	게재 논문, 2020.02.17, published in MDPI, 교신저자 외 단독저자 SCI-E paper

1.2	Simple Real time Facial Expression Recognition for Interactive Arts
1.2.1	주요 MATLAB script, 4개의 case별 script
1.2.2	작성한 모든 scripts
1.2.3	게재 논문, 2019.08.09, published in TECHART, 4장 분량의 short paper

1.3	Objective Saliency and Facial Models for Face Detection in Super Resolution
1.3.1	사용한 모든 scripts로써, 각 cpp가 독자적 프로그램입니다
1.3.2	주요 cpp 스크립트로써, opencv 3.4.1의 환경이 필요합니다.
1.3.3	코딩 내용을 MATLAB으로 옮겨 비교했습니다. OpenCV가 더욱 빨랐습니다.
1.3.4	2019.01.16 국제 학술대회 ICGHIT2019 에서의 구두 발표 paper

2.	참고자료_프로젝트실적
2.1	CFT_AR역사체험
2.1.1	App 실행 녹화본, 광화문에서 진행했으며 premiere로 컷편집을 담당했습니다.
2.1.2	프로젝트 최종 발표자료
2.1.3	프로젝트 최종 보고서
2.1.4	BP의 nodes 중 제가 작성한 부분입니다. ARPlayerPawn BP에 위치합니다.

2.2	VR애니메이션 제작
2.2.1	MAYA->UE4 fbx 포팅 과정에서 발생한 대표적 문제들과 해결한 부분의 녹화
A.	bone 구조가 분리되는 문제입니다. 
B.	bone구조를 해결했으나 날개 회전 모션의 중간 frame이 skip됐습니다.
C.	캐릭터와 카트를 묶고, frame skip을 motionbuilder로 해결했습니다.
D.	모든 fbx가 오류없이 import된 모습입니다. 당시 UV는 작업중이었습니다

2.2.2	모션 캡쳐 데이터 리타겟팅을 담당, advanced skeleton 리타겟을 이용했습니다.
A.	reference rigged 캐릭터에 MAYA로 리타겟팅 후 UE4에 import한 모습
B.	custom 캐릭터에 리타겟팅 후 손가락 모션을 추가해 import했습니다.

2.2.3	일부 animating을 맡았습니다. 모델들은 팀원의 custom rigged 캐릭터입니다.
A.	가장 기본적인 걷는 모션입니다. 키포인트를 잡고 looping했습니다.
B.	놀라는 모션입니다. bone별 keypoint를 잡고 편집점을 A와 연결했습니다.
C.	물체를 추가하고 skinweight를 부여해 일체감을 주었습니다.

2.2.4	사운드 작업 내용입니다. 요약 pdf와 FLstudio 작업파일(.flp)파일이 있습니다.
2.2.5	에디터에서의 녹화본입니다. VR용이므로 수동으로 카메라를 조작했습니다.

2.3	CFT_면(面); 감성인식을 이용한 interactive art
2.3.1	전시에서 실행한 MATLAB 스크립트입니다.
2.3.2	설문 데이터, Google, wechat을 이용했으며, 데이터 분석을 담당했습니다.
2.3.3	최종ppt로써, 발표 및 ppt 제작, GAN을 제외한 전반적인 개발에 기여했습니다.
2.3.4	1분 36초의 최종발표에 사용된 영상으로, 영상편집을 담당했습니다.
2.3.5	최종보고서로, 팀내 사정으로 인해 2시간만에 작성된 점 양해 바랍니다.

2.4	BAYO_자동구도 selfie 드론 연구
2.4.1	cpp로 개발한 프로젝트로써, 전체 코딩을 담당했습니다. main: streaming.cpp
2.4.2	성능 비교로 MATLAB에서 같은 기능을 구현했으나, OpenCV가 우수했습니다.
2.4.3	본교 대회에 출품한 포스터입니다. 대회 결과 우수상을 수여받았습니다.
2.4.4	최종 보고서로, 기술 부분 작성을 담당했습니다.
2.4.5	LINC사업단 창업동아리 최종발표 ppt로, 기술부분 발표를 담당했습니다.
2.4.6	sample영상들에 대한 지평선, saliency 검출 테스트 녹화입니다.

2.5	TAYO_모션인식을 이용한 핑거보드 게임 개발
2.5.1	동작 인식 프로젝트입니다. 결과는 local sever를 통해 unity3D로 전달됩니다
	main: cscomm.cpp (학습 완료 상태에서 동작을 인식, 서버 통신)//중요 line에 주석 포함
	학습 데이터는 trainingsets 폴더에 위치, 직접 데이터를 취득했으며, Sample.cpp로 학습
2.5.2	제가 알고 있는 부분을 팀원들께 전달해주기 위한 몇 가지 설명 자료입니다.
2.5.3	경진대회 당시 발표 포스터입니다. 대회 결과 우수상을 수여받았습니다.
2.5.4	최종 보고서이며, 기술 부분 작성을 담당했습니다. 
2.5.5	최종 발표 ppt이며, 제작과 발표를 담당했습니다.
2.5.6	발표 중 촬영본으로, 위가 투명한 상자를 제작해 leapmotion을 설치했습니다.

3.	참고자료_프로젝트실적_개인
3.1	UE4를 이용한 리듬게임 제작
3.1.1	UE4 프로젝트, 간단한 빌드가 필요합니다. 플레이하실 경우 폴더 내 readme를 확인바랍니다.
3.1.2	Play 녹화로, 직접 플레이, 자동 플레이, 커스텀 모드 3가지가 있습니다.
3.1.3	발표 ppt로, 모티브 게임, 게임 설명, BP캡쳐 및 설명, 사운드 작업 등의 내용이 있습니다.

3.2	Unity3D를 이용한 PVE 게임 제작
3.2.1	Unity3D프로젝트 폴더로, 인게임과 스크립트는 하위 2개 항목을 참고 부탁드립니다.
3.2.2	핵심 스크립트들입니다. 각 파일 최상단에 기능 설명 주석이 있습니다.
3.2.3	데미지 계산을 비활성화한 인게임 녹화입니다.
3.2.4	1페이지로 요약한 게임 정보입니다. 조작법, 시스템에 대한 설명이 있습니다.
3.3	모션인식을 이용한 가위바위보 로봇 제작
3.3.1	2.5의 코드를 일부 수정해, 가위바위보 모션을 학습하고 Arduino와의 통신을 추가했습니다.
3.3.2	포맥스, 스테핑 모터, Arduino를 이용한 로봇 제작 과정 사진들입니다.
3.3.3	전시 촬영본입니다. 로봇과의 가위바위보는 항상 유저가 승리하도록 설계됐습니다.

3.4	OpenGL을 이용한 단편 애니메이션 제작
3.4.1	OpenGL로 애니메이션을 구현한 cpp 프로젝트입니다.
3.4.2	카메라 및 환경을 컨트롤해 녹화하고, 편집을 추가한 영상입니다.
3.4.3	모델링 구현 방법, 쉐이더 설계, 라이팅 방법 등의 설명이 포함되어 있습니다.

3.5	Direct2D를 이용한 게임 제작
3.5.1	Direct를 이용해 애니메이션을 구현한 cpp 프로젝트입니다.
3.5.2	_tcpServerPAcketSol.exe을 먼저 실행하고 _PrisonEscape.exe를 실행해 시작합니다.
3.5.3	게임 조작법, 시스템, 인게임 캡쳐 등의 자료가 포함되어 있습니다.

