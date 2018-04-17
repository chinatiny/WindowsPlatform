//#define DLLEXPORT

#include "SessionManage.h"
#pragma comment( lib, "Wtsapi32.lib" )
#pragma warning( disable:4018 )
#pragma warning( disable:4996 )

//////////////////////////////////////////////////////////////////////////

CSessionManage::CSessionManage(LPTSTR lpszServerName /* = NULL */)
: _server_name(NULL)
, _server_handle(NULL)
{
	OpenServer(lpszServerName);
}

CSessionManage::~CSessionManage()
{
	CloseServer();
}

//////////////////////////////////////////////////////////////////////////

BOOL CSessionManage::OpenServer(LPTSTR lpszServerName)
{
	if (NULL == lpszServerName) {
		_server_handle = WTS_CURRENT_SERVER_HANDLE;
		return TRUE;
	}

	_server_name = new TCHAR[_tcslen(lpszServerName) + 1];
	if (NULL == _server_name)
		return FALSE;

	_tcscpy(_server_name, lpszServerName);

	_server_handle = WTSOpenServer(lpszServerName);
	if (NULL == _server_handle)
		return FALSE;

	return TRUE;
}

void CSessionManage::CloseServer()
{
	if (NULL != _server_name) {
		delete[]_server_name;
		_server_name = NULL;
	}
	if (WTS_CURRENT_SERVER_HANDLE != _server_handle)
		WTSCloseServer(_server_handle);
	_server_handle = NULL;
}

//////////////////////////////////////////////////////////////////////////

int CSessionManage::GetSessions(PWTS_SESSION_INFO pSessions, uint count)
{
	if (NULL == pSessions)
		return 0;

	PWTS_SESSION_INFO	pSessionInfo = NULL;
	DWORD				dwCount = 0;

	if (WTSEnumerateSessions(_server_handle, 0, 1, &pSessionInfo, &dwCount)) {
		dwCount = (dwCount <= count) ? dwCount : count;
		for (int i = 0; i < dwCount; i++)
			pSessions[i] = pSessionInfo[i];
	}

	WTSFreeMemory(pSessionInfo);
	return dwCount;
}

int CSessionManage::GetProcesses(PMY_WTS_PROCESS_INFO pProcesses, uint count)
{
	if (NULL == pProcesses)
		return 0;

	PWTS_PROCESS_INFO	pProcessInfo = NULL;
	DWORD				dwCount = 0;

	if (WTSEnumerateProcesses(_server_handle, 0, 1, &pProcessInfo, &dwCount)) {
		dwCount = (dwCount <= count) ? dwCount : count;
		for (int i = 0; i < dwCount; i++) {
			// session id
			pProcesses[i]._session_id = pProcessInfo[i].SessionId;

			// process id
			pProcesses[i]._process_id = pProcessInfo[i].ProcessId;

			// SID
			if (NULL != pProcessInfo[i].pUserSid)
				pProcesses[i]._sid = *(SID*)(pProcessInfo[i].pUserSid);
			else
				memset(&(pProcesses[i]._sid), 0, sizeof(SID));

			// process name
			if (NULL != pProcessInfo[i].pProcessName)
				_tcscpy(pProcesses[i]._process_name, pProcessInfo[i].pProcessName);
			else
				memset(pProcesses[i]._process_name, 0, sizeof(TCHAR)* NAMELEN);

			// domain name and user name
			DWORD			dwNameLen = NAMELEN;
			SID_NAME_USE	nameuse = SidTypeUser;
			LookupAccountSid(_server_name, pProcessInfo[i].pUserSid, pProcesses[i]._user_name,
				&dwNameLen, pProcesses[i]._domain_name, &dwNameLen, &nameuse);
		}
	}

	WTSFreeMemory(pProcessInfo);
	return dwCount;
}

int CSessionManage::GetProcesses(DWORD dwSessionId, PMY_WTS_PROCESS_INFO pProcesses, uint count)
{
	if (NULL == pProcesses)
		return 0;

	MY_WTS_PROCESS_INFO	pi[512] = { 0 };
	DWORD				dwCount = GetProcesses(pi, 512);
	int					rst = 0;

	for (int i = 0; i < dwCount; i++) {
		if (dwSessionId == pi[i]._session_id) {
			pProcesses[rst++] = pi[i];
			if (rst >= count)
				break;
		}
	}

	return rst;
}

BOOL CSessionManage::GetSessionUser(DWORD dwSessionId, LPTSTR lpszUserName)
{
	if (NULL == lpszUserName)
		return FALSE;

	LPTSTR	lpszName = NULL;
	DWORD	dwCount = 0;
	BOOL	bRet = FALSE;

	if (WTSQuerySessionInformation(_server_handle, dwSessionId, WTSUserName, &lpszName, &dwCount)) {
		_tcscpy(lpszUserName, lpszName);
		bRet = TRUE;
	}

	WTSFreeMemory(lpszName);
	return bRet;
}

BOOL CSessionManage::DisconnectSession(DWORD dwSessionId, BOOL bWait)
{
	return WTSDisconnectSession(_server_handle, dwSessionId, bWait);
}

BOOL CSessionManage::DisconnectSession(BOOL bWait)
{
	DWORD dwSessionId = 0;
	if (!ProcessIdToSessionId(GetCurrentProcessId(), &dwSessionId))
		return FALSE;
	return DisconnectSession(dwSessionId, bWait);
}

BOOL CSessionManage::LogoffSession(DWORD dwSessionId, BOOL bWait)
{
	return WTSLogoffSession(_server_handle, dwSessionId, bWait);
}

BOOL CSessionManage::LogoffSession(BOOL bWait)
{
	DWORD dwSessionId = 0;
	if (!ProcessIdToSessionId(GetCurrentProcessId(), &dwSessionId))
		return FALSE;
	return LogoffSession(dwSessionId, bWait);
}

BOOL CSessionManage::LogoffUser(LPCTSTR lpszUserName, BOOL bWait)
{
	if (NULL == lpszUserName)
		return FALSE;

	PWTS_SESSION_INFO	pSessionInfo = NULL;
	DWORD				dwCount = NULL;
	BOOL				bReturn = FALSE;

	if (WTSEnumerateSessions(_server_handle, 0, 1, &pSessionInfo, &dwCount)) {
		bReturn = TRUE;
		for (int i = 0; i < dwCount; i++) {
			LPTSTR	lpszName = NULL;
			DWORD	dwRet = 0;
			if (WTSQuerySessionInformation(_server_handle, pSessionInfo[i].SessionId, WTSUserName,
				&lpszName, &dwRet)) {
				if (0 == _tcscmp(lpszUserName, lpszName))
					WTSLogoffSession(_server_handle, pSessionInfo[i].SessionId, bWait);
			}
			WTSFreeMemory(lpszName);
		}
	}

	WTSFreeMemory(pSessionInfo);
	return bReturn;
}
