#include <windows.h>
#include <stdio.h>
#include <shlobj.h>

int main(int argc, char* argv[])
{
	HANDLE hToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		printf("错误码:%x\n", GetLastError());
		return 0;
	}
	//用于接收令牌类型
	TOKEN_ELEVATION_TYPE tokenType = TokenElevationTypeDefault;

	DWORD dwRetSize = 0; // 用于接收函数输出信息的字节数

	// 获取令牌信息
	GetTokenInformation(hToken,
		TokenElevationType,// 获取令牌的当前提升等级
		&tokenType,
		sizeof(tokenType),
		&dwRetSize // 所需缓冲区的字节数
		);

	// 根据令牌的类型来输出相应的信息
	switch (tokenType)
	{

	case TokenElevationTypeDefault:
		printf("默认用户, 可能是一个普通用户, 可能是关闭UAC时登录的管理员用户\n");

		// 调用系统函数IsUserAnAdmin, 进一步确定是普通用户还是管理员用户
		return IsUserAnAdmin();
		break;

	case TokenElevationTypeFull:
		printf("管理员账户,并拥有全部的权限\n");
		break;

	case TokenElevationTypeLimited:
	{
									  // 判断受限制的用户是管理员
									  // 如果是管理员, 则这个令牌中会保存有管理员的SID


									  // 1. 获取系统内键管理员用户的SID
									  byte adminSID[SECURITY_MAX_SID_SIZE];
									  DWORD dwSize = SECURITY_MAX_SID_SIZE;
									  CreateWellKnownSid(WinBuiltinAdministratorsSid, // 获取SID的类型,这里是系统内键管理员
										  NULL, // 传NULL,获取本地计算机的管理员
										  adminSID,// 函数输出的管理员SID
										  &dwSize // 输入结构的大小,也作为输出
										  );

									  // 获取本令牌的连接令牌(受限制的令牌都会有一个连接的令牌,受限制的令牌正是由主令牌所创建的. )
									  TOKEN_LINKED_TOKEN linkToken;
									  GetTokenInformation(hToken,
										  TokenLinkedToken,
										  &linkToken,
										  sizeof(linkToken),
										  &dwSize
										  );

									  // 在连接的令牌中查找是否具有管理员的SID
									  BOOL    bIsMember = FALSE; // 用于保存是否包含.
									  CheckTokenMembership(linkToken.LinkedToken,
										  &adminSID,
										  &bIsMember);


									  if (bIsMember) {
										  printf("权限被阉割的受限制管理员账户, 部分权限被移处理\n");
									  }
	}
		break;
	}

	system("pause");


	return 0;
}