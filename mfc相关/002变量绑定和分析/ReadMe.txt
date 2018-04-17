触发函数：
UpdateData ->
BOOL CWnd::UpdateData(BOOL bSaveAndValidate)
{
	CDataExchange dx(this, bSaveAndValidate);
	DoDataExchange(&dx);
}

CDataExchange dx(this, bSaveAndValidate) ->
//1. 构造 一个CDataExchange对象
CDataExchange::CDataExchange(CWnd* pDlgWnd, BOOL bSaveAndValidate)
{
	ASSERT_VALID(pDlgWnd);
	m_bSaveAndValidate = bSaveAndValidate;
	m_pDlgWnd = pDlgWnd;
	m_idLastControl = 0;
}

DoDataExchange(&dx)->
//2. 在当前的窗口类执行DoDataExchange，其中的CDataExchange* 就是上一步构造出来的
void CMy002变量绑定和分析Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Text(pDX, IDC_EDIT1, m_strEdit);
}

//3. CDialogEx::DoDataExchange(pDX)->
其实是一个空函数


//4. 	DDX_Text(pDX, IDC_EDIT1, m_strEdit) ->
其实直接调用SetWindowsText
HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
if (pDX->m_bSaveAndValidate)
{
	int nLen = ::GetWindowTextLength(hWndCtrl);
	::GetWindowText(hWndCtrl, value.GetBufferSetLength(nLen), nLen+1);
	value.ReleaseBuffer();
}
else
{
	AfxSetWindowText(hWndCtrl, value);
}
	
