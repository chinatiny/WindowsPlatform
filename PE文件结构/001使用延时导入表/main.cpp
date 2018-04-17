#include <windows.h>
#include "DllInterface.h"

#include <delayimp.h>
#pragma comment(lib, "delayimp.lib")


int main(int argc, char* argv[])
{
	TestFunc();
	system("pause");
	return 0;
}