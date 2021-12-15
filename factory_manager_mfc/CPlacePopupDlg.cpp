// CPlacePopupDlg.cpp : implementation file
//

#include "pch.h"
#include "CPlacePopupDlg.h"
#include "resource.h"
#include "afxdialogex.h"
#include <mysql.h>


// CPlacePopupDlg dialog
int get_back_count(int id);
int get_exhaust_count(int id);
int get_placed_count(int id);
int get_asset_count(int id);
int get_last_id_from_table(char* tb);
char* TCHARToChar(TCHAR* str, char* charStr);
char* CStringToChar(CString str, char* charStr);
char* Unicode_2_Utf8(wchar_t* wszStr, char* charStr);

extern MYSQL* mysql;

IMPLEMENT_DYNAMIC(CPlacePopupDlg, CDialogEx)

CPlacePopupDlg::CPlacePopupDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PLACE_POPUP_DIALOG, pParent)
	, m_nRemainCount(0)
	, m_szPlace(_T(""))
	, m_nPlaceCount(0)
	, m_oleDate(COleDateTime::GetCurrentTime())
	, m_szBeizhu(_T(""))
{

}

CPlacePopupDlg::~CPlacePopupDlg()
{
}

void CPlacePopupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_REMAIN_COUNT, m_nRemainCount);
	DDX_Text(pDX, IDC_EDIT_PLACE, m_szPlace);
	DDX_Text(pDX, IDC_EDIT_PLACE_COUNT, m_nPlaceCount);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_oleDate);
	DDX_Text(pDX, IDC_EDIT_BEIZHU, m_szBeizhu);
}


BEGIN_MESSAGE_MAP(CPlacePopupDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPlacePopupDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPlacePopupDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CPlacePopupDlg message handlers


void CPlacePopupDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	m_szPlace = m_szPlace.Trim();
	m_szBeizhu = m_szBeizhu.Trim();
	if (check_inputs() == false)
		return;
	int id = get_last_id_from_table("placement_info");
	char query[300];
	char date[20], place[200]; sprintf_s(date, "%04d-%02d-%02d", m_oleDate.GetYear(), m_oleDate.GetMonth(), m_oleDate.GetDay());
	char beizhu[100];
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szBeizhu, beizhu);
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szPlace, place);
	sprintf_s(query, "insert into placement_info (id, asset_id, place, count, date, remark) values('%d', '%d', '%s', '%d', '%s', '%s')", id, m_nAssetId, place, m_nPlaceCount, date, beizhu);
	if (mysql_query(mysql, query) == 0) {
		AfxMessageBox(_T("摆放成功。"));
		CDialogEx::OnOK();
	}
	else {
		AfxMessageBox(_T("摆放失败。"));
	}
}


void CPlacePopupDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


BOOL CPlacePopupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetBackgroundColor(RGB(152, 200, 240));
	set_remain_count(m_nAssetId);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

int get_remain_asset_count(int asset_id);

void CPlacePopupDlg::set_remain_count(int asset_id)
{
	m_nRemainCount = get_remain_asset_count(asset_id);
	UpdateData(FALSE);
}


bool CPlacePopupDlg::check_inputs()
{
	UpdateData(TRUE);
	if (m_szPlace == _T(""))
	{
		AfxMessageBox(_T("请填写摆放工程。"));
		return false;
	}
	if (m_nPlaceCount == 0) {
		AfxMessageBox(_T("请填写摆放数量。"));
		return false;
	}
	else {
		if (m_nRemainCount < m_nPlaceCount)
		{
			AfxMessageBox(_T("摆放数量不能大于未处理数量。"));
			return false;
		}
	}
	return true;
}
