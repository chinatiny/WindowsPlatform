#include <windows.h>
#include <stdio.h>
#include <shlobj.h>
#include <tchar.h>

// 给进程添加对应的权限
// HANDLE hProcess : 要添加权限的进程
// const TCHAR* pszPrivilegeName : 要添加的特权的名称
BOOL    AddPrivilege(HANDLE hProcess, const TCHAR* pszPrivilegeName);

// 判断是否是管理员权限
BOOL IsAdmin(HANDLE hProcess);


int main(int argc, char* argv[])
{

	HANDLE hProcess = NULL;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 7200);

	if (hProcess == NULL) {
		printf("错误码:%x\n", GetLastError());
		return 1;
	}

	// 判断当前进程是否以管理员身份运行
	if (!IsAdmin(GetCurrentProcess())) {

		// 以管理员身份运行本进程
		//  1 获取本进程的文件路径.
		TCHAR path[MAX_PATH] = { 0 }; // 需要初始化
		DWORD dwPathSize = MAX_PATH;
		QueryFullProcessImageName(GetCurrentProcess(), 0, path, &dwPathSize);

		// 2 调用创建进程的API运行本进程.
		ShellExecute(NULL,            // 窗口句柄,没有则填NULL
			_T("runas"),   // 以管理员身份运行的重要参数
			path,            // 所有运行的程序的路径(这里是本进程)
			NULL,            // 命令行参数
			NULL,            // 新进程的工作目录的路径
			SW_SHOW           // 创建后的显示标志(最小化,最大化, 显示,隐藏等)
			);
		system("pause");
		// 退出本进程
		ExitProcess(0);
	}

	// 将权限提升到SeDbug权限
	if (!AddPrivilege(GetCurrentProcess(), SE_DEBUG_NAME/*字符串形式的权限名*/)) {
		printf("提升权限失败\n");
		system("pause");
		return 0;
	}

	// 打开进程句柄
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 7200);
	if (hProcess == NULL) {
		printf("错误码:%x\n", GetLastError());
		system("pause");
		return 0;
	}
	printf( "打开成功\n" );


    system( "pause" );

	CloseHandle(hProcess);
	return 0;
}

BOOL AddPrivilege(HANDLE hProcess, const TCHAR* pszPrivilegeName)
{
	// 进程的特权使用LUID值来表示, 因此, 需要先获取传入的权限名对应的LUID值


	// 0. 获取特权对应的LUID值
	LUID privilegeLuid;
	if (!LookupPrivilegeValue(NULL, pszPrivilegeName, &privilegeLuid))
		return FALSE;


	// 1. 获取本进程令牌
	HANDLE hToken = NULL;
	// 打开令牌时, 需要加上TOKEN_ADJUST_PRIVILEGES 权限(这个权限用于修改令牌特权)
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		printf("错误码:%x\n", GetLastError());
		return 0;
	}

	// 2. 使用令牌特权修改函数将SeDebug的LUID特权值添加到本进程令牌
	TOKEN_PRIVILEGES tokenPrivieges; // 新的特权

	// 使用特权的LUID来初始化结构体.
	tokenPrivieges.PrivilegeCount = 1; // 特权个数
	tokenPrivieges.Privileges[0].Luid = privilegeLuid; // 将特权LUID保存到数组中
	tokenPrivieges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;// 将属性值设为启用(有禁用,移除等其他状态)



	// 调用函数添加特权
	return AdjustTokenPrivileges(hToken,              // 要添加特权的令牌
		FALSE,               // TRUE是移除特权, FALSE是添加特权
		&tokenPrivieges,     // 要添加的特权数组
		sizeof(tokenPrivieges),// 整个特权数组的大小
		NULL,                // 旧的特权数组
		NULL                  // 旧的特权数组的长度
		);
}



BOOL IsAdmin(HANDLE hProcess)
{
	HANDLE hToken = NULL;
	OpenProcessToken(hProcess, TOKEN_QUERY, &hToken);

	TOKEN_ELEVATION_TYPE tokenType = TokenElevationTypeDefault; // 用于接收令牌类型

	DWORD dwRetSize = 0; // 用于接收函数输出信息的字节数

	// 2. 查询进程令牌中的权限提升值.( 这个值会记录当前的令牌是何种类型( 细节在17_权限管理_令牌的获取.cpp ) )
	GetTokenInformation(hToken,
		TokenElevationType,// 获取令牌的当前提升等级
		&tokenType,
		sizeof(tokenType),
		&dwRetSize // 所需缓冲区的字节数
		);


	// 根据令牌的类型来输出相应的信息
	if (TokenElevationTypeFull == tokenType) {
		// 3. 如果令牌是TokenElevationTypeFull , 则拥有至高无上的能力,可以给令牌添加任何特权
		printf("管理员账户,并拥有全部的权限,可以给令牌添加任何特权\n");
		return TRUE;
	}
	// 4. 如果是其他的, 则需要以管理员身份重新运行本进程. 这样就能以第三步的方法解决剩下的问题.
	else if (TokenElevationTypeDefault == tokenType) {
		printf("默认用户, 可能是一个普通用户, 可能是关闭UAC时登录的管理员用户\n");

		// 调用系统函数IsUserAnAdmin, 进一步确定是普通用户还是管理员用户
		return IsUserAnAdmin();
	}
	else if (TokenElevationTypeLimited == tokenType) {

		// 判断受限制的用户是管理员
		// 如果是管理员, 则这个令牌中会保存有管理员的SID

		// 1. 获取系统内键管理员用户的SID
		byte adminSid[SECURITY_MAX_SID_SIZE];
		DWORD dwSize = SECURITY_MAX_SID_SIZE;
		CreateWellKnownSid(WinBuiltinAdministratorsSid, // 获取SID的类型,这里是系统内键管理员
			NULL, // 传NULL,获取本地计算机的管理员
			&adminSid,// 函数输出的管理员SID
			&dwSize // 输入结构的大小,也作为输出
			);

		// 获取本令牌的连接令牌(受限制的令牌都会有一个连接的令牌,受限制的令牌正式由主令牌所创建的. )
		TOKEN_LINKED_TOKEN linkToken;
		GetTokenInformation(hToken,
			TokenLinkedToken, // 获取连接的令牌句柄
			&linkToken,
			sizeof(linkToken),
			&dwSize
			);

		// 在连接的令牌中查找是否具有管理员的SID
		BOOL    bIsContain = FALSE; // 用于保存是否包含.
		CheckTokenMembership(linkToken.LinkedToken, // 在这个令牌中检查
			&adminSid,             // 检查令牌中是否包含此SID
			&bIsContain);           // 输出TRUE则包含,反之不包含



		if (bIsContain) {
			printf("权限被阉割的受限制管理员账户, 部分权限被移处理\n");
		}


		return FALSE; // 不是以管理员权限运行
	}

	return FALSE;
}