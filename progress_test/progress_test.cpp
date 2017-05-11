#include<stdio.h>
#include<windows.h>
#include <iostream>
using namespace std;


volatile int b = 0;
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	int i = 10000;
	int *p = (int*)lpParameter;
	while(i--)
	{
		(*p)++;
		b++;
	}
	return 0;
}

int main(int argc, char* argv[])
{
	int a = 0;
	HANDLE hThread1 = CreateThread(NULL,0,ThreadProc, &a, 0, NULL);
	HANDLE hThread2 = CreateThread(NULL,0,ThreadProc, &a, 0, NULL);
	HANDLE hThread3 = CreateThread(NULL,0,ThreadProc, &a, 0, NULL);
	HANDLE hThread4 = CreateThread(NULL,0,ThreadProc, &a, 0, NULL);
	HANDLE hThread5 = CreateThread(NULL,0,ThreadProc, &a, 0, NULL);
	Sleep(1000);
	CloseHandle(hThread1);
	CloseHandle(hThread2);
	CloseHandle(hThread3);
	CloseHandle(hThread4);
	CloseHandle(hThread5);

	cout<<"a = "<<a<<endl;
	cout<<"b = "<<b<<endl;
	system("pause");
	return 0;
}