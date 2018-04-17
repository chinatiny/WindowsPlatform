// MyButtonWM_PAINT.cpp : 实现文件
//

#include "stdafx.h"
#include "009控件自绘.h"
#include "MyButtonWM_PAINT.h"


// MyButtonWM_PAINT

IMPLEMENT_DYNAMIC(MyButtonWM_PAINT, CButton)

MyButtonWM_PAINT::MyButtonWM_PAINT()
{
	m_isLeftButton = FALSE;
	m_isHover = FALSE;
}

MyButtonWM_PAINT::~MyButtonWM_PAINT()
{
}


BEGIN_MESSAGE_MAP(MyButtonWM_PAINT, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// MyButtonWM_PAINT 消息处理程序




void MyButtonWM_PAINT::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_isLeftButton = TRUE;
	InvalidateRect(NULL, TRUE);
	CButton::OnLButtonDown(nFlags, point);
}


void MyButtonWM_PAINT::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_isLeftButton = FALSE;
	InvalidateRect(NULL, TRUE);

	CButton::OnLButtonUp(nFlags, point);
}


void MyButtonWM_PAINT::OnMouseMove(UINT nFlags, CPoint point)
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
void MyButtonWM_PAINT::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_isHover = TRUE;
	CButton::OnMouseHover(nFlags, point);
	InvalidateRect(NULL, 0);
	CButton::OnMouseHover(nFlags, point);
}


void MyButtonWM_PAINT::OnMouseLeave()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_isHover = FALSE;
	InvalidateRect(0, 0);

	CButton::OnMouseLeave();
}


void MyButtonWM_PAINT::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
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

	// 不为绘图消息调用 CButton::OnPaint()
}
