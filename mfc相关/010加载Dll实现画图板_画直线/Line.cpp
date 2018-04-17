#include "stdafx.h"
#include "Line.h"



CLine::CLine()
{
	memset(&m_beginPos, 0, sizeof(m_beginPos));
	memset(&m_endPos, 0, sizeof(m_endPos));
}


CLine::~CLine()
{
}


void CLine::LButtonDown(int nX, int nY)
{
	m_beginPos.x = nX;
	m_beginPos.y = nY;
}

void CLine::LButtonUp(int nX, int nY)
{
	m_endPos.x = nX;
	m_endPos.y = nY;
	ShapPos shapPos = { m_beginPos, m_endPos };
	m_vecLinePoints.push_back(shapPos);
	//
	memset(&m_beginPos, 0, sizeof(m_beginPos));
	memset(&m_endPos, 0, sizeof(m_endPos));
}

void CLine::MouseMove(CWnd *pWnd, CDC *dc, int nX, int nY)
{
	if (m_beginPos.x != 0 && m_beginPos.y != 0)
	{
		//1.擦掉旧的
		CBrush brush = CBrush();
 		brush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		CRect rect;
		pWnd->GetClientRect(&rect);
		dc->FillRect(&rect, &brush);
		//2. 画出旧的
		DrawOld(dc);
		//3.画出新的
		m_endPos.x = nX;
		m_endPos.y = nY;
		JustDrawByPos(dc, m_beginPos, m_endPos);
	}

}

CLine g_line;


void CLine::JustDrawByPos(CDC* dc, POINT begPios, POINT endPos)
{
	dc->MoveTo(begPios);
	dc->LineTo(endPos);
}

void CLine::DrawOld(CDC *dc)
{
	for (size_t i = 0; i < m_vecLinePoints.size(); i++)
	{
		JustDrawByPos(dc, m_vecLinePoints[i].beginPos, m_vecLinePoints[i].endPos);
	}
}

const TCHAR* GetName() 
{
	return _T("画直线");
}

CShape* GetInstance() 
{
	return &g_line;
}
