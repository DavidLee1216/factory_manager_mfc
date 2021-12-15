// CLoginDlg.cpp : implementation file
//

#include "pch.h"
#include "CLoginDlg.h"
#include "resource.h"
#include "afxdialogex.h"
#include <mysql.h>

// CLoginDlg dialog
extern MYSQL mysql;
extern TCHAR g_szUsername[100];
extern bool g_bUserMode;

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

char* CStringToChar(CString str, char* charStr);
char* TCHARToChar(TCHAR* str, char* charStr);
int CStringToInt(CString str);
double CStringToDouble(CString str);
void SplitDate(CString str, int arr[3]);
TCHAR* CharToTCHAR(char* charStr, TCHAR* str);
wchar_t* Utf8_2_Unicode(char* row_i, wchar_t* wszStr);
char* Unicode_2_Utf8(wchar_t* wszStr, char* charStr);
int get_last_id_from_table(char* tb);
int get_remain_asset_count(int asset_id);
bool check_user_pass(char* username, char* password);

CLoginDlg::CLoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LOGIN, pParent)
	, m_szUsername(_T(""))
	, m_szPassword(_T(""))
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_szUsername);
	DDX_Text(pDX, IDC_EDIT2, m_szPassword);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLoginDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CLoginDlg::OnBnClickedButtonBrowse)
END_MESSAGE_MAP()


// CLoginDlg message handlers


void CLoginDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	char username[100], password[100];
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szUsername, username);
	CStringToChar(m_szPassword, password);
	if (check_user_pass(username, password))
	{
		g_bUserMode = true;
		_tcscpy_s(g_szUsername, m_szUsername);
		CDialogEx::OnOK();
	}
	else {
		AfxMessageBox(_T("用户不存在或密码错误。"));
		return;
	}
}


void CLoginDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetBackgroundColor(RGB(152, 200, 240));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CLoginDlg::OnBnClickedButtonBrowse()
{
	_tcscpy_s(g_szUsername, _T(""));
	CDialogEx::OnOK();
}
