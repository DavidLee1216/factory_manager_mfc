// CUserManageDlg.cpp : implementation file
//

#include "pch.h"
#include "factory_manager_mfc.h"
#include "CUserManageDlg.h"
#include "afxdialogex.h"
#include <mysql.h>

// CUserManageDlg dialog
extern TCHAR g_szUsername[100];
extern MYSQL* mysql;

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
bool set_pass(char* username, char* password);
bool check_user(char* username);
bool modify_user(char* old_user, char* new_user);
bool delete_user(char* user);

IMPLEMENT_DYNAMIC(CUserManageDlg, CDialogEx)

CUserManageDlg::CUserManageDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SETTING_DIALOG, pParent)
	, m_szOldPassword(_T(""))
	, m_szNewPassword1(_T(""))
	, m_szNewPassword2(_T(""))
	, m_szUsername(_T(""))
	, m_szUserPass(_T(""))
{

}

CUserManageDlg::~CUserManageDlg()
{
}

void CUserManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_OLD_PASSWORD, m_szOldPassword);
	DDX_Text(pDX, IDC_EDIT_NEW_PASSWORD1, m_szNewPassword1);
	DDX_Text(pDX, IDC_EDIT_NEW_PASSWORD2, m_szNewPassword2);
	DDX_Control(pDX, IDC_LIST1, m_userList);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_szUsername);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_szUserPass);
}


BEGIN_MESSAGE_MAP(CUserManageDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CUserManageDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDOK, &CUserManageDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CUserManageDlg::OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CUserManageDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, &CUserManageDlg::OnBnClickedButtonChange)
	ON_LBN_SELCHANGE(IDC_LIST1, &CUserManageDlg::OnLbnSelchangeList1)
END_MESSAGE_MAP()


// CUserManageDlg message handlers


void CUserManageDlg::OnBnClickedButtonAdd()
{
	UpdateData(TRUE);
	m_szUsername = m_szUsername.Trim();
	if (m_szUsername == _T("")) {
		AfxMessageBox(_T("请填写用户名。"));
		return;
	}
	char username[100];
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szUsername, username);
	if (check_user(username) == true) {
		AfxMessageBox(_T("用户已存在。"));
		return;
	}
	char query[200];
	int id = get_last_id_from_table("user_info");
	sprintf_s(query, "insert into user_info (id, username, password) values('%d', '%s', '')", id, username);
	if (mysql_query(mysql, query) == 0)
	{
		m_userList.AddString(m_szUsername);
	}
}


void CUserManageDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
}


BOOL CUserManageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetBackgroundColor(RGB(152, 200, 240));
	loadUserList();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CUserManageDlg::OnBnClickedButtonModify()
{
	UpdateData(TRUE);
	m_szUsername = m_szUsername.Trim();
	if (m_szUsername == _T("")) {
		AfxMessageBox(_T("请填写用户名。"));
		return;
	}
	char username[100];
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szUsername, username);
	if (check_user(username) == true) {
		AfxMessageBox(_T("用户已存在。"));
		return;
	}
	int idx = m_userList.GetCurSel();
	if (idx == -1) {
		AfxMessageBox(_T("请选择用户名。"));
		return;
	}
	TCHAR oldUsername[100];
	m_userList.GetText(idx, oldUsername);
	char old_username[100];
	Unicode_2_Utf8(oldUsername, old_username);
	modify_user(old_username, username);
	m_userList.DeleteString(idx);
	m_userList.InsertString(idx, m_szUsername);
}


void CUserManageDlg::OnBnClickedButtonDelete()
{
	int idx = m_userList.GetCurSel();
	if (idx == -1) {
		AfxMessageBox(_T("请选择用户名。"));
		return;
	}
	TCHAR oldUsername[100];
	m_userList.GetText(idx, oldUsername);
	char old_username[100];
	Unicode_2_Utf8(oldUsername, old_username);
	delete_user(old_username);
	m_userList.DeleteString(idx);
}


void CUserManageDlg::OnBnClickedButtonChange()
{
	UpdateData(TRUE);
	char username[100], oldPass[100], newPass1[100], newPass2[100];
	Unicode_2_Utf8(g_szUsername, username);
	CStringToChar(m_szOldPassword, oldPass);
	CStringToChar(m_szNewPassword1, newPass1);
	CStringToChar(m_szNewPassword2, newPass2);
	char query[200];
	if (check_user_pass(username, oldPass)) {
		if (m_szNewPassword1.Compare(m_szNewPassword2) == 0) {
			set_pass(username, newPass1);
			AfxMessageBox(_T("变更成功。"));
		}
		else {
			AfxMessageBox(_T("新密码不匹配。"));
		}

	}
	else {
		AfxMessageBox(_T("旧密码不正确。"));
	}
}


void CUserManageDlg::loadUserList()
{
	char query[100];
	sprintf_s(query, "select username from user_info");
	if (mysql_query(mysql, query) == 0) {
		TCHAR temp[100];
		MYSQL_RES* res = mysql_store_result(mysql);
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			Utf8_2_Unicode(row[0], temp);
			m_userList.AddString(temp);
		}
		mysql_free_result(res);
	}
}


void CUserManageDlg::OnLbnSelchangeList1()
{
	int idx = m_userList.GetCurSel();
	if (idx == -1)
		return;
	TCHAR tcUsername[100];
	m_userList.GetText(idx, tcUsername);
	char username[100];
	Unicode_2_Utf8(tcUsername, username);
	if (_tcscmp(g_szUsername, _T("admin")) == 0) {
		char query[200];
		sprintf_s(query, "select password from user_info where username='%s'", username);
		if (mysql_query(mysql, query) == 0) {
			MYSQL_RES* res = mysql_store_result(mysql);
			if (res->row_count > 0) {
				MYSQL_ROW row = mysql_fetch_row(res);
				TCHAR temp[100];
				Utf8_2_Unicode(row[0], temp);
				m_szUserPass = temp;
				UpdateData(FALSE);
			}
			mysql_free_result(res);
		}
	}
}
