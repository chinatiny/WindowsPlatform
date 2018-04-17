#ifndef __SessionManage_h__
#define __SessionManage_h__



#include <Windows.h>
#include <tchar.h>
#include <WtsApi32.h>

#define NAMELEN			64

typedef unsigned int	uint;

typedef struct _my_wts_process_info {
	DWORD		_session_id;
	DWORD		_process_id;
	TCHAR		_process_name[NAMELEN];
	TCHAR		_domain_name[NAMELEN];
	TCHAR		_user_name[NAMELEN];
	SID			_sid;
} MY_WTS_PROCESS_INFO, *PMY_WTS_PROCESS_INFO;

class  CSessionManage
{
public:
	CSessionManage(LPTSTR lpszServerName = NULL);
	virtual ~CSessionManage();

	int		GetSessions(PWTS_SESSION_INFO pSessions, uint count);
	int		GetProcesses(DWORD dwSessionId, PMY_WTS_PROCESS_INFO pProcesses, uint count);
	int		GetProcesses(PMY_WTS_PROCESS_INFO pProcesses, uint count);
	BOOL	GetSessionUser(DWORD dwSessionId, LPTSTR lpszUserName);

	BOOL	DisconnectSession(DWORD dwSessionId, BOOL bWait);
	BOOL	DisconnectSession(BOOL bWait);
	BOOL	LogoffSession(DWORD dwSessionId, BOOL bWait);
	BOOL	LogoffSession(BOOL bWait);
	BOOL	LogoffUser(LPCTSTR lpszUserName, BOOL bWait);

protected:
	CSessionManage(const CSessionManage&);
	CSessionManage& operator = (const CSessionManage&);

	BOOL	OpenServer(LPTSTR lpszServerName);
	void	CloseServer();

private:
	LPTSTR		_server_name;
	HANDLE		_server_handle;
};

#endif // __SessionManage_h__
