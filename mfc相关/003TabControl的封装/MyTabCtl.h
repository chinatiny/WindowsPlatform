#pragma once
#include <vector>


// MyTabCtl

class MyTabCtl : public CTabCtrl
{
	DECLARE_DYNAMIC(MyTabCtl)

public:
	MyTabCtl();
	virtual ~MyTabCtl();


	void AddTab(const CString& tabName, CWnd* pWnd);
	void SetCurSel(int nIndex);
private:
	std::vector<CWnd*> m_vecWnd;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult);
};


