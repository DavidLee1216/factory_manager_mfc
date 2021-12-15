
// factory_manager_mfcDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "factory_manager_mfc.h"
#include "factory_manager_mfcDlg.h"
#include "afxdialogex.h"

extern bool g_bUserMode;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TAB_HEIGHT 30

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CfactorymanagermfcDlg dialog



CfactorymanagermfcDlg::CfactorymanagermfcDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FACTORY_MANAGER_MFC_DIALOG, pParent)
{
	m_pAssetInputDlg = 0;
	m_pPlaceDlg = 0;
	m_pSearchDlg = 0;
	m_pUserManageDlg = 0;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CfactorymanagermfcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tabControl);
}

BEGIN_MESSAGE_MAP(CfactorymanagermfcDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CfactorymanagermfcDlg::OnBnClickedOk)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CfactorymanagermfcDlg::OnTcnSelchangeTab1)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CLOSE, &CfactorymanagermfcDlg::OnBnClickedClose)
END_MESSAGE_MAP()


// CfactorymanagermfcDlg message handlers

BOOL CfactorymanagermfcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CRect rect;
	GetClientRect(rect);
	if (g_bUserMode) {
		m_tabControl.InsertItem(0, _T("资产入库摆放"));
		m_tabControl.InsertItem(1, _T("摆放资产管理"));
		m_tabControl.InsertItem(2, _T("资产材料查看"));
		m_tabControl.InsertItem(3, _T("设定"));
		m_tabControl.SetCurSel(0);
	}
	else {
		m_tabControl.InsertItem(0, _T("资产材料查看"));
	}
	rect.top = rect.top + 5;
	rect.left = rect.left + 5;
	rect.right = rect.right - 5;
	rect.bottom = rect.bottom - 5;
	m_tabControl.AdjustRect(TRUE, rect);
	m_tabControl.SetItemSize(CSize(50, TAB_HEIGHT));
	
	if (g_bUserMode && m_pAssetInputDlg == NULL) {
		m_pAssetInputDlg = new CAssetInputDlg;
		m_pAssetInputDlg->Create(IDD_HOME_DIALOG, this);
		m_pAssetInputDlg->ShowWindow(SW_SHOW);
	}
	if (g_bUserMode && m_pPlaceDlg == NULL) {
		m_pPlaceDlg = new CPlaceDlg;
		m_pPlaceDlg->Create(IDD_PLACE_DIALOG, this);
		m_pPlaceDlg->ShowWindow(SW_HIDE);
	}
	if (m_pSearchDlg == NULL) {
		m_pSearchDlg = new CSearchDlg;
		m_pSearchDlg->Create(IDD_SEARCH_DIALOG, this);
		if (g_bUserMode)
			m_pSearchDlg->ShowWindow(SW_HIDE);
		else
			m_pSearchDlg->ShowWindow(SW_SHOW);
	}
	if (g_bUserMode && m_pUserManageDlg == NULL) {
		m_pUserManageDlg = new CUserManageDlg;
		m_pUserManageDlg->Create(IDD_SETTING_DIALOG, this);
		m_pUserManageDlg->ShowWindow(SW_HIDE);
	}


	GetClientRect(rect);
	MoveWindow(rect);

	// TODO: Add extra initialization here
	ShowWindow(SW_MAXIMIZE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CfactorymanagermfcDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CfactorymanagermfcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CfactorymanagermfcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CfactorymanagermfcDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
/*	CDialogEx::OnOK();*/
}


void CfactorymanagermfcDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	int sel = m_tabControl.GetCurSel();
	if (g_bUserMode) {
		if (sel == 0) {
			m_pAssetInputDlg->ShowWindow(SW_SHOW);
			m_pPlaceDlg->ShowWindow(SW_HIDE);
			m_pSearchDlg->ShowWindow(SW_HIDE);
			m_pUserManageDlg->ShowWindow(SW_HIDE);
		}
		else if (sel == 1) {
			m_pAssetInputDlg->ShowWindow(SW_HIDE);
			m_pPlaceDlg->ShowWindow(SW_SHOW);
			m_pSearchDlg->ShowWindow(SW_HIDE);
			m_pUserManageDlg->ShowWindow(SW_HIDE);
		}
		else if (sel == 2) {
			m_pAssetInputDlg->ShowWindow(SW_HIDE);
			m_pPlaceDlg->ShowWindow(SW_HIDE);
			m_pSearchDlg->ShowWindow(SW_SHOW);
			m_pUserManageDlg->ShowWindow(SW_HIDE);
		}
		else {
			m_pAssetInputDlg->ShowWindow(SW_HIDE);
			m_pPlaceDlg->ShowWindow(SW_HIDE);
			m_pSearchDlg->ShowWindow(SW_HIDE);
			m_pUserManageDlg->ShowWindow(SW_SHOW);
		}
	}
	else {
		m_pSearchDlg->ShowWindow(SW_SHOW);
	}
}


void CfactorymanagermfcDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(rect);
	if (m_tabControl.m_hWnd)
	{
		rect.top = rect.top + 5;
		rect.left = rect.left + 5;
		rect.right = rect.right - 5;
		rect.bottom = rect.bottom - 5;
		m_tabControl.MoveWindow(rect);
	}
	CRect rect1 = rect;
	if (m_pAssetInputDlg && m_pAssetInputDlg->m_hWnd) {
		rect.top = rect1.top + TAB_HEIGHT + 5;
		rect.bottom = rect1.bottom - 5;
		rect.left = rect1.left + 5;
		rect.right = rect1.right - 5;
		m_pAssetInputDlg->MoveWindow(rect);
	}
	if (m_pPlaceDlg && m_pPlaceDlg->m_hWnd) {
		rect.top = rect1.top + TAB_HEIGHT + 5;
		rect.bottom = rect1.bottom - 5;
		rect.left = rect1.left + 5;
		rect.right = rect1.right - 5;
		m_pPlaceDlg->MoveWindow(rect);
	}
	if (m_pSearchDlg && m_pSearchDlg->m_hWnd) {
		rect.top = rect1.top + TAB_HEIGHT + 5;
		rect.bottom = rect1.bottom - 5;
		rect.left = rect1.left + 5;
		rect.right = rect1.right - 5;
		m_pSearchDlg->MoveWindow(rect);
	}
	if (m_pUserManageDlg && m_pUserManageDlg->m_hWnd) {
		rect.top = rect1.top + TAB_HEIGHT + 5;
		rect.bottom = rect1.bottom - 5;
		rect.left = rect1.left + 5;
		rect.right = rect1.right - 5;
		m_pUserManageDlg->MoveWindow(rect);
	}

}


BOOL CfactorymanagermfcDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_pAssetInputDlg)
		delete m_pAssetInputDlg;
	if (m_pPlaceDlg)
		delete m_pPlaceDlg;
	if (m_pSearchDlg)
		delete m_pSearchDlg;
	if (m_pUserManageDlg)
		delete m_pUserManageDlg;

	return CDialogEx::DestroyWindow();
}


void CfactorymanagermfcDlg::OnBnClickedClose()
{
	CDialogEx::OnCancel();
}
