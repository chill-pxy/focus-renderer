#include<iostream>
#include<thread>
#include<Windows.h>
using namespace std;

typedef void (WINAPI* MY_FUNC)(void);
MY_FUNC func;

typedef int (WINAPI* MY_FUNC2)(void);
MY_FUNC2 func2;

int main()
{
	HMODULE g_hDlls = LoadLibrary(TEXT("E:\\project\\FOCUS\\build\\x64-Debug\\bin\\drhi.dll"));
	HMODULE g_hDll = LoadLibrary(TEXT("E:\\project\\FOCUS\\build\\x64-Debug\\bin\\engine.dll"));
	if (g_hDll)
	{
		func = (MY_FUNC)GetProcAddress(g_hDll, "run");
		func2 = (MY_FUNC2)GetProcAddress(g_hDll, "getSceneObjCount");
	}

	thread t([] {
		func();
		});
	t.detach();

	while (true)
	{
		std::cout << func2();
	}

	return 0;
}