// CAssetInputDlg.cpp : implementation file
//

#include "pch.h"
#include "factory_manager_mfc.h"
#include "CAssetInputDlg.h"
#include "CPlacePopupDlg.h"
#include "CDetailViewDlg.h"
#include "afxdialogex.h"
#include "XLEzAutomation.h"
#include "CPrinterView.h"
#include <regex>
#include <mysql.h>

#define ID_PRINTER_VIEW 1500
#define INPUT_GRID_COLUMN_COUNT 9
#define EXCEL_COLUMN_COUNT 12
// CAssetInputDlg dialog

TCHAR g_szUsername[100];
bool g_bUserMode = false;

MYSQL* mysql;
MYSQL_RES* myRes;
MYSQL_ROW myRow;

IMPLEMENT_DYNAMIC(CAssetInputDlg, CDialogEx)

TCHAR inputGridItems[INPUT_GRID_COLUMN_COUNT][30] = { _T("No"), _T("品名"), _T("规格/型号"), _T("归属部门"),  _T("归属厂"), _T("入库日期"), _T("数量"), _T("单价"), _T("付款情况") };
TCHAR payStateOptionString[2][10] = { _T("是"), _T("不") };
TCHAR excel_header[EXCEL_COLUMN_COUNT][50] = { _T("品名"), _T("入库日期"), _T("数量"), _T("摆放工程"), _T("规格/型号"), _T("数量"), _T("单价"), _T("归属部门"), _T("归属厂"), _T("付款情况"), _T("确认日期"), _T("备注") };


char* CStringToChar(CString str, char* charStr);
char* TCHARToChar(TCHAR* str, char* charStr);
int CStringToInt(CString str);
double CStringToDouble(CString str);
void SplitDate(CString str, int arr[3]);
TCHAR* CharToTCHAR(char* charStr, TCHAR* str);
wchar_t* Utf8_2_Unicode(char* row_i, wchar_t* wszStr);
char* Unicode_2_Utf8(wchar_t* wszStr, char* charStr);
int get_last_id_from_table(char* tb);

void trim(char* str);

using namespace std;

CAssetInputDlg::CAssetInputDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HOME_DIALOG, pParent)
	, m_bNoPaid(FALSE)
	, m_name(_T(""))
	, m_model(_T(""))
	, m_owner(_T(""))
	, m_oleInputDate(COleDateTime::GetCurrentTime())
	, m_nAmount(0)
	, m_dblPrice(0)
	, m_szField(_T(""))
	, m_bPaid(TRUE)
{
	m_pPrintView = new CPrinterView(this);
}

CAssetInputDlg::~CAssetInputDlg()
{
	delete m_pPrintView;
}

void CAssetInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INPUT_GRID, m_grid);
	DDX_Radio(pDX, IDC_RADIO_YES, m_bNoPaid);
	DDX_Text(pDX, IDC_EDIT_NAME, m_name);
	DDX_Text(pDX, IDC_EDIT_MODEL, m_model);
	DDX_Text(pDX, IDC_EDIT_OWNER, m_owner);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER, m_oleInputDate);
	DDX_Text(pDX, IDC_EDIT_AMOUNT, m_nAmount);
	DDX_Text(pDX, IDC_EDIT_PRICE, m_dblPrice);
	DDX_Text(pDX, IDC_EDIT_FIELD, m_szField);
}


BEGIN_MESSAGE_MAP(CAssetInputDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAssetInputDlg::OnBnClickedOk)
	ON_WM_CTLCOLOR()
// 	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_RADIO_YES, &CAssetInputDlg::OnBnClickedRadioYes)
	ON_BN_CLICKED(IDC_RADIO_NO, &CAssetInputDlg::OnBnClickedRadioNo)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, &CAssetInputDlg::OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_PLACE, &CAssetInputDlg::OnBnClickedButtonPlace)
	ON_BN_CLICKED(IDC_BUTTON__MODIFY, &CAssetInputDlg::OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON__DELETE, &CAssetInputDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON__EXCEL, &CAssetInputDlg::OnBnClickedButtonExcel)
	ON_NOTIFY(NM_CLICK, IDC_INPUT_GRID, OnGridSelChanged)
	ON_NOTIFY(NM_DBLCLK, IDC_INPUT_GRID, OnGridDoubleClicked)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CAssetInputDlg::OnBnClickedButtonExport)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON__EXCEL_EXPORT, &CAssetInputDlg::OnBnClickedButton)
END_MESSAGE_MAP()


// CAssetInputDlg message handlers


void CAssetInputDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
// 	CDialogEx::OnOK();
}

void CAssetInputDlg::OnGridSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	CCellRange selectedRange = m_grid.GetSelectedCellRange();
	int nMinRow = selectedRange.GetMinRow();
	m_name = m_grid.GetItemText(nMinRow, 1);
	m_model = m_grid.GetItemText(nMinRow, 2);
	m_szField = m_grid.GetItemText(nMinRow, 3);
	m_owner = m_grid.GetItemText(nMinRow, 4);
	CString date = m_grid.GetItemText(nMinRow, 5);
	int arr[3];
	SplitDate(date, arr);
	m_oleInputDate.SetDate(arr[0], arr[1], arr[2]);
	m_nAmount = CStringToInt(m_grid.GetItemText(nMinRow, 6));
	m_dblPrice = CStringToDouble(m_grid.GetItemText(nMinRow, 7));
	m_bPaid = m_grid.GetItemText(nMinRow, 8) == payStateOptionString[0] ? TRUE : FALSE;
	m_bNoPaid = !m_bPaid;
	UpdateData(FALSE);
}

void CAssetInputDlg::OnGridDoubleClicked(NMHDR* pNMHDR, LRESULT* pResult)
{
	CCellRange selectedRange = m_grid.GetSelectedCellRange();
	int nMinRow = selectedRange.GetMinRow();
	int id = CStringToInt(m_grid.GetItemText(nMinRow, 0));
	CDetailViewDlg dlg;
	dlg.m_nAssetId = id;
	dlg.DoModal();
}

HBRUSH CAssetInputDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}

// 
// void CAssetInputDlg::OnPaint()
// {
// 	CPaintDC dc(this); // device context for painting
// 	HBITMAP hbm = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGROUND));
// 	BITMAP bm;
// 	HDC hdcMem = CreateCompatibleDC(dc);
// 	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
// 	GetObject(hbm, sizeof(bm), &bm);
// 	BitBlt(dc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
// 	SelectObject(hdcMem, hbmOld);
// 	DeleteDC(hdcMem);
// }
// 

BOOL CAssetInputDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
// 	SetBackgroundColor(RGB(185, 213, 237));
	SetBackgroundColor(RGB(152, 200, 240));
	setGrid();
	LoadInputData();

	addCtrlIdsToResizeController();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CAssetInputDlg::setGrid()
{
	m_grid.EnableDragAndDrop(FALSE);
	m_grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_grid.SetEditable(FALSE);
	m_grid.SetListMode(TRUE);
	m_grid.SetRowResize(FALSE);
	m_grid.SetColumnResize(TRUE);
	m_grid.SetFixedRowCount(1);
	m_grid.SetRowHeight(0, 30);
	m_grid.SetDefCellHeight(30);
	m_grid.SetDefCellWidth(120);
	m_grid.SetColumnCount(INPUT_GRID_COLUMN_COUNT);
	m_grid.SetColumnWidth(1, 250);
	m_grid.SetColumnWidth(4, 150);
	m_grid.SetFixedBkColor(RGB(0, 25, 255));
	m_grid.SetBkColor(RGB(255, 255, 255));
	for (int i = 0; i < INPUT_GRID_COLUMN_COUNT; i++)
	{
		m_grid.SetItemText(0, i, inputGridItems[i]);
		m_grid.SetItemFormat(0, i, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	m_grid.SetFixedTextColor(RGB(255, 255, 255));
}


void CAssetInputDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);


	if (m_grid.m_hWnd) {
		m_resizeController.resize(cx, cy);
	}
}


void CAssetInputDlg::OnBnClickedRadioYes()
{
	m_bNoPaid = FALSE;
	m_bPaid = TRUE;
}


void CAssetInputDlg::OnBnClickedRadioNo()
{
	m_bNoPaid = TRUE;
	m_bPaid = FALSE;
}


void CAssetInputDlg::OnBnClickedButtonInsert()
{
	UpdateData(TRUE);
	m_name = m_name.Trim();
	m_model = m_model.Trim();
	m_owner = m_owner.Trim();
	m_szField = m_szField.Trim();
	if (!check_inputs())
		return;
	char query[500];
	char name[100], model[100], owner[100];
	CStringToChar(m_name, name);
	CStringToChar(m_model, model);
	CStringToChar(m_owner, owner);
	char date[20]; sprintf_s(date, "%04d-%02d-%02d", m_oleInputDate.GetYear(), m_oleInputDate.GetMonth(), m_oleInputDate.GetDay());
	TCHAR tcDate[20]; _stprintf_s(tcDate, _T("%04d-%02d-%02d"), m_oleInputDate.GetYear(), m_oleInputDate.GetMonth(), m_oleInputDate.GetDay());
	TCHAR tcPaid[10]; 
	if (m_bPaid)
		_stprintf_s(tcPaid, _T("%s"), payStateOptionString[0]);
	else
		_stprintf_s(tcPaid, _T("%s"), payStateOptionString[1]);
	int id = get_last_id_from_table("assets_info");
	char nameUtf8[300], modelUtf8[200], ownerUtf8[100], fieldUtf8[100];
	Unicode_2_Utf8((wchar_t*)(LPCTSTR)m_name, nameUtf8);
	Unicode_2_Utf8((wchar_t*)(LPCTSTR)m_model, modelUtf8);
	Unicode_2_Utf8((wchar_t*)(LPCTSTR)m_owner, ownerUtf8);
	Unicode_2_Utf8((wchar_t*)(LPCTSTR)m_szField, fieldUtf8);
	sprintf_s(query, "insert into assets_info(id, name, in_date, count, model, price, field, owner, pay_state, exhaust, back) values('%d', '%s', '%s', '%d', '%s', '%.2f', '%s', '%s', '%d', 0, 0)",
		id, nameUtf8, date, m_nAmount, modelUtf8, m_dblPrice, fieldUtf8, ownerUtf8, m_bPaid);
	if (mysql_query(mysql, query) == 0) {
		int row_count = m_grid.GetRowCount();
		TCHAR temp[100];
		m_grid.SetRowCount(row_count + 1);
		m_grid.SetItemInt(row_count, 0, id);
		m_grid.SetItemText(row_count, 1, m_name);
		m_grid.SetItemText(row_count, 2, m_model);
		m_grid.SetItemText(row_count, 3, m_szField);
		m_grid.SetItemText(row_count, 4, m_owner);
		m_grid.SetItemText(row_count, 5, tcDate);
		m_grid.SetItemInt(row_count, 6, m_nAmount);
		m_grid.SetItemDouble(row_count, 7, m_dblPrice);
		m_grid.SetItemText(row_count, 8, tcPaid);
		for (int j = 0; j < INPUT_GRID_COLUMN_COUNT; j++)
		{
			m_grid.SetItemFormat(row_count, j, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		m_grid.Refresh();
		AfxMessageBox(_T("添加成功。"));
	}
	else {
		AfxMessageBox(_T("数据库连接失败！"));
	}
}


void CAssetInputDlg::OnBnClickedButtonPlace()
{
	CCellRange selectedRange = m_grid.GetSelectedCellRange();
	if (selectedRange.Count() == 0) {
		AfxMessageBox(_T("选择该变更的资料。"));
		return;
	}
	int nMinRow = selectedRange.GetMinRow();
	int id = CStringToInt(m_grid.GetItemText(nMinRow, 0));
	CPlacePopupDlg dlg;
	dlg.m_nAssetId = id;
	dlg.DoModal();
}


void CAssetInputDlg::OnBnClickedButtonModify()
{
	CCellRange selectedRange = m_grid.GetSelectedCellRange();
	if (selectedRange.Count() == 0) {
		AfxMessageBox(_T("选择该变更的资料。"));
		return;
	}
	int nMinRow = selectedRange.GetMinRow();
	int id = CStringToInt(m_grid.GetItemText(nMinRow, 0));
	UpdateData(TRUE);
	m_name = m_name.Trim();
	m_model = m_model.Trim();
	m_owner = m_owner.Trim();
	m_szField = m_szField.Trim();
	if (!check_inputs())
		return;
	char query[500];
	char name[100], model[100], owner[100];
	CStringToChar(m_name, name);
	CStringToChar(m_model, model);
	CStringToChar(m_owner, owner);
	char date[20]; sprintf_s(date, "%04d-%02d-%02d", m_oleInputDate.GetYear(), m_oleInputDate.GetMonth(), m_oleInputDate.GetDay());
	TCHAR tcDate[20]; _stprintf_s(tcDate, _T("%04d-%02d-%02d"), m_oleInputDate.GetYear(), m_oleInputDate.GetMonth(), m_oleInputDate.GetDay());
	TCHAR tcPaid[10];
	if (m_bPaid)
		_stprintf_s(tcPaid, _T("%s"), payStateOptionString[0]);
	else
		_stprintf_s(tcPaid, _T("%s"), payStateOptionString[1]);
	char nameUtf8[300], modelUtf8[200], ownerUtf8[100], fieldUtf8[100];
	Unicode_2_Utf8((wchar_t*)(LPCTSTR)m_name, nameUtf8);
	Unicode_2_Utf8((wchar_t*)(LPCTSTR)m_model, modelUtf8);
	Unicode_2_Utf8((wchar_t*)(LPCTSTR)m_owner, ownerUtf8);
	Unicode_2_Utf8((wchar_t*)(LPCTSTR)m_szField, fieldUtf8);
	sprintf_s(query, "update assets_info set name='%s', model='%s', field='%s', owner='%s', count='%d', price='%.2f', pay_state='%d', in_date='%s' where id='%d'",
		nameUtf8, modelUtf8, fieldUtf8, ownerUtf8, m_nAmount, m_dblPrice, m_bPaid, date, id);
	if (mysql_query(mysql, query) == 0) {
		TCHAR temp[100];
		m_grid.SetItemInt(nMinRow, 0, id);
		m_grid.SetItemText(nMinRow, 1, m_name);
		m_grid.SetItemText(nMinRow, 2, m_model);
		m_grid.SetItemText(nMinRow, 3, m_szField);
		m_grid.SetItemText(nMinRow, 4, m_owner);
		m_grid.SetItemText(nMinRow, 5, tcDate);
		m_grid.SetItemInt(nMinRow, 6, m_nAmount);
		m_grid.SetItemDouble(nMinRow, 7, m_dblPrice);
		m_grid.SetItemText(nMinRow, 8, tcPaid);
		for (int j = 0; j < INPUT_GRID_COLUMN_COUNT; j++)
		{
			m_grid.SetItemFormat(nMinRow, j, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		m_grid.Refresh();
		AfxMessageBox(_T("变更成功。"));
	}
	else {
		AfxMessageBox(_T("数据库连接失败！"));
	}
}


void CAssetInputDlg::OnBnClickedButtonDelete()
{
	CCellRange selectedRange = m_grid.GetSelectedCellRange();
	if (selectedRange.Count() == 0) {
		AfxMessageBox(_T("选择该删除的资料"));
		return;
	}
	int nMinRow = selectedRange.GetMinRow();
	int id = CStringToInt(m_grid.GetItemText(nMinRow, 0));
	if (MessageBox(_T("确定您要删除资料。"), _T("确认"), MB_YESNO) == IDYES) {
		addToDeleteHistory(id);
		char query[100];
		sprintf_s(query, "delete from assets_info where id='%d'", id);
		if (mysql_query(mysql, query) == 0) {
			m_grid.DeleteRow(nMinRow);
			m_grid.Refresh();
			sprintf_s(query, "delete from placement_info where asset_id='%d'", id);
			mysql_query(mysql, query);
			sprintf_s(query, "delete from back_info where asset_id='%d'", id);
			mysql_query(mysql, query);
			sprintf_s(query, "delete from movement_info where asset_id='%d'", id);
			mysql_query(mysql, query);
			sprintf_s(query, "delete from exhaust_info where asset_id='%d'", id);
			mysql_query(mysql, query);
			AfxMessageBox(_T("删除成功"));
		}
		else {
			AfxMessageBox(_T("删除失败"));
		}
	}
}


void CAssetInputDlg::OnBnClickedButtonExcel()
{
	TCHAR szFilters[] = _T("Excel Files (*.xls;*.xlsx)|*.xls;*.xlsx||");//|All Files (*.*)|*.*||

// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg(TRUE, _T("xlsx"), _T(""),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if (fileDlg.DoModal() == IDOK)
	{
		CString pathName = fileDlg.GetPathName();
		readExcelFile(pathName);
	}

}


bool CAssetInputDlg::check_inputs()
{
	if (m_name == _T(""))
	{
		AfxMessageBox(_T("请填写品名。"));
		return false;
	}
// 	if (m_model == _T(""))
// 	{
// 		AfxMessageBox(_T("请填写规格/型号。"));
// 		return false;
// 	}
	if (m_owner == _T("")) {
		AfxMessageBox(_T("请填写归属厂。"));
		return false;
	}
	if (m_nAmount == 0) {
		AfxMessageBox(_T("请填写数量。"));
		return false;
	}
	if (m_dblPrice == 0) {
		AfxMessageBox(_T("请填写单价。"));
		return false;
	}
	return true;
}


void CAssetInputDlg::LoadInputData()
{
	char query[100];
	sprintf_s(query, "select * from assets_info order by id");
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		m_grid.SetRowCount(res->row_count + 1);
		int i = 0;
		TCHAR temp[100];
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			i++;
			m_grid.SetItemInt(i, 0, atoi(row[0]));
			Utf8_2_Unicode(row[1], temp);
			m_grid.SetItemText(i, 1, temp);
			Utf8_2_Unicode(row[5], temp);
			m_grid.SetItemText(i, 2, temp);
			Utf8_2_Unicode(row[7], temp);
			m_grid.SetItemText(i, 3, temp);
			Utf8_2_Unicode(row[8], temp);
			m_grid.SetItemText(i, 4, temp);
			Utf8_2_Unicode(row[3], temp);
			m_grid.SetItemText(i, 5, temp);
			Utf8_2_Unicode(row[4], temp);
			m_grid.SetItemText(i, 6, temp);
			Utf8_2_Unicode(row[6], temp);
			m_grid.SetItemText(i, 7, temp);
			m_grid.SetItemText(i, 8, payStateOptionString[atoi(row[9])==0?1:0]);
			for (int j = 0; j < INPUT_GRID_COLUMN_COUNT; j++)
			{
				m_grid.SetItemFormat(i, j, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
		}
		mysql_free_result(res);
		m_grid.Refresh();
	}
}


void CAssetInputDlg::addToDeleteHistory(int id)
{
	char query[500];
	sprintf_s(query, "select * from assets_info where id='%d'", id);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		if (res->row_count > 0) {
			MYSQL_ROW row = mysql_fetch_row(res);
			char* name = row[1];
			char* kor_name = row[2];
			char* date = row[3];
			char* count = row[4];
			char* model = row[5];
			char* price = row[6];
			char* field = row[7];
			char* owner = row[8];
			CTime time = CTime::GetCurrentTime();
			char today[50];
			sprintf_s(today, "%04d-%02d-%02d", time.GetYear(), time.GetMonth(), time.GetDay());
			int del_id = get_last_id_from_table("delete_history_info");
			char username[100];
			TCHARToChar(g_szUsername, username);
			sprintf_s(query, "insert into delete_history_info (id, asset_id, name, kor_name, in_date, count, model, price, field, owner, delete_date, user) values('%d', '%d', '%s', '%s', '%s',\
				 '%s', '%s', '%s', '%s', '%s', '%s', '%s')", del_id, id, name, kor_name, date, count, model, price, field, owner, today, username);
			if (mysql_query(mysql, query)==0)
			{
			}
		}
	}
}

void CAssetInputDlg::readExcelFile(CString szFileName)
{
	CXLEzAutomation XL(FALSE);
	XL.OpenExcelFile(szFileName);
	CString first_header_str;
	first_header_str = XL.GetCellValue(1, 1);
	int row = 0;
	bool header = true;
	CString items[EXCEL_COLUMN_COUNT];
	while ((first_header_str = XL.GetCellValue(1, row + 1))!=_T(""))
	{
		row++;
		if(header && first_header_str != excel_header[0])
			continue;
		if (header) {
			for (int i = 1; i <= EXCEL_COLUMN_COUNT; i++)
			{
				if (XL.GetCellValue(i, row) != excel_header[i - 1])
				{
					AfxMessageBox(_T("不合适的文件。"));
					XL.ReleaseExcel();
					return;
				}
			}
			header = false;
			continue;
		}
		for (int i = 1; i <= EXCEL_COLUMN_COUNT; i++)
		{
			items[i - 1] = XL.GetCellValue(i, row);
		}
		if(checkItemFormats(items))
			insertExcelItems(items);
		else
			break;
	}
	m_grid.Refresh();
	XL.ReleaseExcel();
	AfxMessageBox(_T("数据导入成功"));
}


void CAssetInputDlg::insertExcelItems(CString* items)
{
	char utf8Items[13][100];
	items[6].Format(_T("%.2f"), CStringToDouble(items[6]));
	items[2].Format(_T("%d"), CStringToInt(items[2]));
	int nPlaceCount = CStringToInt(items[5]);
	if (nPlaceCount == 0)
		nPlaceCount = CStringToInt(items[2]);
	items[5].Format(_T("%d"), nPlaceCount);
	for (int i = 0; i < EXCEL_COLUMN_COUNT; i++)
	{
		Unicode_2_Utf8((TCHAR*)(LPCTSTR)items[i], utf8Items[i]);
	}
	int id = get_last_id_from_table("assets_info");
	char* name = utf8Items[0];
	char* date = utf8Items[1];
	char* count = utf8Items[2];
	char* model = utf8Items[4];
	char* place = utf8Items[3];
	char* place_count = utf8Items[5];
	char* price = utf8Items[6];
	char* field = utf8Items[7];
	char* owner = utf8Items[8];
	char* pay_state = utf8Items[9];
	char* place_date = utf8Items[10];
	char* beizhu = utf8Items[11];
	char query[500];
	int nPayState = 0;
	if (pay_state[0] != 0)
		nPayState = 1;
	sprintf_s(query, "insert into assets_info (id, name, kor_name, in_date, count, model, price, field, owner, pay_state, exhaust, back) values('%d', '%s', '', '%s', '%s', '%s', '%s', '%s', '%s', '%d', '0', '0')",
		id, name, date, count, model, price, field, owner, nPayState);
	if (mysql_query(mysql, query) == 0) {
		int place_id = get_last_id_from_table("placement_info");
		if (place_date[0] == 0)
			strcpy(place_date, date);
		sprintf_s(query, "insert into placement_info (id, asset_id, place, count, date, remark) values('%d', '%d', '%s', '%s', '%s', '%s')", place_id, id, place, count, place_date, beizhu);
		if (mysql_query(mysql, query) == 0) {
			int row_count = m_grid.GetRowCount();
			m_grid.SetRowCount(row_count + 1);\
			m_grid.SetItemInt(row_count, 0, id);
			m_grid.SetItemText(row_count, 1, items[0]);
			m_grid.SetItemText(row_count, 2, items[4]);
			m_grid.SetItemText(row_count, 3, items[7]);
			m_grid.SetItemText(row_count, 4, items[8]);
			m_grid.SetItemText(row_count, 5, items[1]);
			m_grid.SetItemText(row_count, 6, items[2]);
			m_grid.SetItemText(row_count, 7, items[6]);
			if (nPayState == 0)
				m_grid.SetItemText(row_count, 8, payStateOptionString[1]);
			else
				m_grid.SetItemText(row_count, 8, payStateOptionString[0]);
			for (int j = 0; j < INPUT_GRID_COLUMN_COUNT; j++)
			{
				m_grid.SetItemFormat(row_count, j, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
		}
	}
}


void CAssetInputDlg::addCtrlIdsToResizeController()
{
	CRect rect;
	GetClientRect(rect);
	m_resizeController.m_pParent = this;
	m_resizeController.setInitialSize(rect);
	m_resizeController.addId(IDC_STATIC_INPUT);
	m_resizeController.addId(IDC_STATIC_NAME);
	m_resizeController.addId(IDC_EDIT_NAME);
	m_resizeController.addId(IDC_STATIC_MODEL);
	m_resizeController.addId(IDC_EDIT_MODEL);
	m_resizeController.addId(IDC_STATIC_FIELD);
	m_resizeController.addId(IDC_EDIT_FIELD);
	m_resizeController.addId(IDC_STATIC_OWNER);
	m_resizeController.addId(IDC_EDIT_OWNER);
	m_resizeController.addId(IDC_STATIC_DATE);
	m_resizeController.addId(IDC_DATETIMEPICKER);
	m_resizeController.addId(IDC_STATIC_AMOUNT);
	m_resizeController.addId(IDC_EDIT_AMOUNT);
	m_resizeController.addId(IDC_STATIC_PRICE);
	m_resizeController.addId(IDC_EDIT_PRICE);
	m_resizeController.addId(IDC_STATIC_PAY_STATE);
	m_resizeController.addId(IDC_RADIO_YES);
	m_resizeController.addId(IDC_RADIO_NO);
	m_resizeController.addId(IDC_BUTTON_INSERT);
	m_resizeController.addId(IDC_BUTTON_PLACE);
	m_resizeController.addId(IDC_BUTTON__MODIFY);
	m_resizeController.addId(IDC_BUTTON__DELETE);
	m_resizeController.addId(IDC_BUTTON__EXCEL);
	m_resizeController.addId(IDC_BUTTON_EXPORT);
	m_resizeController.addId(IDC_BUTTON__EXCEL_EXPORT);
	m_resizeController.addId(IDC_INPUT_GRID);
}


void CAssetInputDlg::OnBnClickedButtonExport()
{
	m_pPrintView->OnFilePrint();
}


int CAssetInputDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pPrintView->Create(CRect(0, 0, 20, 20), this, ID_PRINTER_VIEW);
	m_pPrintView->ShowWindow(SW_HIDE);

	return 0;
}


void CAssetInputDlg::OnBnClickedButton()
{
	TCHAR szFilters[] = _T("Excel Files (*.xls;*.xlsx)|*.xls;*.xlsx||");//|All Files (*.*)|*.*||

// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg(FALSE, _T("xlsx"), _T(""),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if (fileDlg.DoModal() == IDOK)
	{
		CString pathName = fileDlg.GetPathName();
		writeExcelFile(pathName);
	}

}

void CAssetInputDlg::writeExcelFile(CString filePath)
{
	char query[500];
	TCHAR temp[200];
	CXLEzAutomation XL(FALSE);
	sprintf_s(query, "select b.name, b.in_date, b.count, a.place, b.model, a.count, b.price, b.field, b.owner, b.pay_state, a.date, a.remark from placement_info as a, assets_info as b  where  a.asset_id = b.id and a.count > 0");
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		int line = 1;
		for (int i = 1; i <= EXCEL_COLUMN_COUNT; i++)
		{
			XL.SetCellValue(i, line, excel_header[i-1]);
		}
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			line++;
			for (int i = 1; i <= EXCEL_COLUMN_COUNT; i++)
			{
				if(i != 10)
					Utf8_2_Unicode(row[i-1], temp);
				else {
					_stprintf_s(temp, _T("%s"), payStateOptionString[atoi(row[9])==0?1:0]);
				}
				XL.SetCellValue(i, line, temp);
			}
		}
		mysql_free_result(res);
		XL.SaveFileAs(filePath);
		XL.ReleaseExcel();
		AfxMessageBox(_T("导出成功"));
	}
}


bool CAssetInputDlg::checkItemFormats(CString* items)
{
	regex date_format("^[0-9]{4}-[0-9]{2}-[0-9]{2}$");
	regex place_date_format("^([0-9]{4}-[0-9]{2}-[0-9]{2})?$");
	regex price_format("^([0-9]+(\\.[0-9]+)?)?$");
	regex number_format("^[0-9]+(\\.[0-9]+)?$");
	char temp[100];
	CStringToChar(items[1], temp);//input date
	if (regex_match(temp, date_format) == false) {
		AfxMessageBox(_T("入库日期格式错误。确认Excel文件。"));
		return false;
	}
	CStringToChar(items[2], temp);
	if (regex_match(temp, number_format) == false) {
		AfxMessageBox(_T("数量格式错误。确认Excel文件。"));
		return false;
	}
	CStringToChar(items[6], temp);
	if (regex_match(temp, price_format) == false) {
		AfxMessageBox(_T("单价格式错误。确认Excel文件。"));
		return false;
	}
	CStringToChar(items[10], temp);
	if (regex_match(temp, place_date_format) == false) {
		AfxMessageBox(_T("确认日期格式错误。确认Excel文件。"));
		return false;
	}
	return true;
}
