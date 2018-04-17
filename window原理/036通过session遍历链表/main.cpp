#include <stdio.h>
#include "SessionManage.h"

int main(void)
{
	CSessionManage sm;

	WTS_SESSION_INFO si[16] = { 0 };
	int session_count = sm.GetSessions(si, 16);
	for (int i = 0; i < session_count; i++)
		_tprintf(_T("session%02d: %d\n"), i + 1, si[i].SessionId);

	printf("\r\n");

	MY_WTS_PROCESS_INFO pi[128] = { 0 };
	int process_count = sm.GetProcesses(pi, 128);
	for (int i = 0; i < process_count; i++)
		_tprintf(_T("process%02d: %04d %s\n"), i + 1, pi[i]._process_id, pi[i]._process_name);

	return 0;
}
