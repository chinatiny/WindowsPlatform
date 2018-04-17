// MyDrawItemButton.cpp : 实现文件
//

#include "stdafx.h"
#include "009控件自绘.h"
#include "MyDrawItemButton.h"


// CMyDrawItemButton

IMPLEMENT_DYNAMIC(CMyDrawItemButton, CButton)

CMyDrawItemButton::CMyDrawItemButton()
{
	m_isLeftButton = FALSE;
	m_isHover = FALSE;
}

CMyDrawItemButton::~CMyDrawItemButton()
{
}


BEGIN_MESSAGE_MAP(CMyDrawItemButton, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_DRAWITEM()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()



// CMyDrawItemButton 消息处理程序





void CMyDrawItemButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_isLeftButton = TRUE;
	InvalidateRect(NULL, TRUE);
	CButton::OnLButtonDown(nFlags, point);
}


void CMyDrawItemButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_isLeftButton = FALSE;
	InvalidateRect(NULL, TRUE);

	CButton::OnLButtonUp(nFlags, point);
}



void CMyDrawItemButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	TRACKMOUSEEVENT tme = { 0 };
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_HOVER | TME_LEAVE;
	tme.dwHoverTime = 100;
	tme.hwndTrack = m_hWnd;
	TrackMouseEvent(&tme);   //只有这样才可以发送悬停和离开消息

	CButton::OnMouseMove(nFlags, point);
}


//鼠标悬停
void CMyDrawItemButton::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_isHover = TRUE;
	CButton::OnMouseHover(nFlags, point);
	InvalidateRect(NULL, 0);
	CButton::OnMouseHover(nFlags, point);
}


void CMyDrawItemButton::OnMouseLeave()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_isHover = FALSE;
	InvalidateRect(0, 0);

	CButton::OnMouseLeave();
}




void CMyDrawItemButton::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/) {

	CClientDC dc(this);
	// 判断有没有悬停
	if (m_isHover == TRUE) {

		// 判断有没有按下左键
		if (m_isLeftButton == TRUE) {
			// 绘制鼠标按下时按钮的样式
			dc.TextOutW(0, 0, L"鼠标按下");
		}
		else {
			// 鼠标没按下,但悬停了, 绘制悬停的样式
			dc.TextOutW(0, 0, L"鼠标悬停");
		}
	}
	else {
		// 鼠标没按下, 也没悬停,绘制鼠标离开的样式
		dc.TextOutW(0, 0, L"鼠标离开");
	}
}



