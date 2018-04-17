// MyTabCtl.cpp : 实现文件
//

#include "stdafx.h"
#include "003TabControl的封装.h"
#include "MyTabCtl.h"


// MyTabCtl

IMPLEMENT_DYNAMIC(MyTabCtl, CTabCtrl)

MyTabCtl::MyTabCtl()
{

}

MyTabCtl::~MyTabCtl()
{
}

void MyTabCtl::AddTab(const CString& tabName, CWnd* pWnd) {
	pWnd->SetParent(this);
	CTabCtrl::InsertItem(m_vecWnd.size(), tabName);
	m_vecWnd.push_back(pWnd);
}

void MyTabCtl::SetCurSel(int nIndex) {
	for (size_t i = 0; i < m_vecWnd.size(); ++i)
	{
		m_vecWnd[i]->ShowWindow(SW_HIDE);
	}
	m_vecWnd[nIndex]->ShowWindow(SW_SHOW);
	CRect rt;
	// 获取选项卡的大小
	GetClientRect(rt);
	rt.top += 20;
	rt.left += 1;
	rt.right -= 1;
	rt.bottom -= 1;
	// 将子窗口的大小设置为和选项卡控件一样大.
	m_vecWnd[nIndex]->MoveWindow(rt);
}


BEGIN_MESSAGE_MAP(MyTabCtl, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, &MyTabCtl::OnTcnSelchange)
END_MESSAGE_MAP()



// MyTabCtl 消息处理程序




void MyTabCtl::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
	
	int nIndex = CTabCtrl::GetCurSel();
	SetCurSel(nIndex);
}
