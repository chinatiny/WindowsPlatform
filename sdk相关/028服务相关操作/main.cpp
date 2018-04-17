#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <map>
#include <string>


///////////////////////////////内部函数/////////////////////////////////////
//服务类型
static std::map<DWORD, TCHAR*> s_mapSrvType = {
	{ SERVICE_FILE_SYSTEM_DRIVER, _T("SERVICE_FILE_SYSTEM_DRIVER") },
	{ SERVICE_KERNEL_DRIVER, _T("SERVICE_KERNEL_DRIVER") },
	{ SERVICE_WIN32_OWN_PROCESS, _T("SERVICE_WIN32_OWN_PROCESS") },
	{ SERVICE_WIN32_SHARE_PROCESS, _T("SERVICE_WIN32_SHARE_PROCESS") },
};

//服务状态
static std::map<DWORD, TCHAR*> s_mapSrvState = {
	{ SERVICE_CONTINUE_PENDING, _T("SERVICE_CONTINUE_PENDING") },
	{ SERVICE_PAUSE_PENDING, _T("SERVICE_PAUSE_PENDING") },
	{ SERVICE_PAUSED, _T("SERVICE_PAUSED") },
	{ SERVICE_RUNNING, _T("SERVICE_RUNNING") },
	{ SERVICE_START_PENDING, _T("SERVICE_START_PENDING") },
	{ SERVICE_STOP_PENDING, _T("SERVICE_STOP_PENDING") },
	{ SERVICE_STOPPED, _T("SERVICE_STOPPED") },
};

//服务启动类型
static std::map<DWORD, TCHAR*> s_mapSrvStartType = {
	{ SERVICE_AUTO_START, _T("SERVICE_AUTO_START") },
	{ SERVICE_BOOT_START, _T("SERVICE_BOOT_START") },
	{ SERVICE_DEMAND_START, _T("SERVICE_DEMAND_START") },
	{ SERVICE_DISABLED, _T("SERVICE_DISABLED") },
	{ SERVICE_SYSTEM_START, _T("SERVICE_SYSTEM_START") },
};



//遍历服务
static void EnumSrv()
{
	//1. 打开远程计算机服务控制管理
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCM) return;

	//2. 第一次调用需要获取内存大小
	DWORD dwServiceNum = 0;
	DWORD dwSize = 0;
	EnumServicesStatusEx(
		hSCM,
		SC_ENUM_PROCESS_INFO,
		SERVICE_WIN32,
		SERVICE_STATE_ALL,
		NULL,
		0,
		&dwSize,
		&dwServiceNum,
		NULL,
		NULL
		);

	//3. 申请需要的内存
	LPENUM_SERVICE_STATUS_PROCESS pEnumService = (LPENUM_SERVICE_STATUS_PROCESS)new char[dwSize];

	//4. 第二次枚举
	BOOL bStatus = FALSE;
	bStatus = EnumServicesStatusEx(
		hSCM,
		SC_ENUM_PROCESS_INFO,
		SERVICE_WIN32,
		SERVICE_STATE_ALL,
		(PBYTE)pEnumService,
		dwSize,
		&dwSize,
		&dwServiceNum,
		NULL,
		NULL
		);

	//5. 遍历信息
	for (DWORD i = 0; i < dwServiceNum; i++)
	{
		_tprintf(_T("服务名字:%s\n\t服务状态:%s\n\t服务类型:%s\n"),
			pEnumService[i].lpServiceName,
			s_mapSrvState[pEnumService[i].ServiceStatusProcess.dwCurrentState],
			s_mapSrvType[pEnumService[i].ServiceStatusProcess.dwServiceType]
			);

		//打开一个服务
		SC_HANDLE hService = OpenService(
			hSCM,
			pEnumService[i].lpServiceName,
			SERVICE_QUERY_CONFIG
			);
		if (NULL == hService) continue;

		//第一次调用获取需要的缓冲区大小
		QueryServiceConfig(hService, NULL, 0, &dwSize);

		//分配内存
		LPQUERY_SERVICE_CONFIG pServiceConfig = (LPQUERY_SERVICE_CONFIG) new char[dwSize];

		//第二次调用，获取信息
		QueryServiceConfig(hService, pServiceConfig, dwSize, &dwSize);
		_tprintf(_T("\t启动类型:%s\n"), s_mapSrvStartType[pServiceConfig->dwStartType]);
		_tprintf(_T("\t路径:%s\n"), pServiceConfig->lpBinaryPathName);

		delete[] pServiceConfig;
		CloseServiceHandle(hService);
	}

	//6. 清理内存
	CloseServiceHandle(hSCM);
	delete[] pEnumService;
}

//打开服务
static void OpenSrv()
{
	char pszServiceName[MAX_PATH] = { 0 };
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hService = NULL;
	DWORD dwSize = 0;
	SERVICE_STATUS serviceStatus = { 0 };

	printf("请输入打开服务名字：");
	scanf_s("%s", pszServiceName, _countof(pszServiceName));
	fflush(stdin);

	//
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCM)
		goto __EXIT;
	hService = OpenServiceA(
		hSCM,
		pszServiceName,
		SERVICE_ALL_ACCESS
		);
	if (NULL == hService)
	{
		printf("无法打开服务:%s\n", pszServiceName);
		goto __EXIT;
	}

	//获取服务当前的状态
	QueryServiceStatus(hService, &serviceStatus);
	if (ERROR_ACCESS_DENIED == GetLastError())
	{
		printf("权限不够\n");
		goto __EXIT;
	}

	if (SERVICE_STOPPED != serviceStatus.dwCurrentState)
	{
		printf("服务不是关闭状态,无法开启!~\n");
		goto __EXIT;
	}
	//开启服务
	StartService(hService, NULL, NULL);
__EXIT:
	if (NULL != hService) CloseServiceHandle(hService);
	if (NULL != hSCM) 	CloseServiceHandle(hSCM);
}

//关闭服务
static void CloseSrv()
{

	char pszServiceName[MAX_PATH] = { 0 };
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hService = NULL;
	DWORD dwSize = 0;
	SERVICE_STATUS serviceStatus = { 0 };

	printf("请输入关闭服务名字：");
	scanf_s("%s", pszServiceName, _countof(pszServiceName));
	fflush(stdin);

	//
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCM)
		goto __EXIT;
	hService = OpenServiceA(
		hSCM,
		pszServiceName,
		SERVICE_ALL_ACCESS
		);
	if (NULL == hService)
	{
		printf("无法打开服务:%s\n", pszServiceName);
		goto __EXIT;
	}

	//获取服务当前的状态
	QueryServiceStatus(hService, &serviceStatus);
	if (ERROR_ACCESS_DENIED == GetLastError())
	{
		printf("权限不够\n");
		goto __EXIT;
	}

	if (SERVICE_RUNNING != serviceStatus.dwCurrentState)
	{
		printf("服务不是开启状态,无法关闭!~\n");
		goto __EXIT;
	}
	//关闭服务
	ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus);
__EXIT:
	if (NULL != hService) CloseServiceHandle(hService);
	if (NULL != hSCM) 	CloseServiceHandle(hSCM);

}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "chs"); //支持中文本地化
	while (true)
	{
		printf("-----------------------------------------------------\n");
		printf("\t1. 遍历服务\n");
		printf("\t2. 打开服务\n");
		printf("\t3. 关闭服务\n");
		printf("\t4. 退出\n");
		printf("-----------------------------------------------------\n");

		int nChoice;
		printf("请输入你的选择(1~4):");
		scanf_s("%d", &nChoice);
		fflush(stdin);
		if (!(nChoice >= 1 && nChoice <= 4))
		{
			printf("输入错误请重新输入！");
			system("cls");
			continue;
		}
		switch (nChoice)
		{
		case 1:
			EnumSrv();
			break;
		case 2:
			OpenSrv();
			break;
		case 3:
			CloseSrv();
			break;
		}
		system("pause");
		system("cls");
	}
	return 1;

}
