#include <windows.h>
#include <string>
#include <iostream>
#pragma comment(lib, "winmm.lib")

int start;

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	int myNumber = (int)(INT_PTR)lpParam;

	for (int j = 1; j < 23; j++)
	{

		for (int i = 0; i < 1000000000; i++)
		{
		}

		int end = timeGetTime();
		double millisec = (end - start) / ((double)CLOCKS_PER_SEC / 1000);
		printf("Thread number %d | %f\n", myNumber + 1, millisec);

	}

	ExitThread(0);
}
int main(int argc, char* argv[])
{
	int temp;
	std::cin >> temp;
	start = timeGetTime();
	int threadNumber = std::stoi(argv[1]);

	HANDLE* handles = new HANDLE[threadNumber];
	for (int i = 0; i < threadNumber; i++) {
		handles[i] = CreateThread(NULL, 0, &ThreadProc, (LPVOID)(INT_PTR)i, CREATE_SUSPENDED,
			NULL);
	}

	// запуск потоков
	for (int i = 0; i < threadNumber; i++) {
		ResumeThread(handles[i]);
	}

	// ожидание окончания работы потоков
	WaitForMultipleObjects(threadNumber, handles, true, INFINITE);
	return 0;
}