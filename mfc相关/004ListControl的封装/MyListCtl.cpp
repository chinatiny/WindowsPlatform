// MyListCtl.cpp : 实现文件
//

#include "stdafx.h"
#include "004ListControl的封装.h"
#include "MyListCtl.h"
#include <varargs.h>


// MyListCtl

IMPLEMENT_DYNAMIC(MyListCtl, CListCtrl)

MyListCtl::MyListCtl()
{
	m_nColumnNum = 0;
}

MyListCtl::~MyListCtl()
{
}

int MyListCtl::GetColNum()
{
	return m_nColumnNum;
}

int MyListCtl::GetRowNum()
{
	return CListCtrl::GetItemCount();
}

void MyListCtl::SetGridStyle()
{
	DWORD dwOldStyle = CListCtrl::GetExStyle();
	CListCtrl::SetExtendedStyle((dwOldStyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES) & ~LVS_EX_CHECKBOXES);
}

// 向列表中添加文本
void MyListCtl::SetItemText(int nRow, int nCol, CString strText, int nImage)
{
	if (0 == nCol)
	{
		CListCtrl::InsertItem(nRow, strText,  nImage);
	}
	else
	{
		CListCtrl::SetItemText(nRow, nCol, strText);
	}
}

void MyListCtl::SetItemText(int nRow, int nImage, int nColNum, ...)
{
	va_list argPtr;
	va_start(argPtr, nRow);
	TCHAR *pArg = va_arg(argPtr, TCHAR*);
	pArg = va_arg(argPtr, TCHAR*);
	for (int i = 0; i < nColNum; i++)
	{
		TCHAR *pArg = va_arg(argPtr, TCHAR*);
		SetItemText(nRow, i, pArg, nImage);
	}
	va_end(argPtr);

}

void MyListCtl::InsertColumn(int nCol, LPCTSTR lpszColName, int nWidth)
{
	CListCtrl::InsertColumn(nCol, lpszColName, LVCFMT_LEFT, nWidth);
	++m_nColumnNum;
}


void  MyListCtl::GetSelectedRow(std::vector<int> &vecSelectRow)
{
	CString str;
	for (int i = 0; i < CListCtrl::GetItemCount(); i++)
	{
		if (CListCtrl::GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			vecSelectRow.push_back(i);
		}
	}
}

CString MyListCtl::GetSpecItemText(int nItem, int nColNum)
{
	CString strReturn;
	if ((nItem < GetRowNum() && nItem >= 0) ||
		(nColNum < GetColNum() && nColNum >= 0))
	{
		strReturn = CListCtrl::GetItemText(nItem, nColNum);
		return strReturn;
	}
	return strReturn;
}

BEGIN_MESSAGE_MAP(MyListCtl, CListCtrl)
END_MESSAGE_MAP()



// MyListCtl 消息处理程序


