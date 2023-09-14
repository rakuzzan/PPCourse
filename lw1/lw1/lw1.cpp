#include <windows.h>
#include <string>
#include <iostream>

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	std::cout << std::to_string(*static_cast<int*>(lpParam)) << std::endl;
	ExitThread(0);
}

int main()
{
	int n;
	std::cin >> n;

	HANDLE* handles = new HANDLE[n];

	for (int i = 0; i < n; i++)
	{
		handles[i] = CreateThread(nullptr, 0, &ThreadProc, LPVOID(&i), CREATE_SUSPENDED, nullptr);
	}

	for (int i = 0; i < n; i++)
	{
		ResumeThread(handles[i]);
	}

	WaitForMultipleObjects(n, handles, true, INFINITE);

	return 0;
}