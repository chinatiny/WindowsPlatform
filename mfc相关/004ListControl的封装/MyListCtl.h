#pragma once
#include <vector>


// MyListCtl

class MyListCtl : public CListCtrl
{
	DECLARE_DYNAMIC(MyListCtl)

public:
	MyListCtl();
	virtual ~MyListCtl();
	//设置网格风格
	void SetGridStyle();
	//获取多少列
	int GetColNum();
	//获取多少行
	int GetRowNum();

	//获取当前选择行
	void GetSelectedRow(std::vector<int> &vecSelectRow);

	//获取nItem行和nColumnNum列的文本
	CString GetSpecItemText(int nRow, int nColNum);
	//设置nItem行nSubItem列的文本
	void SetItemText(int nRow, int nCol, CString strText, int nImage = -1);

	//设置nItem行的nColNum个元素,如果不存在图片nImage 设置为-1
	void SetItemText(int nRow, int nImage, int nColNum, ...);
	//设置列
	void InsertColumn(int nCol, LPCTSTR lpszColName, int nWidth);


private:
	int m_nColumnNum;

protected:
	DECLARE_MESSAGE_MAP()
};


