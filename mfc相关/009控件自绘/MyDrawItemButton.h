#pragma once


// CMyDrawItemButton

class CMyDrawItemButton : public CButton
{
	DECLARE_DYNAMIC(CMyDrawItemButton)

public:
	CMyDrawItemButton();
	virtual ~CMyDrawItemButton();

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
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnMouseLeave();
};


