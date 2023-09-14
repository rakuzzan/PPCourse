#include <windows.h>
#include <string>
#include <iostream>

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	std::cout << lpParam << std::endl;
	ExitThread(0);
}

int main()
{
	// создание двух потоков
	HANDLE* handles = new HANDLE[2];

	handles[0] = CreateThread(NULL, 0, &ThreadProc, NULL, CREATE_SUSPENDED,
		NULL);
	handles[1] = CreateThread(NULL, 0, &ThreadProc, NULL, CREATE_SUSPENDED,
		NULL);



	ResumeThread(handles[0]);
	ResumeThread(handles[1]);

	WaitForMultipleObjects(2, handles, true, INFINITE);
	return 0;
}