#pragma once
#include "DllInterface.h"
#include <vector>


typedef struct  
{
	POINT beginPos;
	POINT endPos;
}ShapPos;

class CLine :
	public CShape
{
public:
	CLine();
	~CLine();
public:
	void LButtonDown(int nX, int nY);
	void LButtonUp(int nX, int nY);
	void MouseMove(CWnd *pWnd, CDC *dc, int nX, int nY);
	void JustDrawByPos(CDC* dc, POINT begPios, POINT endPos);
	void DrawOld(CDC *dc);

private:
	std::vector<ShapPos> m_vecLinePoints;
	POINT m_beginPos;
	POINT m_endPos;
};

extern CLine g_line;
