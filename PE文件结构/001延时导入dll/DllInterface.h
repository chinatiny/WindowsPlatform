#pragma  once


#ifdef EXPORT_PROJECT
#define  DLL_API extern "C" __declspec(dllexport) 
#else
#define  DLL_API extern "C" __declspec(dllexport) 
#endif


DLL_API void TestFunc();