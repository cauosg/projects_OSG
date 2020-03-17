#include <Windows.h>
#include <stdio.h>
#include <conio.h>

void main()
{//yyy,xxx,zzz,r
	char test[] = "09009090";

	HANDLE hDevice = CreateFile(L"COM5", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);

	if (hDevice != INVALID_HANDLE_VALUE)
	{
		printf("Port opened! \n");
		DCB lpTest;
		GetCommState(hDevice, &lpTest);
		lpTest.BaudRate = CBR_1200;
		lpTest.ByteSize = 8;
		lpTest.Parity = NOPARITY;
		lpTest.StopBits = ONESTOPBIT;
		SetCommState(hDevice, &lpTest);

		DWORD btsIO;

		WriteFile(hDevice, test, strlen(test), &btsIO, NULL);

		CloseHandle(hDevice);
	}

	_getch();
	return ;
}