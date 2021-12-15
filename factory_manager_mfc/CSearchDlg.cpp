// CSearchDlg.cpp : implementation file
//

#include "pch.h"
#include "factory_manager_mfc.h"
#include "CSearchDlg.h"
#include "afxdialogex.h"
#include <mysql.h>

#define INPUT_GRID_COLUMN_COUNT 8
#define PLACE_GRID_COLUMN_COUNT 11
#define MOVE_GRID_COLUMN_COUNT 8
#define BACK_GRID_COLUMN_COUNT 9
#define EXHAUST_GRID_COLUMN_COUNT 9
#define HISTORY_GRID_COLUMN_COUNT 10

IMPLEMENT_DYNAMIC(CSearchDlg, CDialogEx)

TCHAR inputSearchGridItems[INPUT_GRID_COLUMN_COUNT][30] = { _T("No") , _T("品名"), _T("入库日期") , _T("规格/型号") , _T("归属部门") , _T("数量") , _T("单价") , _T("付款情况") };
TCHAR placeSearchGridItems[PLACE_GRID_COLUMN_COUNT][30] = { _T("资产No") , _T("摆放No"), _T("品名"), _T("摆放工程") , _T("归属部门") , _T("入库日期") , _T("摆放日期") , _T("规格/型号") , _T("数量") , _T("单价") , _T("备注") };
TCHAR moveSearchGridItems[MOVE_GRID_COLUMN_COUNT][30] = { _T("资产No"), _T("移动No") , _T("品名"), _T("规格/型号") , _T("移动日期") , _T("前工程") , _T("后工程") , _T("数量") };
TCHAR backSearchGridItems[BACK_GRID_COLUMN_COUNT][30] = { _T("资产No"), _T("返送No") , _T("品名"), _T("规格/型号") , _T("返送日期") , _T("返送工程") , _T("数量") , _T("备注") , _T("返送备注") };
TCHAR exhaustSearchGridItems[EXHAUST_GRID_COLUMN_COUNT][30] = { _T("资产No"), _T("处置No") , _T("品名"), _T("规格/型号") , _T("处置日期") , _T("处置工程") , _T("数量") , _T("备注") , _T("处置备注") };
TCHAR delHistorySearchGridItems[HISTORY_GRID_COLUMN_COUNT][30] = { _T("资产No"), _T("删除No") , _T("品名"), _T("入库日期") , _T("规格/型号") , _T("数量") , _T("单价") , _T("归属厂") , _T("删除日期") , _T("删除用户") };

extern TCHAR payStateOptionString[2][10];

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

extern bool g_bUserMode;

CSearchDlg::CSearchDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SEARCH_DIALOG, pParent)
	, m_szName(_T(""))
	, m_szPlace(_T(""))
	, m_oleDateFrom(COleDateTime::GetCurrentTime())
	, m_oleDateTo(COleDateTime::GetCurrentTime())
	, m_szModel(_T(""))
	, m_bDateSearch(FALSE)
{

}

CSearchDlg::~CSearchDlg()
{
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INPUT_SEARCH_GRID, m_inputGrid);
	DDX_Control(pDX, IDC_SEARCH_PLACE_GRID, m_placeGrid);
	DDX_Control(pDX, IDC_MOVE_SEARCH_GRID, m_moveGrid);
	DDX_Control(pDX, IDC_BACK_GRID, m_backGrid);
	DDX_Control(pDX, IDC_EXHAUST_GRID, m_exhaustGrid);
	DDX_Control(pDX, IDC_DELETE_HISTORY_GRID, m_delHistoryGrid);
	DDX_Control(pDX, IDC_COMBO_SEARCH_SCOPE, m_comboSearchScope);
	DDX_Text(pDX, IDC_EDIT_PRODUCT_NAME, m_szName);
	DDX_Text(pDX, IDC_EDIT_PLACE, m_szPlace);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_FROM, m_oleDateFrom);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_TO, m_oleDateTo);
	DDX_Text(pDX, IDC_EDIT_MODEL, m_szModel);
	DDX_Check(pDX, IDC_CHECK_DATE, m_bDateSearch);
}


BEGIN_MESSAGE_MAP(CSearchDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSearchDlg::OnBnClickedOk)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBO_SEARCH_SCOPE, &CSearchDlg::OnCbnSelchangeComboSearchScope)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CSearchDlg::OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_VIEW, &CSearchDlg::OnBnClickedButtonView)
END_MESSAGE_MAP()


// CSearchDlg message handlers


void CSearchDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
}


BOOL CSearchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetBackgroundColor(RGB(152, 200, 240));
	setComboSearchScope();

	setInputGrid();
	setPlaceGrid();
	setMoveGrid();
	setBackGrid();
	setExhaustGrid();
	setDelHistoryGrid();

	addCtrlIdsToResizeController();

	m_placeGrid.ShowWindow(SW_HIDE);
	m_moveGrid.ShowWindow(SW_HIDE);
	m_backGrid.ShowWindow(SW_HIDE);
	m_exhaustGrid.ShowWindow(SW_HIDE);
	m_delHistoryGrid.ShowWindow(SW_HIDE);

	OnBnClickedButtonView();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CSearchDlg::setInputGrid()
{
	m_inputGrid.EnableDragAndDrop(FALSE);
	m_inputGrid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_inputGrid.SetEditable(FALSE);
	m_inputGrid.SetListMode(TRUE);
	m_inputGrid.SetRowResize(FALSE);
	m_inputGrid.SetColumnResize(TRUE);
	m_inputGrid.SetFixedRowCount(1);
	m_inputGrid.SetRowHeight(0, 30);
	m_inputGrid.SetColumnCount(INPUT_GRID_COLUMN_COUNT);
	m_inputGrid.SetDefCellHeight(30);
	m_inputGrid.SetDefCellWidth(200);
	m_inputGrid.SetColumnWidth(0, 100);
	m_inputGrid.SetColumnWidth(1, 300);
	m_inputGrid.SetColumnWidth(2, 200);
	m_inputGrid.SetColumnWidth(3, 200);
	m_inputGrid.SetColumnWidth(4, 200);
	m_inputGrid.SetColumnWidth(6, 150);
	m_inputGrid.SetFixedBkColor(RGB(0, 25, 255));
	m_inputGrid.SetBkColor(RGB(255, 255, 255));
	m_inputGrid.EnableColumnHide(TRUE);
	TCHAR temp[50];
	for (int i = 0; i < INPUT_GRID_COLUMN_COUNT; i++)
	{
		_tcscpy_s(temp, inputSearchGridItems[i]);
		m_inputGrid.SetItemText(0, i, temp);
		m_inputGrid.SetItemFormat(0, i, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	m_inputGrid.SetFixedTextColor(RGB(255, 255, 255));
}


void CSearchDlg::setPlaceGrid()
{
	m_placeGrid.EnableDragAndDrop(FALSE);
	m_placeGrid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_placeGrid.SetEditable(FALSE);
	m_placeGrid.SetFixedRowCount(1);
	m_placeGrid.SetRowHeight(0, 30);
	m_placeGrid.SetColumnCount(PLACE_GRID_COLUMN_COUNT);
	m_placeGrid.SetDefCellHeight(30);
	m_placeGrid.SetDefCellWidth(200);
	m_placeGrid.SetColumnWidth(0, 100);
	m_placeGrid.SetItemFormat(0, 0, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_placeGrid.SetColumnWidth(2, 250);
	m_placeGrid.SetColumnWidth(3, 150);
	m_placeGrid.SetColumnWidth(4, 150);
	m_placeGrid.SetColumnWidth(5, 150);
	m_placeGrid.SetColumnWidth(6, 150);
	m_placeGrid.SetColumnWidth(7, 150);
	m_placeGrid.SetColumnWidth(9, 150);
	m_placeGrid.SetItemFormat(0, 1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_placeGrid.SetFixedBkColor(RGB(0, 25, 255));
	m_placeGrid.SetBkColor(RGB(255, 255, 255));
	m_placeGrid.EnableColumnHide(TRUE);
	TCHAR temp[50];
	for (int i = 0; i < PLACE_GRID_COLUMN_COUNT; i++)
	{
		_tcscpy_s(temp, placeSearchGridItems[i]);
		m_placeGrid.SetItemText(0, i, temp);
	}
	m_placeGrid.SetFixedTextColor(RGB(255, 255, 255));
}


void CSearchDlg::setMoveGrid()
{
	m_moveGrid.EnableDragAndDrop(FALSE);
	m_moveGrid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_moveGrid.SetEditable(FALSE);
	m_moveGrid.SetFixedRowCount(1);
	m_moveGrid.SetRowHeight(0, 30);
	m_moveGrid.SetColumnCount(MOVE_GRID_COLUMN_COUNT);
	m_moveGrid.SetDefCellHeight(30);
	m_moveGrid.SetDefCellWidth(120);
	m_moveGrid.SetColumnWidth(0, 100);
	m_moveGrid.SetItemFormat(0, 0, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_moveGrid.SetColumnWidth(2, 250);
	m_moveGrid.SetColumnWidth(3, 150);
	m_moveGrid.SetColumnWidth(4, 150);
	m_moveGrid.SetColumnWidth(5, 150);
	m_moveGrid.SetColumnWidth(6, 150);
	m_moveGrid.SetItemFormat(0, 1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_moveGrid.SetFixedBkColor(RGB(0, 25, 255));
	m_moveGrid.SetBkColor(RGB(255, 255, 255));
	m_moveGrid.EnableColumnHide(TRUE);
	TCHAR temp[50];
	for (int i = 0; i < MOVE_GRID_COLUMN_COUNT; i++)
	{
		_tcscpy_s(temp, moveSearchGridItems[i]);
		m_moveGrid.SetItemText(0, i, temp);
	}
	m_moveGrid.SetFixedTextColor(RGB(255, 255, 255));
}


void CSearchDlg::setBackGrid()
{
	m_backGrid.EnableDragAndDrop(FALSE);
	m_backGrid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_backGrid.SetEditable(FALSE);
	m_backGrid.SetFixedRowCount(1);
	m_backGrid.SetRowHeight(0, 30);
	m_backGrid.SetColumnCount(BACK_GRID_COLUMN_COUNT);
	m_backGrid.SetDefCellHeight(30);
	m_backGrid.SetDefCellWidth(120);
	m_backGrid.SetColumnWidth(0, 100);
	m_backGrid.SetItemFormat(0, 0, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_backGrid.SetColumnWidth(2, 250);
	m_backGrid.SetColumnWidth(3, 150);
	m_backGrid.SetColumnWidth(4, 150);
	m_backGrid.SetColumnWidth(5, 150);
	m_backGrid.SetColumnWidth(6, 150);
	m_backGrid.SetColumnWidth(8, 150);
	m_backGrid.SetItemFormat(0, 1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_backGrid.SetFixedBkColor(RGB(0, 25, 255));
	m_backGrid.SetBkColor(RGB(255, 255, 255));
	m_backGrid.EnableColumnHide(TRUE);
	TCHAR temp[50];
	for (int i = 0; i < BACK_GRID_COLUMN_COUNT; i++)
	{
		_tcscpy_s(temp, backSearchGridItems[i]);
		m_backGrid.SetItemText(0, i, temp);
	}
	m_backGrid.SetFixedTextColor(RGB(255, 255, 255));
}


void CSearchDlg::setExhaustGrid()
{
	m_exhaustGrid.EnableDragAndDrop(FALSE);
	m_exhaustGrid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_exhaustGrid.SetEditable(FALSE);
	m_exhaustGrid.SetFixedRowCount(1);
	m_exhaustGrid.SetRowHeight(0, 30);
	m_exhaustGrid.SetColumnCount(EXHAUST_GRID_COLUMN_COUNT);
	m_exhaustGrid.SetDefCellHeight(30);
	m_exhaustGrid.SetDefCellWidth(120);
	m_exhaustGrid.SetColumnWidth(0, 100);
	m_exhaustGrid.SetItemFormat(0, 0, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_exhaustGrid.SetColumnWidth(2, 250);
	m_exhaustGrid.SetColumnWidth(3, 150);
	m_exhaustGrid.SetColumnWidth(4, 150);
	m_exhaustGrid.SetColumnWidth(5, 150);
	m_exhaustGrid.SetColumnWidth(6, 150);
	m_exhaustGrid.SetColumnWidth(8, 150);
	m_exhaustGrid.SetItemFormat(0, 1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_exhaustGrid.SetFixedBkColor(RGB(0, 25, 255));
	m_exhaustGrid.SetBkColor(RGB(255, 255, 255));
	m_exhaustGrid.EnableColumnHide(TRUE);
	TCHAR temp[50];
	for (int i = 0; i < EXHAUST_GRID_COLUMN_COUNT; i++)
	{
		_tcscpy_s(temp, exhaustSearchGridItems[i]);
		m_exhaustGrid.SetItemText(0, i, temp);
	}
	m_exhaustGrid.SetFixedTextColor(RGB(255, 255, 255));
}


void CSearchDlg::setDelHistoryGrid()
{
	m_delHistoryGrid.EnableDragAndDrop(FALSE);
	m_delHistoryGrid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_delHistoryGrid.SetEditable(FALSE);
	m_delHistoryGrid.SetFixedRowCount(1);
	m_delHistoryGrid.SetRowHeight(0, 30);
	m_delHistoryGrid.SetColumnCount(HISTORY_GRID_COLUMN_COUNT);
	m_delHistoryGrid.SetDefCellHeight(30);
	m_delHistoryGrid.SetDefCellWidth(120);
	m_delHistoryGrid.SetColumnWidth(0, 100);
	m_delHistoryGrid.SetItemFormat(0, 0, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_delHistoryGrid.SetColumnWidth(2, 250);
	m_delHistoryGrid.SetColumnWidth(3, 150);
	m_delHistoryGrid.SetColumnWidth(4, 150);
	m_delHistoryGrid.SetColumnWidth(6, 150);
	m_delHistoryGrid.SetColumnWidth(7, 150);
	m_delHistoryGrid.SetColumnWidth(8, 150);
	m_delHistoryGrid.SetColumnWidth(9, 150);
	m_delHistoryGrid.SetItemFormat(0, 1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_delHistoryGrid.SetFixedBkColor(RGB(0, 25, 255));
	m_delHistoryGrid.SetBkColor(RGB(255, 255, 255));
	m_delHistoryGrid.EnableColumnHide(TRUE);
	TCHAR temp[50];
	for (int i = 0; i < HISTORY_GRID_COLUMN_COUNT; i++)
	{
		_tcscpy_s(temp, delHistorySearchGridItems[i]);
		m_delHistoryGrid.SetItemText(0, i, temp);
	}
	m_delHistoryGrid.SetFixedTextColor(RGB(255, 255, 255));
}


void CSearchDlg::setComboSearchScope()
{
	m_comboSearchScope.AddString(_T("资产资料"));
	m_comboSearchScope.AddString(_T("摆放资料"));
	m_comboSearchScope.AddString(_T("移动资料"));
	m_comboSearchScope.AddString(_T("返送资料"));
	m_comboSearchScope.AddString(_T("处置资料"));
	if(g_bUserMode)
		m_comboSearchScope.AddString(_T("删除记录资料"));
	m_comboSearchScope.SetCurSel(0);
}


void CSearchDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	CRect rect;
	if (m_inputGrid) {
		m_resizeController.resize(cx, cy);
		m_inputGrid.GetWindowRect(rect);
		ScreenToClient(rect);
		if (m_placeGrid) {
			m_placeGrid.MoveWindow(rect);
		}
		if (m_moveGrid) {
			m_moveGrid.MoveWindow(rect);
		}
		if (m_exhaustGrid) {
			m_exhaustGrid.MoveWindow(rect);
		}
		if (m_backGrid) {
			m_backGrid.MoveWindow(rect);
		}
		if (m_delHistoryGrid) {
			m_delHistoryGrid.MoveWindow(rect);
		}
	}
}


void CSearchDlg::OnCbnSelchangeComboSearchScope()
{
	int sel = m_comboSearchScope.GetCurSel();
	if (sel == 0) {
		m_inputGrid.ShowWindow(SW_SHOW);
		m_placeGrid.ShowWindow(SW_HIDE);
		m_backGrid.ShowWindow(SW_HIDE);
		m_moveGrid.ShowWindow(SW_HIDE);
		m_exhaustGrid.ShowWindow(SW_HIDE);
		m_delHistoryGrid.ShowWindow(SW_HIDE);
	}
	else if (sel == 1) {
		m_inputGrid.ShowWindow(SW_HIDE);
		m_placeGrid.ShowWindow(SW_SHOW);
		m_backGrid.ShowWindow(SW_HIDE);
		m_moveGrid.ShowWindow(SW_HIDE);
		m_exhaustGrid.ShowWindow(SW_HIDE);
		m_delHistoryGrid.ShowWindow(SW_HIDE);
	}
	else if (sel == 2) {
		m_inputGrid.ShowWindow(SW_HIDE);
		m_placeGrid.ShowWindow(SW_HIDE);
		m_backGrid.ShowWindow(SW_HIDE);
		m_moveGrid.ShowWindow(SW_SHOW);
		m_exhaustGrid.ShowWindow(SW_HIDE);
		m_delHistoryGrid.ShowWindow(SW_HIDE);
	}
	else if (sel == 3) {
		m_inputGrid.ShowWindow(SW_HIDE);
		m_placeGrid.ShowWindow(SW_HIDE);
		m_backGrid.ShowWindow(SW_SHOW);
		m_moveGrid.ShowWindow(SW_HIDE);
		m_exhaustGrid.ShowWindow(SW_HIDE);
		m_delHistoryGrid.ShowWindow(SW_HIDE);
	}
	else if (sel == 4) {
		m_inputGrid.ShowWindow(SW_HIDE);
		m_placeGrid.ShowWindow(SW_HIDE);
		m_backGrid.ShowWindow(SW_HIDE);
		m_moveGrid.ShowWindow(SW_HIDE);
		m_exhaustGrid.ShowWindow(SW_SHOW);
		m_delHistoryGrid.ShowWindow(SW_HIDE);
	}
	else if (sel == 5) {
		m_inputGrid.ShowWindow(SW_HIDE);
		m_placeGrid.ShowWindow(SW_HIDE);
		m_backGrid.ShowWindow(SW_HIDE);
		m_moveGrid.ShowWindow(SW_HIDE);
		m_exhaustGrid.ShowWindow(SW_HIDE);
		m_delHistoryGrid.ShowWindow(SW_SHOW);
	}
	OnBnClickedButtonView();
}


void CSearchDlg::OnBnClickedButtonSearch()
{
	UpdateData(TRUE);
	int sel = m_comboSearchScope.GetCurSel();
	if (sel == 0)
		searchAssetData();
	else if (sel == 1)
		searchPlaceData();
	else if (sel == 2)
		searchMoveData();
	else if (sel == 3)
		searchBackData();
	else if (sel == 4)
		searchExhaustData();
	else if (sel == 5)
		searchDeleteHistoryData();
}


void CSearchDlg::searchAssetData(bool bSearch)
{
	m_szName = m_szName.Trim();
	m_szModel = m_szModel.Trim();
	char date_from[20], date_to[20];
	sprintf_s(date_from, "%04d-%02d-%02d", m_oleDateFrom.GetYear(), m_oleDateFrom.GetMonth(), m_oleDateFrom.GetDay());
	sprintf_s(date_to, "%04d-%02d-%02d", m_oleDateTo.GetYear(), m_oleDateTo.GetMonth(), m_oleDateTo.GetDay());
	if (m_bDateSearch == false) {
		date_from[0] = 0;
		date_to[0] = 0;
	}
	char name[100], model[100];
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szName, name);
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szModel, model);
	char query[1000];
	if(bSearch)
		make_query_for_asset_search(name, model, date_from, date_to, query);
	else
		make_query_for_asset_search("", "", "", "", query);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		m_inputGrid.SetRowCount(res->row_count + 1);
		int i = 0;
		TCHAR temp[100];
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			i++;
			for (int j = 0; j < INPUT_GRID_COLUMN_COUNT; j++)
			{
				if(j != 7)
					Utf8_2_Unicode(row[j], temp);
				else {
					if (atoi(row[j]) == 1)
						_tcscpy_s(temp, payStateOptionString[0]);
					else
						_tcscpy_s(temp, payStateOptionString[1]);
				}
				m_inputGrid.SetItemText(i, j, temp);
			}
		}
		m_inputGrid.Refresh();
		mysql_free_result(res);
	}
}


void CSearchDlg::searchPlaceData(bool bSearch)
{
	m_szName = m_szName.Trim();
	m_szModel = m_szModel.Trim();
	m_szPlace = m_szPlace.Trim();
	char date_from[20], date_to[20];
	sprintf_s(date_from, "%04d-%02d-%02d", m_oleDateFrom.GetYear(), m_oleDateFrom.GetMonth(), m_oleDateFrom.GetDay());
	sprintf_s(date_to, "%04d-%02d-%02d", m_oleDateTo.GetYear(), m_oleDateTo.GetMonth(), m_oleDateTo.GetDay());
	if (m_bDateSearch == false) {
		date_from[0] = 0;
		date_to[0] = 0;
	}
	char name[100], model[100], place[100];
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szName, name);
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szModel, model);
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szPlace, place);
	char query[1000];
	if(bSearch)
		make_query_for_place_search(name, date_from, date_to, place, model, query);
	else
		make_query_for_place_search("", "", "", "", "", query);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		m_placeGrid.SetRowCount(res->row_count + 1);
		int i = 0;
		TCHAR temp[100];
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			i++;
			for (int j = 0; j < PLACE_GRID_COLUMN_COUNT; j++)
			{
				if(j != 1)
					Utf8_2_Unicode(row[j], temp);
				else
					_stprintf_s(temp, _T("%06d"), atoi(row[1]));
				m_placeGrid.SetItemText(i, j, temp);
			}
		}
		m_placeGrid.Refresh();
		mysql_free_result(res);
	}
}


void CSearchDlg::searchBackData(bool bSearch)
{
	m_szName = m_szName.Trim();
	m_szModel = m_szModel.Trim();
	m_szPlace = m_szPlace.Trim();
	char date_from[20], date_to[20];
	sprintf_s(date_from, "%04d-%02d-%02d", m_oleDateFrom.GetYear(), m_oleDateFrom.GetMonth(), m_oleDateFrom.GetDay());
	sprintf_s(date_to, "%04d-%02d-%02d", m_oleDateTo.GetYear(), m_oleDateTo.GetMonth(), m_oleDateTo.GetDay());
	if (m_bDateSearch == false) {
		date_from[0] = 0;
		date_to[0] = 0;
	}
	char name[100], model[100], place[100];
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szName, name);
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szModel, model);
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szPlace, place);
	char query[1000];
	if(bSearch)
		make_query_for_back_search(name, date_from, date_to, place, model, query);
	else
		make_query_for_back_search("", "", "", "", "", query);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		m_backGrid.SetRowCount(res->row_count + 1);
		int i = 0;
		TCHAR temp[100];
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			i++;
			for (int j = 0; j < BACK_GRID_COLUMN_COUNT; j++)
			{
				Utf8_2_Unicode(row[j], temp);
				m_backGrid.SetItemText(i, j, temp);
			}
		}
		m_backGrid.Refresh();
		mysql_free_result(res);
	}
}


void CSearchDlg::searchMoveData(bool bSearch)
{
	m_szName = m_szName.Trim();
	m_szModel = m_szModel.Trim();
	m_szPlace = m_szPlace.Trim();
	char date_from[20], date_to[20];
	sprintf_s(date_from, "%04d-%02d-%02d", m_oleDateFrom.GetYear(), m_oleDateFrom.GetMonth(), m_oleDateFrom.GetDay());
	sprintf_s(date_to, "%04d-%02d-%02d", m_oleDateTo.GetYear(), m_oleDateTo.GetMonth(), m_oleDateTo.GetDay());
	if (m_bDateSearch == false) {
		date_from[0] = 0;
		date_to[0] = 0;
	}
	char name[100], model[100], place[100];
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szName, name);
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szModel, model);
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szPlace, place);
	char query[1000];
	if(bSearch)
		make_query_for_movement_search(name, date_from, date_to, place, model, query);
	else
		make_query_for_movement_search("", "", "", "", "", query);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		m_moveGrid.SetRowCount(res->row_count + 1);
		int i = 0;
		TCHAR temp[100];
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			i++;
			for (int j = 0; j < MOVE_GRID_COLUMN_COUNT; j++)
			{
				Utf8_2_Unicode(row[j], temp);
				m_moveGrid.SetItemText(i, j, temp);
			}
		}
		m_moveGrid.Refresh();
		mysql_free_result(res);
	}
}


void CSearchDlg::searchExhaustData(bool bSearch)
{
	m_szName = m_szName.Trim();
	m_szModel = m_szModel.Trim();
	m_szPlace = m_szPlace.Trim();
	char date_from[20], date_to[20];
	sprintf_s(date_from, "%04d-%02d-%02d", m_oleDateFrom.GetYear(), m_oleDateFrom.GetMonth(), m_oleDateFrom.GetDay());
	sprintf_s(date_to, "%04d-%02d-%02d", m_oleDateTo.GetYear(), m_oleDateTo.GetMonth(), m_oleDateTo.GetDay());
	if (m_bDateSearch == false) {
		date_from[0] = 0;
		date_to[0] = 0;
	}
	char name[100], model[100], place[100];
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szName, name);
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szModel, model);
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szPlace, place);
	char query[1000];
	if(bSearch)
		make_query_for_exhaust_search(name, date_from, date_to, place, model, query);
	else
		make_query_for_exhaust_search("", "", "", "", "", query);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		m_exhaustGrid.SetRowCount(res->row_count + 1);
		int i = 0;
		TCHAR temp[100];
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			i++;
			for (int j = 0; j < EXHAUST_GRID_COLUMN_COUNT; j++)
			{
				Utf8_2_Unicode(row[j], temp);
				m_exhaustGrid.SetItemText(i, j, temp);
			}
		}
		m_exhaustGrid.Refresh();
		mysql_free_result(res);
	}
}


void CSearchDlg::searchDeleteHistoryData(bool bSearch)
{
	m_szName = m_szName.Trim();
	m_szModel = m_szModel.Trim();
	m_szPlace = m_szPlace.Trim();
	char date_from[20], date_to[20];
	sprintf_s(date_from, "%04d-%02d-%02d", m_oleDateFrom.GetYear(), m_oleDateFrom.GetMonth(), m_oleDateFrom.GetDay());
	sprintf_s(date_to, "%04d-%02d-%02d", m_oleDateTo.GetYear(), m_oleDateTo.GetMonth(), m_oleDateTo.GetDay());
	if (m_bDateSearch == false) {
		date_from[0] = 0;
		date_to[0] = 0;
	}
	char name[100], model[100], place[100];
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szName, name);
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szModel, model);
	Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szPlace, place);
	char query[1000];
	if(bSearch)
		make_query_for_delete_history_search(name, date_from, date_to, place, model, query);
	else
		make_query_for_delete_history_search("", "", "", "", "", query);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		m_delHistoryGrid.SetRowCount(res->row_count + 1);
		int i = 0;
		TCHAR temp[100];
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			i++;
			for (int j = 0; j < HISTORY_GRID_COLUMN_COUNT; j++)
			{
				Utf8_2_Unicode(row[j], temp);
				m_delHistoryGrid.SetItemText(i, j, temp);
			}
		}
		m_delHistoryGrid.Refresh();
		mysql_free_result(res);
	}
}


void CSearchDlg::make_query_for_asset_search(char* name, char* model, char* date_from, char* date_to, char* query)
{
	char q1[100], q2[100], q3[100], q4[100];
	bool bPrev = false;
	if (name[0] != 0)
	{
		sprintf_s(q1, "name like '%%%s%%'", name);
		bPrev = true;
	}
	else {
		q1[0] = 0;
	}
	if (model[0] != 0) {
		if (!bPrev)
			sprintf_s(q2, "field like '%%%s%%'", model);
		else
			sprintf_s(q2, " and field like '%%%s%%'", model);
		bPrev = true;
	}
	else {
		q2[0] = 0;
	}
	if (date_from[0] != 0) {
		if (!bPrev)
			sprintf_s(q3, "in_date >= '%s'", date_from);
		else
			sprintf_s(q3, " and in_date >= '%s'", date_from);
		bPrev = true;
	}
	else {
		q3[0] = 0;
	}
	if (date_to[0] != 0) {
		if (!bPrev)
			sprintf_s(q4, "in_date <= '%s'", date_to);
		else
			sprintf_s(q4, " and in_date <= '%s'", date_to);
		bPrev = true;
	}
	else {
		q4[0] = 0;
	}
	if(bPrev)
		sprintf(query, "select id, name, in_date, model, field, count, price, pay_state from assets_info where %s%s%s%s", q1, q2, q3, q4);
	else
		sprintf(query, "select id, name, in_date, model, field, count, price, pay_state from assets_info order by id");
}


void CSearchDlg::make_query_for_place_search(char* name, char* date_from, char* date_to, char* place, char* model, char* query)
{
	char q1[100], q2[100], q3[100], q4[100], q5[100];
	bool bPrev = false;
	if (name[0] != 0){
		sprintf_s(q1, " and b.name like '%%%s%%'", name);
		bPrev = true;
	}
	else {
		q1[0] = 0;
	}
	if (date_from[0] != 0) {
		sprintf_s(q2, " and a.date >= '%s'", date_from);
		bPrev = true;
	}
	else {
		q2[0] = 0;
	}
	if (date_to[0] != 0) {
		sprintf_s(q3, " and a.date <= '%s'", date_to);
		bPrev = true;
	}
	else {
		q3[0] = 0;
	}
	if (place[0] != 0) {
		sprintf_s(q4, " and a.place like '%%%s%%'", place);
		bPrev = true;
	}
	else {
		q4[0] = 0;
	}
	if (model[0] != 0) {
		sprintf_s(q5, " and field like '%%%s%%'", model);
		bPrev = true;
	}
	else {
		q5[0] = 0;
	}
	sprintf(query, "select b.id, a.id, b.name, a.place, b.field, b.in_date, a.date, b.model, a.count, b.price, a.remark from placement_info as a, assets_info as b  where  a.asset_id = b.id and a.count > 0 %s%s%s%s%s order by a.asset_id", q1, q2, q3, q4, q5);
}


void CSearchDlg::make_query_for_movement_search(char* name, char* date_from, char* date_to, char* place, char* model, char* query)
{
	char q1[100], q2[100], q3[100], q4[100], q5[100];
	if (name[0] != 0) {
		sprintf_s(q1, " and b.name like '%%%s%%'", name);
	}
	else {
		q1[0] = 0;
	}
	if (date_from[0] != 0) {
		sprintf_s(q2, " and c.date >= '%s'", date_from);
	}
	else {
		q2[0] = 0;
	}
	if (date_to[0] != 0) {
		sprintf_s(q3, " and c.date <= '%s'", date_to);
	}
	else {
		q3[0] = 0;
	}
	if (place[0] != 0) {
		sprintf_s(q4, " and (d.place like '%%%s%%' or e.place like '%%%s%')", place, place);
	}
	else {
		q4[0] = 0;
	}
	if (model[0] != 0) {
		sprintf_s(q5, " and b.field like '%%%s%%''", model);
	}
	else {
		q5[0] = 0;
	}
	sprintf(query, "select b.id, c.id, b.name, b.model, c.date, d.place AS old_place, e.place AS new_place, c.count from assets_info as b, movement_info AS c, \
		(SELECT a.id, a.place FROM placement_info AS a, movement_info AS b WHERE a.id = b.old_place) AS d,\
		(SELECT a.id, a.place FROM placement_info AS a, movement_info AS b WHERE a.id = b.new_place) AS e \
		WHERE c.asset_id = b.id AND c.old_place = d.id AND c.new_place = e.id %s%s%s%s%s", q1, q2, q3, q4, q5);

}


void CSearchDlg::make_query_for_back_search(char* name, char* date_from, char* date_to, char* place, char* model, char* query)
{
	char q1[100], q2[100], q3[100], q4[100], q5[100];
	if (name[0] != 0) {
		sprintf_s(q1, " and b.name like '%%%s%%'", name);
	}
	else {
		q1[0] = 0;
	}
	if (date_from[0] != 0) {
		sprintf_s(q2, " and c.date >= '%s'", date_from);
	}
	else {
		q2[0] = 0;
	}
	if (date_to[0] != 0) {
		sprintf_s(q3, " and c.date <= '%s'", date_to);
	}
	else {
		q3[0] = 0;
	}
	if (place[0] != 0) {
		sprintf_s(q4, " and d.place like '%%%s%%'", place);
	}
	else {
		q4[0] = 0;
	}
	if (model[0] != 0) {
		sprintf_s(q5, " and b.field like '%%%s%%''", model);
	}
	else {
		q5[0] = 0;
	}
	sprintf(query, "select b.id, c.id, b.name, b.model, c.date, d.place, c.count, d.remark, c.beizhu from assets_info as b, back_info AS c, \
		(SELECT a.id, a.place, a.remark FROM placement_info AS a, back_info AS b WHERE a.id = b.place) AS d\
		 WHERE c.asset_id = b.id AND c.place = d.id %s%s%s%s%s", q1, q2, q3, q4, q5);
}


void CSearchDlg::make_query_for_exhaust_search(char* name, char* date_from, char* date_to, char* place, char* model, char* query)
{
	char q1[100], q2[100], q3[100], q4[100], q5[100];
	if (name[0] != 0) {
		sprintf_s(q1, " and b.name like '%%%s%%'", name);
	}
	else {
		q1[0] = 0;
	}
	if (date_from[0] != 0) {
		sprintf_s(q2, " and c.date >= '%s'", date_from);
	}
	else {
		q2[0] = 0;
	}
	if (date_to[0] != 0) {
		sprintf_s(q3, " and c.date <= '%s'", date_to);
	}
	else {
		q3[0] = 0;
	}
	if (place[0] != 0) {
		sprintf_s(q4, " and d.place like '%%%s%%'", place);
	}
	else {
		q4[0] = 0;
	}
	if (model[0] != 0) {
		sprintf_s(q5, " and b.field like '%%%s%%''", model);
	}
	else {
		q5[0] = 0;
	}
	sprintf(query, "select b.id, c.id, b.name, b.model, c.date, d.place, c.count, d.remark, c.beizhu from assets_info as b, exhaust_info AS c, \
		(SELECT a.id, a.place, a.remark FROM placement_info AS a, exhaust_info AS b WHERE a.id = b.place) AS d\
		 WHERE c.asset_id = b.id AND c.place = d.id %s%s%s%s%s", q1, q2, q3, q4, q5);
}


void CSearchDlg::make_query_for_delete_history_search(char* name, char* date_from, char* date_to, char* model, char* user, char* query)
{
	char q1[100], q2[100], q3[100], q4[100], q5[100];
	bool bPrev = false;
	if (name[0] != 0) {
		sprintf_s(q1, "name like '%%%s%%'", name);
		bPrev = true;
	}
	else {
		q1[0] = 0;
	}
	if (date_from[0] != 0) {
		if (!bPrev)
			sprintf_s(q2, "delete_date >= '%s'", date_from);
		else
			sprintf_s(q2, " and delete_date >= '%s'", date_from);
		bPrev = true;
	}
	else {
		q2[0] = 0;
	}
	if (date_to[0] != 0) {
		if (!bPrev)
			sprintf_s(q3, "delete_date <= '%s'", date_to);
		else
			sprintf_s(q3, " and delete_date <= '%s'", date_to);
		bPrev = true;
	}
	else {
		q3[0] = 0;
	}
	if (model[0] != 0) {
		if (!bPrev)
			sprintf_s(q4, "field like '%%%s%%'", model);
		else
			sprintf_s(q4, " and field like '%%%s%%'", model);
		bPrev = true;
	}
	else {
		q4[0] = 0;
	}
	if (user[0] != 0) {
		if (!bPrev)
			sprintf_s(q5, "user like '%%%s%%'", user);
		else
			sprintf_s(q5, " and user like '%%%s%%'", user);
		bPrev = true;
	}
	else
		q5[0] = 0;
	if(bPrev)
		sprintf(query, "select asset_id, id, name, in_date, model, count, price, owner, delete_date, user from delete_history_info where %s%s%s%s%s", q1, q2, q3, q4, q5);
	else
		sprintf(query, "select asset_id, id, name, in_date, model, count, price, owner, delete_date, user from delete_history_info");
}


void CSearchDlg::addCtrlIdsToResizeController()
{
	CRect rect;
	GetClientRect(rect);
	m_resizeController.m_pParent = this;
	m_resizeController.setInitialSize(rect);
	m_resizeController.addId(IDC_STATIC_SEARCH_GROUP);
	m_resizeController.addId(IDC_STATIC_SEARCH_SCOPE);
	m_resizeController.addId(IDC_COMBO_SEARCH_SCOPE);
	m_resizeController.addId(IDC_STATIC_PRODUCT_NAME);
	m_resizeController.addId(IDC_EDIT_PRODUCT_NAME);
	m_resizeController.addId(IDC_STATIC_PLACE);
	m_resizeController.addId(IDC_EDIT_PLACE);
	m_resizeController.addId(IDC_STATIC_DATE_SCOPE);
	m_resizeController.addId(IDC_DATETIMEPICKER_FROM);
	m_resizeController.addId(IDC_STATIC_TIRE);
	m_resizeController.addId(IDC_DATETIMEPICKER_TO);
	m_resizeController.addId(IDC_CHECK_DATE);
	m_resizeController.addId(IDC_STATIC_MODEL);
	m_resizeController.addId(IDC_EDIT_MODEL);
	m_resizeController.addId(IDC_BUTTON_SEARCH);
	m_resizeController.addId(IDC_BUTTON_VIEW);
	m_resizeController.addId(IDC_STATIC_SEARCH_RESULT);
	m_resizeController.addId(IDC_INPUT_SEARCH_GRID);
	m_resizeController.addId(IDC_SEARCH_PLACE_GRID);
	m_resizeController.addId(IDC_MOVE_SEARCH_GRID);
	m_resizeController.addId(IDC_BACK_GRID);
	m_resizeController.addId(IDC_EXHAUST_GRID);
	m_resizeController.addId(IDC_DELETE_HISTORY_GRID);
}


void CSearchDlg::OnBnClickedButtonView()
{
	int sel = m_comboSearchScope.GetCurSel();
	if (sel == 0)
		searchAssetData(false);
	else if (sel == 1)
		searchPlaceData(false);
	else if (sel == 2)
		searchMoveData(false);
	else if (sel == 3)
		searchBackData(false);
	else if (sel == 4)
		searchExhaustData(false);
	else if (sel == 5)
		searchDeleteHistoryData(false);
}
