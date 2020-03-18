#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <ddraw.h>
#include <dsound.h>
#include <stdio.h>

#include "dsutil.h"
#include "ddutil.h"
int sendshape = 1;
extern void CommMain(int argc, char **argv);
extern void CommRecv(char *recvData);
extern void comsend();

int mq = 1;
int iq = 1;
void init();
long FAR PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND MainHwnd;
int itscene = 1;
RECT score[11];
bool windsound = false;
bool startsound = false;
bool oversound = false;
bool bgmsound = false;
extern int gamemode = 0;
int recvmode = 0;
struct globalctr{
	int flow = 0;
	double velocity = 1;
	double gravity = 0.02;
	double MAXSPEED = 5;
	int MAXOBJ = 5;
	int hp[2];
	int mp[2];
	int score;
	int LEVEL = 4;

	int posX2p = 0;
	int posY2p = 0;
	int score2p = 0;
	int player2 = 0;
	double velocity2p = 1;

	int p2scene = 1;
	int p1scene = 1;
}; globalctr Ginfo;



struct OBJ{

	int posX = 20 * (rand() % 22) + 20;
	int posY = 20 * (rand() % 28) + 20;

	double velocity = 1;
	bool droping = false;
	bool shapeisset = false;
	bool breaking = false;
	RECT nowShape;
	int sendshape = 1;
	int scene = 1;
	bool breakable = false;

	bool flag;
};
OBJ heroobj;
OBJ heroobj2p;
OBJ stone[10];

RECT	BackRect = { 0, 0, 480, 800 };
RECT	MapRect = { 0, 0, 480, 800 };
RECT	stoneRect = { 50, 50, 100, 100 };
RECT	typestone[6];
RECT	heroRect = { 0, 0, 200, 150 };
RECT    heroRect2p = { 0, 0, 200, 150 };

RECT sceneChar[20];


LPDIRECTDRAW         DirectOBJ;
LPDIRECTDRAWSURFACE  RealScreen;
LPDIRECTDRAWSURFACE  BackScreen;
LPDIRECTDRAWCLIPPER	 ClipScreen;

LPDIRECTDRAWSURFACE  MAPBACK;
LPDIRECTDRAWSURFACE  SpriteImage;
LPDIRECTDRAWSURFACE  overimage;
LPDIRECTDRAWSURFACE  hpImage;
LPDIRECTDRAWSURFACE  Spritestone;
LPDIRECTDRAWSURFACE  startIMG;
LPDIRECTDRAWSURFACE  BackImage;
LPDIRECTDRAWSURFACE  heroImage;

LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;
HSNDOBJ Sound[10];


double MouseX = 100;
double MouseY = 100;


int Nscore = 0;


BOOL _InitDirectSound(void)
{
	if (DirectSoundCreate(NULL, &SoundOBJ, NULL) == DS_OK)
	{
		if (SoundOBJ->SetCooperativeLevel(MainHwnd, DSSCL_PRIORITY) != DS_OK) return FALSE;

		memset(&DSB_desc, 0, sizeof(DSBUFFERDESC));
		DSB_desc.dwSize = sizeof(DSBUFFERDESC);
		DSB_desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

		if (SoundOBJ->CreateSoundBuffer(&DSB_desc, &SoundDSB, NULL) != DS_OK) return FALSE;
		SoundDSB->SetVolume(0);
		SoundDSB->SetPan(0);
		return TRUE;
	}
	return FALSE;
}


BOOL Fail(HWND hwnd, char *Output)
{
	ShowWindow(hwnd, SW_HIDE);
	MessageBox(hwnd, Output, "Game Programming", MB_OK);
	DestroyWindow(hwnd);
	return FALSE;
}

void _ReleaseAll(void)
{
	if (DirectOBJ != NULL)
	{

		if (BackScreen != NULL)
		{
			BackScreen->Release();
			BackScreen = NULL;
		}
		if (RealScreen != NULL)
		{
			RealScreen->Release();
			RealScreen = NULL;
		}
		if (SpriteImage != NULL)
		{
			SpriteImage->Release();
			SpriteImage = NULL;
		}

		DirectOBJ->Release();
		DirectOBJ = NULL;
	}
}


BOOL _GameMode(HINSTANCE hInstance, int nCmdShow, DWORD  x, DWORD  y, DWORD  bpp, int FullScreen)
{
	WNDCLASS wc;
	DDSURFACEDESC ddsd;
	DDSCAPS ddscaps;
	LPDIRECTDRAW pdd;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockBrush(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "GameProg";
	RegisterClass(&wc);

	if (FullScreen){
		MainHwnd = CreateWindowEx(
			0, "GameProg", NULL, WS_POPUP, 0, 0,
			GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			NULL, NULL, hInstance, NULL);
	}
	else{
		MainHwnd = CreateWindow("GameProg", "Prison Escape by OSG",
			WS_OVERLAPPEDWINDOW, 0, 0, x, y, NULL, NULL, hInstance, NULL);
	}
	if (!MainHwnd) return FALSE;


	// 다이렉트 드로우(DD) 생성
	if (FAILED(DirectDrawCreate(NULL, &pdd, NULL)))
		return Fail(MainHwnd, "DirectDrawCreate");
	// DD에 연결
	if (FAILED(pdd->QueryInterface(IID_IDirectDraw, (LPVOID *)&DirectOBJ)))
		return Fail(MainHwnd, "QueryInterface");

	// 윈도우 핸들의 협력 단계를 설정한다.
	if (FullScreen){
		if (FAILED(DirectOBJ->SetCooperativeLevel(MainHwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN)))
			return Fail(MainHwnd, "SetCooperativeLevel");
		// Set full screen display mode
		if (FAILED(DirectOBJ->SetDisplayMode(x, y, bpp)))
			return Fail(MainHwnd, "SetDisplayMode");

		memset(&ddsd, 0, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;
		if (FAILED(DirectOBJ->CreateSurface(&ddsd, &RealScreen, NULL)))
			return Fail(MainHwnd, "CreateSurface");

		memset(&ddscaps, 0, sizeof(ddscaps));
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		if (FAILED(RealScreen->GetAttachedSurface(&ddscaps, &BackScreen)))
			return Fail(MainHwnd, "GetAttachedSurface");
	}
	else{
		if (FAILED(DirectOBJ->SetCooperativeLevel(MainHwnd, DDSCL_NORMAL)))
			return Fail(MainHwnd, "SetCooperativeLevel");

		memset(&ddsd, 0, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		ddsd.dwBackBufferCount = 0;
		if (FAILED(DirectOBJ->CreateSurface(&ddsd, &RealScreen, NULL)))
			return Fail(MainHwnd, "CreateSurface");

		memset(&ddsd, 0, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwWidth = x;
		ddsd.dwHeight = y;
		if (FAILED(DirectOBJ->CreateSurface(&ddsd, &BackScreen, NULL)))
			return Fail(MainHwnd, "CreateSurface");

		if (FAILED(DirectOBJ->CreateClipper(0, &ClipScreen, NULL)))
			return Fail(MainHwnd, "CreateClipper");

		if (FAILED(ClipScreen->SetHWnd(0, MainHwnd)))
			return Fail(MainHwnd, "SetHWnd");

		if (FAILED(RealScreen->SetClipper(ClipScreen)))
			return Fail(MainHwnd, "SetClipper");

		SetWindowPos(MainHwnd, NULL, 0, 0, x, y, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	SetFocus(MainHwnd);
	ShowWindow(MainHwnd, nCmdShow);
	UpdateWindow(MainHwnd);
	ShowCursor(TRUE);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Implmentation Here




void heroscene(RECT	&hero, OBJ &character){
	static bool animate = false;
	static int num = 1;

	character.scene = (Ginfo.velocity - 1) * 16 / (Ginfo.MAXSPEED - 1);


	if (Ginfo.mp[0] < 128 && character.scene>15)
		Ginfo.mp[0] += Ginfo.LEVEL;
	else if (Ginfo.mp[0] > 127)
		character.breakable = true;


	

	if (num < 5)
		num++;
	else
		num = 0;

	if (animate&&num == 0)
		animate = false;
	else if (num == 0)
		animate = true;

	if (character.scene>14 && !character.breakable){
		if (!windsound && heroobj.scene > 12)
		{

			SndObjPlay(Sound[6], NULL);
			windsound = true;
		}
		else if (heroobj.scene < 10){
			windsound = false;
		}
		character.scene = 14;
	}		

	if (character.scene == 16 && animate)
		character.scene = 15;

	if (character.scene == 14 && animate)
		character.scene = 13;

	Ginfo.p1scene = character.scene;

	hero = sceneChar[character.scene];
	if (character.breakable)
		hero = sceneChar[character.scene+2];
}

void stonebreak(OBJ &stone){

	static int nowscene = 3;
	static int num = 1;

	if (num < 3)
		num++;
	else
		num = 0;

	if (nowscene < 5 && num == 0)
		nowscene++;
	else  if (num == 0){
		stone.breaking = false;
		nowscene = 3;
	}
	stone.nowShape = typestone[nowscene];

}


void scoreview(int posX, int posY, int scoreN){

	int scoreposX = posX;
	int scoreposY = posY;

	int orgscoreN = scoreN;

	int q100000 = scoreN / 100000;
	scoreN -= q100000 * 100000;
	int q10000 = scoreN / 10000;
	scoreN -= q10000 * 10000;
	int q1000 = scoreN / 1000;
	scoreN -= q1000 * 1000;
	int q100 = scoreN / 100;
	scoreN -= q100 * 100;
	int q10 = scoreN / 10;
	scoreN -= q10 * 10;
	int q1 = scoreN;


	BackScreen->BltFast(scoreposX, scoreposY, hpImage, &score[q100000], DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
	scoreposX += 13;
	BackScreen->BltFast(scoreposX, scoreposY, hpImage, &score[q10000], DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
	scoreposX += 13;
	BackScreen->BltFast(scoreposX, scoreposY, hpImage, &score[q1000], DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
	scoreposX += 13;
	BackScreen->BltFast(scoreposX, scoreposY, hpImage, &score[q100], DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
	scoreposX += 13;
	BackScreen->BltFast(scoreposX, scoreposY, hpImage, &score[q10], DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
	scoreposX += 13;
	BackScreen->BltFast(scoreposX, scoreposY, hpImage, &score[q1], DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);

}

void gameover(int FullScreen){
	//static int itscene = 0;
	static int num = 1;
	RECT	BackRect = { 0, 0, 480, 800 };
	RECT scoresprite = { 0, 26, 52, 39 };
	RECT scorepos = { 128, 385, 180, 398 };
	//RECT Rect1 = { heroobj.posX, heroobj.posY, heroobj.posX + 120, heroobj.posX + 160 };

	SndObjStop(Sound[0]);
	BackScreen->BltFast(0, 0, overimage, &BackRect, DDBLTFAST_WAIT | NULL);


	if (num < 5)
		num++;
	else
		num = 0;
	if (itscene < 11 && num == 0)
		itscene++;

	heroRect = { 60 * itscene, 160, 60 + 60 * itscene, 220 };

	//BackScreen->Blt(&scorepos, hpImage, &scoresprite, DDBLT_WAIT, NULL);
	BackScreen->BltFast(180, 385, hpImage, &scoresprite, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
	BackScreen->BltFast(heroobj.posX, heroobj.posY, heroImage, &heroRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);

	scoreview(250, 385,Ginfo.score);

	if (FullScreen)
		RealScreen->Flip(NULL, DDFLIP_WAIT);
	else{
		RECT WinRect;
		RECT Rect = { 0, 0, 480, 800 };

		GetWindowRect(MainHwnd, &WinRect);
		//BackScreen->Blt(&Rect1, heroImage, &heroRect, DDBLTFAST_WAIT ,DDBLTFAST_SRCCOLORKEY);

		RealScreen->Blt(&WinRect, BackScreen, &Rect, DDBLT_WAIT, NULL);
	}

}

void interaction(){
	//static bool breakable = false;
	Ginfo.score += Ginfo.velocity*Ginfo.LEVEL;
	for (int i = 0; i < Ginfo.MAXOBJ; i++)
	{

		if (stone[i].shapeisset == false){
			stone[i].sendshape = rand() % 3;
			stone[i].nowShape = typestone[stone[i].sendshape];
			stone[i].shapeisset = true;
		}

		if (stone[i].droping == false && abs(heroobj.posX - stone[i].posX - 30) < 50 && abs(heroobj.posY - stone[i].posY - 30) < 20){
			stone[i].droping = true;

			if (Ginfo.velocity >1)
				Ginfo.velocity -= 0.5;
			SndObjPlay(Sound[3], NULL);

			if (heroobj.scene>12 && heroobj.breakable){
				stone[i].breaking = true;
				heroobj.breakable = false;
				Ginfo.mp[0] = 18;
				Ginfo.score += 300 * Ginfo.LEVEL;
				SndObjPlay(Sound[7], NULL);
			}
			else if (heroobj.scene >= 12)	//(Ginfo.velocity >= Ginfo.MAXSPEED - 0.5)
			{
				Ginfo.hp[0] -= 10 - pow(Ginfo.MAXSPEED - Ginfo.velocity, 2);
				SndObjPlay(Sound[4], NULL);
			}
		}

		if (stone[i].breaking)
			stonebreak(stone[i]);

		if (stone[i].droping){
			stone[i].posY += pow(stone[i].velocity, 2);//Ginfo.velocity/Ginfo.MAXSPEED*
			stone[i].velocity++;

		}


		if (stone[i].posY > 1300 || stone[i].posY <0)
		{
			stone[i].posX = (20 * (rand() % 20)) + 20;
			stone[i].posY = 1200;//(20 * (rand() % 38)) + 20;
			stone[i].velocity = 1;
			stone[i].droping = false;
			stone[i].shapeisset = false;
		}
		BackScreen->BltFast(stone[i].posX, stone[i].posY, Spritestone, &stone[i].nowShape, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
		stone[i].posY -= pow(Ginfo.velocity, 2);
	}

}

void p2control(){	
	RECT hp2p = { 0, 0, Ginfo.hp[1], 13 };
	RECT mp2p = { 0, 39, Ginfo.mp[1], 52 };

	heroobj2p.posX = Ginfo.posX2p;
	heroobj2p.posY = Ginfo.posY2p;

	heroscene(heroRect2p,heroobj2p);
	//BackScreen->BltFast(30, 30, SpriteImage, &stoneRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
	BackScreen->BltFast(heroobj2p.posX, heroobj2p.posY, heroImage, &heroRect2p, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);

	scoreview(320, 100, Ginfo.score2p);

	heroRect2p = sceneChar[Ginfo.p2scene];

	BackScreen->BltFast(320, 50, hpImage, &hp2p, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
	BackScreen->BltFast(320, 75, hpImage, &mp2p, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);


}
void mainmode(int FullScreen){
	
	if (Ginfo.score > 50000){
		Ginfo.LEVEL = 3.75;//4
		MapRect = { 1920, 0, 2400, 800 };
	}
	else if (Ginfo.score > 20000){
		Ginfo.LEVEL = 3;//4
	MapRect = { 1440, 0, 1920, 800 };
}
	else if (Ginfo.score > 10000){
		Ginfo.LEVEL = 2.5;//3
		MapRect = { 960, 0, 1440, 800 };
	}
	else if (Ginfo.score > 3000){
		Ginfo.LEVEL = 2;//2
		MapRect = { 480, 0, 960, 800 };
	}
	else
		MapRect = { 0, 0, 480, 800 };

	Ginfo.MAXOBJ = Ginfo.velocity + Ginfo.LEVEL;
	Ginfo.MAXSPEED = Ginfo.LEVEL + 3;

	RECT hp1p = { 0, 0, Ginfo.hp[0], 13 };
	RECT mp1p = { 0, 39, Ginfo.mp[0], 52 };

	srand(rand());
	BackScreen->BltFast(0, 0, BackImage, &BackRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);


	if (Ginfo.velocity < Ginfo.MAXSPEED)
		Ginfo.velocity += Ginfo.gravity;
	Ginfo.flow += pow(Ginfo.velocity, 2);
	if (Ginfo.flow < 801){
		MapRect.top = Ginfo.flow;
		MapRect.bottom = Ginfo.flow + 800;
	}
	else
	{
		Ginfo.flow = 0;
		MapRect.top = Ginfo.flow;
		MapRect.bottom = Ginfo.flow + 800;
	}
	//1276
	BackScreen->BltFast(0, 0, BackImage, &MapRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);

	//
	if (MouseX>60 && MouseX<420)
		heroobj.posX = MouseX - 30;
	if (MouseY>60 && MouseY<740)
		heroobj.posY = MouseY - 30;
	heroscene(heroRect,heroobj);
	//BackScreen->BltFast(30, 30, SpriteImage, &stoneRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
	BackScreen->BltFast(heroobj.posX, heroobj.posY, heroImage, &heroRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);



	interaction();

	scoreview(50, 100, Ginfo.score);

	BackScreen->BltFast(50, 50, hpImage, &hp1p, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
	BackScreen->BltFast(50, 75, hpImage, &mp1p, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);

	if (Ginfo.player2==2)
		p2control();

	if (FullScreen)
		RealScreen->Flip(NULL, DDFLIP_WAIT);
	else{
		RECT WinRect;
		RECT Rect = { 0, 0, 480, 800 };

		GetWindowRect(MainHwnd, &WinRect);
		RealScreen->Blt(&WinRect, BackScreen, &Rect, DDBLT_WAIT, NULL);
	}

	if (Ginfo.hp[0] < 15)		
		gamemode = 2; itscene = 1;
		

}



void Startmode(int FullScreen)
{
	RECT	BackRect = { 0, 0, 480, 800 };
	static double counter = 1;
	// Clear Back Ground

	BackScreen->BltFast(0, 0, MAPBACK, &BackRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);

	BackScreen->BltFast(0, 0, startIMG, &BackRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);

	Ginfo.velocity += Ginfo.gravity;
	if (Ginfo.velocity>3)
		heroobj.posY += pow(counter, 2) / 2500;
	counter++;

	if (heroobj.posY > 800)
		counter = 1;


	heroscene(heroRect,heroobj);
	//BackScreen->BltFast(30, 30, SpriteImage, &stoneRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
	BackScreen->BltFast(300, heroobj.posY, heroImage, &heroRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);


	if (FullScreen)
		RealScreen->Flip(NULL, DDFLIP_WAIT);
	else{
		RECT WinRect;
		RECT Rect = { 0, 0, 480, 800 };

		GetWindowRect(MainHwnd, &WinRect);

		RealScreen->Blt(&WinRect, BackScreen, &Rect, DDBLT_WAIT, NULL);

	}



}

/*void soundFunc(bool mode[]){
	for (int i = 0; i < 3; i++)
	if (!mode[i]){
		mode[i] = true;
		for (int j = 0; j < 3; j++)
		if (i != j)
			mode[i] = false;
	}
}*/

void _GameProc(int FullScreen)
{
	static int temp = 0;
	//Startmode(0); return;
	/*if (recvmode != temp){
		gamemode = recvmode;
		init();
	}*/comsend();
	temp = recvmode;
	switch (gamemode){
	case 0: Startmode(0); 
		Ginfo.score = 0;
		if (!startsound){
			SndObjStop(Sound[0]);
			SndObjPlay(Sound[0], NULL);
			SndObjPlay(Sound[2], NULL);
			startsound = true;
			bgmsound = false;
		windsound = false;
		oversound = false;
		}
		break;
	case 1: mainmode(0);
		if (!bgmsound){
			SndObjPlay(Sound[0], DSBPLAY_LOOPING);
	
		startsound = false;
		oversound = false;
		}
		break;
	case 2: gameover(0); 
		if (!oversound){
			SndObjStop(Sound[0]); 
			SndObjPlay(Sound[1], NULL);
			oversound = true;
		
		windsound = false;
		startsound = false;
		}
		break;
	default: break;
	}

}


long FAR PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
		MouseX = LOWORD(lParam);
		MouseY = HIWORD(lParam);
		break;

	case WM_LBUTTONDOWN:

		break;

	case WM_DESTROY:  _ReleaseAll();
		PostQuitMessage(0);
		break;

	case WM_TIMER:

		_GameProc(0);

		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
		case VK_F12:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			return 0;


		case VK_LEFT:
			return 0;

		case VK_RIGHT:
			return 0;

		case VK_UP:

			return 0;

		case VK_DOWN:
			return 0;


		case VK_SPACE:
			gamemode++;
			switch (gamemode){
			case 0: 
					
				Ginfo.velocity = 1; 
				break;
			case 1: 
				
				Ginfo.velocity = 1;
				break;
			default: gamemode = 0;
				init();
				break;
			}
			break;

		case VK_CONTROL:
			SndObjPlay(Sound[5], NULL);
			if (iq == 1)
				iq = 2;
			else
				iq = 1;
			break;
		}
		break;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
void init(){
	Ginfo.LEVEL = 1;
	Ginfo.flow = 0;
	Ginfo.velocity = 1;
	for (int i = 0; i < 10; i++){
		stone[i].posX = (20 * (rand() % 20)) + 20;
		stone[i].posY = (20 * (rand() % 38)) + 20;
		stone[i].velocity = 1;
		stone[i].droping = false;
		stone[i].shapeisset = false;
	}
	heroobj.posX = 400;
	heroobj.posY = 240;


	typestone[0] = { 0, 50, 127, 120 };
	typestone[1] = { 127, 50, 254, 120 };
	typestone[2] = { 254, 50, 381, 120 };
	typestone[3] = { 0, 148, 127, 268 };
	typestone[4] = { 127, 148, 254, 268 };
	typestone[5] = { 254, 148, 381, 268 };

	Ginfo.hp[0] = 135;
	Ginfo.hp[1] = 135;
	Ginfo.mp[0] = 18;
	Ginfo.mp[1] = 18;


	for (int i = 0; i < 9; i++)
		score[i + 1] = { 13 * i, 13, 13 * (i + 1), 26 };
	score[0] = { 117, 13, 130, 26 };
	score[10] = { 117, 13, 130, 26 };

	for (int i = 0; i < 7; i++)
		sceneChar[i] = { 5 + 50 * i, 80, 55 + 50 * i, 160 };
	for (int i = 7; i < 19; i++)
		sceneChar[i] = { 60 * (i - 7), 0, 50 + 60 * (i - 7), 80 };
	
}
void imageinit(){


	BackImage = DDLoadBitmap(DirectOBJ, "Map.bmp", 0, 0);
	DDSetColorKey(BackImage, RGB(1, 0, 0));

	SpriteImage = DDLoadBitmap(DirectOBJ, "stone.bmp", 0, 0);
	DDSetColorKey(SpriteImage, RGB(0, 0, 0));

	hpImage = DDLoadBitmap(DirectOBJ, "hpmini.bmp", 0, 0);
	DDSetColorKey(hpImage, RGB(0, 0, 0));

	Spritestone = DDLoadBitmap(DirectOBJ, "spritestone.bmp", 0, 0);
	DDSetColorKey(Spritestone, RGB(0, 0, 0));

	startIMG = DDLoadBitmap(DirectOBJ, "Startmenu.bmp", 0, 0);
	DDSetColorKey(startIMG, RGB(0, 0, 0));

	heroImage = DDLoadBitmap(DirectOBJ, "prisoner.bmp", 0, 0);
	DDSetColorKey(heroImage, RGB(0, 0, 255));

	overimage = DDLoadBitmap(DirectOBJ, "gameover.bmp", 0, 0);
	DDSetColorKey(overimage, RGB(0, 0, 0));

	MAPBACK = DDLoadBitmap(DirectOBJ, "mapback.bmp", 0, 0);
	DDSetColorKey(MAPBACK, RGB(0, 0, 0));


}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){

	MSG msg;
	init();
	heroobj.flag = true;
	heroobj2p.flag = false;
	if (!_GameMode(hInstance, nCmdShow, 480, 800, 32, 0)) return FALSE;

	imageinit();

	if (_InitDirectSound())
	{
		Sound[0] = SndObjCreate(SoundOBJ, "BGM.WAV", 1);
		Sound[1] = SndObjCreate(SoundOBJ, "death.WAV", 2);
		Sound[2] = SndObjCreate(SoundOBJ, "huh.WAV", 3);
		Sound[3] = SndObjCreate(SoundOBJ, "landing.WAV", 4);
		Sound[4] = SndObjCreate(SoundOBJ, "puk.WAV", 5);
		Sound[5] = SndObjCreate(SoundOBJ, "select.WAV", 6);
		Sound[6] = SndObjCreate(SoundOBJ, "wind.WAV", 6);
		Sound[7] = SndObjCreate(SoundOBJ, "boom.WAV", 6);
		//SndObjPlay( Sound[0], DSBPLAY_LOOPING );
	}

	SetTimer(MainHwnd, 1, 20, NULL);

	CommMain(NULL, NULL);

	// Main message loop
	while (GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	DestroyWindow(MainHwnd);

	return TRUE;
}

///////////////////// End of Game Program
