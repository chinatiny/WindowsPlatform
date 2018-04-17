#pragma once
#include <afxwin.h>

// dll 必须要导出的绘图接口
class CShape {
public:
	virtual void LButtonDown(int nX, int nY) = 0;
	virtual void LButtonUp(int nX, int nY) = 0;
	virtual 	void MouseMove(CWnd *pWnd, CDC *dc, int nX, int nY) = 0;
	virtual void JustDrawByPos(CDC* dc, POINT begPios, POINT endPos) = 0;
	virtual void DrawOld(CDC *dc) = 0;
	virtual ~CShape() {}
};

extern "C" _declspec(dllexport) const TCHAR* GetName();
extern "C" _declspec(dllexport) CShape* GetInstance();