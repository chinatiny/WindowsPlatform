#pragma once


// MyButtonWM_PAINT

class MyButtonWM_PAINT : public CButton
{
	DECLARE_DYNAMIC(MyButtonWM_PAINT)

public:
	MyButtonWM_PAINT();
	virtual ~MyButtonWM_PAINT();


private:
	BOOL m_isLeftButton;
	BOOL m_isHover;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnPaint();
};


