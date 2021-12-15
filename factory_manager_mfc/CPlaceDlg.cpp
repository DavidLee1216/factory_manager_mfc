// CPlaceDlg.cpp : implementation file
//

#include "pch.h"
#include "factory_manager_mfc.h"
#include "CPlaceDlg.h"
#include "afxdialogex.h"
#include <mysql.h>

#define PLACE_GRID_COLUMN_COUNT 11
#define BACK_GRID_COLUMN_COUNT 7
#define EXHAUST_GRID_COLUMN_COUNT 7

TCHAR placeGridItems[PLACE_GRID_COLUMN_COUNT][30] = { _T("资产No"), _T("摆放No"), _T("品名") , _T("摆放工程") , _T("入库日期") , _T("摆放日期") , _T("规格/型号") , _T("归属部门"), _T("数量") , _T("单价") , _T("备注") };
TCHAR backGridItems[BACK_GRID_COLUMN_COUNT][30] = { _T("No"), _T("place_No"), _T("back_No") , _T("返送工程") , _T("返送日期") , _T("数量"), _T("备注") };
TCHAR exhaustGridItems[EXHAUST_GRID_COLUMN_COUNT][30] = { _T("No"), _T("place_No"), _T("exhaust_No") , _T("处置工程") , _T("处置日期") , _T("数量") , _T("备注") };

extern MYSQL* mysql;

IMPLEMENT_DYNAMIC(CPlaceDlg, CDialogEx)

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

CPlaceDlg::CPlaceDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PLACE_DIALOG, pParent)
	, m_nSearchId(0)
	, m_szPlace(_T(""))
	, m_olePlaceDate(COleDateTime::GetCurrentTime())
	, m_nModifyCount(0)
	, m_szBeizhu(_T(""))
	, m_nBackCount(0)
	, m_oleBackDate(COleDateTime::GetCurrentTime())
	, m_nExhaustCount(0)
	, m_oleExhaustDate(COleDateTime::GetCurrentTime())
	, m_szBackBeizhu(_T(""))
	, m_szExhaustBeizhu(_T(""))
{

}

CPlaceDlg::~CPlaceDlg()
{
}

void CPlaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLACE_GRID, m_place_grid);
	DDX_Control(pDX, IDC_BACK_GRID, m_back_grid);
	DDX_Control(pDX, IDC_EXHAUST_GRID, m_exhaust_grid);
	DDX_Text(pDX, IDC_EDIT_NO, m_nSearchId);
	DDX_Text(pDX, IDC_EDIT2, m_szPlace);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_olePlaceDate);
	DDX_Text(pDX, IDC_EDIT_MODIFY_MOVE_AMOUNT, m_nModifyCount);
	DDX_Text(pDX, IDC_EDIT_BEIZHU, m_szBeizhu);
	DDX_Text(pDX, IDC_EDIT_BACK_AMOUNT, m_nBackCount);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_BACK, m_oleBackDate);
	DDX_Text(pDX, IDC_EDIT_EXHAUST_AMOUNT, m_nExhaustCount);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_EXHAUST, m_oleExhaustDate);
	DDX_Text(pDX, IDC_EDIT_BACK_BEIZHU, m_szBackBeizhu);
	DDX_Text(pDX, IDC_EDIT_EXHAUST_BEIZHU, m_szExhaustBeizhu);
}


BEGIN_MESSAGE_MAP(CPlaceDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPlaceDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SIMPLE_SEARCH, &CPlaceDlg::OnBnClickedButtonSimpleSearch)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CPlaceDlg::OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CPlaceDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_MOVE, &CPlaceDlg::OnBnClickedButtonMove)
	ON_BN_CLICKED(IDC_BUTTON_BACK, &CPlaceDlg::OnBnClickedButtonBack)
	ON_BN_CLICKED(IDC_BUTTON_BACK_CANCEL, &CPlaceDlg::OnBnClickedButtonBackCancel)
	ON_BN_CLICKED(IDC_BUTTON_EXHAUST, &CPlaceDlg::OnBnClickedButtonExhaust)
	ON_BN_CLICKED(IDC_BUTTON_EXHAUST_CANCEL, &CPlaceDlg::OnBnClickedButtonExhaustCancel)
	ON_NOTIFY(NM_CLICK, IDC_PLACE_GRID, OnGridSelChanged)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPlaceDlg message handlers

void CPlaceDlg::OnGridSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	CCellRange selectedRange = m_place_grid.GetSelectedCellRange();
	if (selectedRange.Count() == 0) {
		AfxMessageBox(_T("选择该变更的资料。"));
		return;
	}
	int nMinRow = selectedRange.GetMinRow();
	int placeId = CStringToInt(m_place_grid.GetItemText(nMinRow, 1));
	m_szPlace = m_place_grid.GetItemText(nMinRow, 3);
	m_nModifyCount = CStringToInt(m_place_grid.GetItemText(nMinRow, 8));
	CString date = m_place_grid.GetItemText(nMinRow, 5);
	int arr[3];
	SplitDate(date, arr);
	m_olePlaceDate.SetDate(arr[0], arr[1], arr[2]);
	m_szBeizhu = m_place_grid.GetItemText(nMinRow, 10);
	UpdateData(FALSE);
	loadBackData(placeId);
	loadExhaustData(placeId);
}

void CPlaceDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
}


BOOL CPlaceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetBackgroundColor(RGB(152, 200, 240));
	setPlaceGrid();
	setBackGrid();
	setExhaustGrid();

	addCtrlIdsToResizeController();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlaceDlg::setPlaceGrid()
{
	m_place_grid.EnableDragAndDrop(FALSE);
	m_place_grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_place_grid.SetEditable(FALSE);
	m_place_grid.SetListMode(TRUE);
	m_place_grid.SetFixedRowCount(1);
	m_place_grid.SetColumnCount(PLACE_GRID_COLUMN_COUNT);
	m_place_grid.SetRowResize(FALSE);
	m_place_grid.SetColumnResize(TRUE);
	m_place_grid.SetRowHeight(0, 30);
	m_place_grid.SetDefCellHeight(30);
	m_place_grid.SetDefCellWidth(120);
	m_place_grid.SetColumnWidth(2, 250);
	m_place_grid.SetFixedBkColor(RGB(0, 25, 255));
	m_place_grid.SetBkColor(RGB(255, 255, 255));
	m_place_grid.EnableColumnHide(TRUE);
	TCHAR temp[50];
	for (int i = 0; i < PLACE_GRID_COLUMN_COUNT; i++)
	{
		m_place_grid.SetItemText(0, i, placeGridItems[i]);
		m_place_grid.SetItemFormat(0, i, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	m_place_grid.SetFixedTextColor(RGB(255, 255, 255));
}


void CPlaceDlg::setBackGrid()
{
	m_back_grid.EnableDragAndDrop(FALSE);
	m_back_grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_back_grid.SetEditable(FALSE);
	m_back_grid.SetListMode(TRUE);
	m_back_grid.SetFixedRowCount(1);
	m_back_grid.SetRowResize(FALSE);
	m_back_grid.SetColumnResize(TRUE);
	m_back_grid.SetRowHeight(0, 30);
	m_back_grid.SetColumnCount(BACK_GRID_COLUMN_COUNT);
	m_back_grid.SetFixedBkColor(RGB(0, 25, 255));
	m_back_grid.SetDefCellHeight(30);
	m_back_grid.SetDefCellWidth(120);
	m_back_grid.SetBkColor(RGB(255, 255, 255));
	m_back_grid.EnableColumnHide(TRUE);
	for (int i = 0; i < BACK_GRID_COLUMN_COUNT; i++)
	{
		m_back_grid.SetItemFormat(0, i, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		if (i == 1 || i == 2) {
			m_back_grid.SetColumnWidth(i, 0);
		}
		m_back_grid.SetItemText(0, i, backGridItems[i]);
	}
	m_back_grid.SetFixedTextColor(RGB(255, 255, 255));
}


void CPlaceDlg::setExhaustGrid()
{
	m_exhaust_grid.EnableDragAndDrop(FALSE);
	m_exhaust_grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_exhaust_grid.SetEditable(FALSE);
	m_exhaust_grid.SetListMode(TRUE);
	m_exhaust_grid.SetFixedRowCount(1);
	m_exhaust_grid.SetRowResize(FALSE);
	m_exhaust_grid.SetColumnResize(TRUE);
	m_exhaust_grid.SetRowHeight(0, 30);
	m_exhaust_grid.SetColumnCount(EXHAUST_GRID_COLUMN_COUNT);
	m_exhaust_grid.SetFixedBkColor(RGB(0, 25, 255));
	m_exhaust_grid.SetDefCellHeight(30);
	m_exhaust_grid.SetDefCellWidth(120);
	m_exhaust_grid.SetBkColor(RGB(255, 255, 255));
	m_exhaust_grid.EnableColumnHide(TRUE);
	for (int i = 0; i < EXHAUST_GRID_COLUMN_COUNT; i++)
	{
		m_exhaust_grid.SetItemFormat(0, i, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		if (i == 1 || i == 2) {
			m_exhaust_grid.SetColumnWidth(i, 0);
		}
		m_exhaust_grid.SetItemText(0, i, exhaustGridItems[i]);
	}
	m_exhaust_grid.SetFixedTextColor(RGB(255, 255, 255));
}


void CPlaceDlg::OnBnClickedButtonSimpleSearch()
{
	UpdateData(TRUE);
	m_place_grid.DeleteNonFixedRows();
	char query[500];
	TCHAR temp[100];
	sprintf_s(query, "select b.id, a.id, b.name, a.place, b.in_date, a.date, b.model, b.field, a.count, b.price, a.remark from placement_info as a, assets_info as b where a.asset_id=b.id order by a.asset_id");
	if (m_nSearchId != 0)
		sprintf_s(query, "select b.id, a.id, b.name, a.place, b.in_date, a.date, b.model, b.field, a.count, b.price, a.remark from placement_info as a, assets_info as b where a.asset_id=b.id and b.id='%d' order by a.asset_id", m_nSearchId);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		m_place_grid.SetRowCount(res->row_count + 1);
		int i = 0;
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			i++;
			for (int j = 0; j < PLACE_GRID_COLUMN_COUNT; j++)
			{
				if(j != 1)
					Utf8_2_Unicode(row[j], temp);
				else {
					_stprintf_s(temp, _T("%06d"), atoi(row[1]));
				}
				m_place_grid.SetItemText(i, j, temp);
			}
		}
		mysql_free_result(res);
		m_place_grid.Refresh();
	}

}


void CPlaceDlg::OnBnClickedButtonModify()
{
	UpdateData(TRUE);
	if (check_modify_inputs() == false)
		return;
	CCellRange selectedRange = m_place_grid.GetSelectedCellRange();
	if (selectedRange.Count() == 0) {
		AfxMessageBox(_T("选择该变更的资料。"));
		return;
	}
	int nMinRow = selectedRange.GetMinRow();
	int placeId = CStringToInt(m_place_grid.GetItemText(nMinRow, 1));
	int assetId = CStringToInt(m_place_grid.GetItemText(nMinRow, 0));
	char date[20]; sprintf_s(date, "%04d-%02d-%02d", m_olePlaceDate.GetYear(), m_olePlaceDate.GetMonth(), m_olePlaceDate.GetDay());
	TCHAR tcDate[20]; _stprintf_s(tcDate, _T("%04d-%02d-%02d"), m_olePlaceDate.GetYear(), m_olePlaceDate.GetMonth(), m_olePlaceDate.GetDay());
	char place[100], beizhu[100];
	Unicode_2_Utf8((wchar_t*)(LPCTSTR)m_szPlace, place);
	Unicode_2_Utf8((wchar_t*)(LPCTSTR)m_szBeizhu, beizhu);
	int curCount = CStringToInt(m_place_grid.GetItemText(nMinRow, 8));
	int remain_count = get_remain_asset_count(assetId);
	if (m_nModifyCount - curCount > remain_count) {
		AfxMessageBox(_T("审查填写数量。超过资产入库数量。"));
		return;
	}
	if (MessageBox(_T("确定您要变更。"), _T("确认"), MB_YESNO) == IDYES) {
		char query[200];
		TCHAR temp[100];
		sprintf_s(query, "update placement_info set date='%s', count='%d', place='%s', remark='%s' where id='%d'", date, m_nModifyCount, place, beizhu, placeId);
		if (mysql_query(mysql, query) == 0) {
			m_place_grid.SetItemText(nMinRow, 3, m_szPlace);
			m_place_grid.SetItemInt(nMinRow, 8, m_nModifyCount);
			m_place_grid.SetItemText(nMinRow, 5, tcDate);
			m_place_grid.SetItemText(nMinRow, 10, m_szBeizhu);
			m_place_grid.Refresh();
			AfxMessageBox(_T("操作成功"));
		}
		else {
			AfxMessageBox(_T("操作失败"));
		}
	}
}


void CPlaceDlg::OnBnClickedButtonDelete()
{
	CCellRange selectedRange = m_place_grid.GetSelectedCellRange();
	if (selectedRange.Count() == 0) {
		AfxMessageBox(_T("请选择要删除的资料。"));
		return;
	}
	int nMinRow = selectedRange.GetMinRow();
	int placeId = CStringToInt(m_place_grid.GetItemText(nMinRow, 1));
	int assetId = CStringToInt(m_place_grid.GetItemText(nMinRow, 0));
	if (MessageBox(_T("确定您要删除。"), _T("确认"), MB_YESNO) == IDYES) {
		char query[100];
		sprintf_s(query, "delete from placement_info where id='%d'", placeId);
		if (mysql_query(mysql, query) == 0) {
			m_place_grid.DeleteRow(nMinRow);
			m_place_grid.Refresh();
			AfxMessageBox(_T("操作成功"));
		}
		else {
			AfxMessageBox(_T("操作失败"));
		}
	}
}


void CPlaceDlg::OnBnClickedButtonMove()
{
	if (check_modify_inputs() == false)
		return;
	CCellRange selectedRange = m_place_grid.GetSelectedCellRange();
	if (selectedRange.Count() == 0) {
		AfxMessageBox(_T("选择该变更的资料。"));
		return;
	}
	int nMinRow = selectedRange.GetMinRow();
	int placeId = CStringToInt(m_place_grid.GetItemText(nMinRow, 1));
	int assetId = CStringToInt(m_place_grid.GetItemText(nMinRow, 0));
	CString szOldPlace = m_place_grid.GetItemText(nMinRow, 3);
	char date[20]; sprintf_s(date, "%04d-%02d-%02d", m_olePlaceDate.GetYear(), m_olePlaceDate.GetMonth(), m_olePlaceDate.GetDay());
	TCHAR tcDate[20]; _stprintf_s(tcDate, _T("%04d-%02d-%02d"), m_olePlaceDate.GetYear(), m_olePlaceDate.GetMonth(), m_olePlaceDate.GetDay());
	char place[100], oldPlace[100];
	Unicode_2_Utf8((wchar_t*)(LPCTSTR)m_szPlace, place);
	Unicode_2_Utf8((wchar_t*)(LPCTSTR)szOldPlace, oldPlace);
	char remark[100];
	Unicode_2_Utf8((wchar_t*)(LPCTSTR)m_szBeizhu, remark);
	int curCount = CStringToInt(m_place_grid.GetItemText(nMinRow, 8));
	int remain_count = get_remain_asset_count(assetId);
	if (m_nModifyCount > curCount) {
		AfxMessageBox(_T("审查填写数量。超过资产入库数量。"));
		return;
	}
	if (MessageBox(_T("确定您要移动。"), _T("确认"), MB_YESNO) == IDYES) {
		char query[200];
		TCHAR temp[100];
		int new_place_id = get_last_id_from_table("placement_info");
		sprintf_s(query, "insert into placement_info (id, asset_id, place, count, date, remark) values('%d', '%d', '%s', '%d', '%s', '%s')", new_place_id, assetId, place, m_nModifyCount, date, remark);
		if (mysql_query(mysql, query) == 0) {
			int new_movement_id = get_last_id_from_table("movement_info");
			sprintf_s(query, "insert into movement_info (id, asset_id, old_place, new_place, count, date) values('%d', '%d', '%d', '%d', '%d', '%s')", new_movement_id, assetId, placeId, new_place_id, m_nModifyCount, date);
			if (mysql_query(mysql, query) == 0) {
				int row_count = m_place_grid.GetRowCount();
				m_place_grid.SetRowCount(row_count + 1);
				m_place_grid.SetItemInt(row_count, 0, assetId);
				_stprintf_s(temp, _T("%06d"), new_place_id);
				m_place_grid.SetItemText(row_count, 1, temp);
				m_place_grid.SetItemText(row_count, 2, m_place_grid.GetItemText(nMinRow, 2));
				m_place_grid.SetItemText(row_count, 3, m_szPlace);
				m_place_grid.SetItemText(row_count, 4, m_place_grid.GetItemText(nMinRow, 4));
				m_place_grid.SetItemText(row_count, 5, tcDate);
				m_place_grid.SetItemText(row_count, 6, m_place_grid.GetItemText(nMinRow, 6));
				m_place_grid.SetItemText(row_count, 7, m_place_grid.GetItemText(nMinRow, 7));
				m_place_grid.SetItemInt(row_count, 8, m_nModifyCount);
				m_place_grid.SetItemText(row_count, 9, m_place_grid.GetItemText(nMinRow, 9));
				m_place_grid.SetItemText(row_count, 10, m_szBeizhu);
			}
			else {
				AfxMessageBox(_T("操作失败"));
				return;
			}
		}
		else {
			AfxMessageBox(_T("操作失败"));
			return;
		}
		sprintf_s(query, "update placement_info set count='%d' where id='%d'", curCount-m_nModifyCount, placeId);
		if (mysql_query(mysql, query) == 0) {
			m_place_grid.SetItemInt(nMinRow, 8, curCount-m_nModifyCount);
			m_place_grid.Refresh();
			AfxMessageBox(_T("操作成功"));
		}
		else {
			AfxMessageBox(_T("操作失败"));
		}

	}
}

void CPlaceDlg::OnBnClickedButtonBack()
{
	CCellRange selectedRange = m_place_grid.GetSelectedCellRange();
	if (selectedRange.Count() == 0) {
		AfxMessageBox(_T("请选择该返送的资料。"));
		return;
	}
	int nMinRow = selectedRange.GetMinRow();
	int nPlaceCount = CStringToInt(m_place_grid.GetItemText(nMinRow, 8));
	int nAssetId = CStringToInt(m_place_grid.GetItemText(nMinRow, 0));
	int nPlaceId = CStringToInt(m_place_grid.GetItemText(nMinRow, 1));
	UpdateData(TRUE);
	char date[20]; sprintf_s(date, "%04d-%02d-%02d", m_oleBackDate.GetYear(), m_oleBackDate.GetMonth(), m_oleBackDate.GetDay());
	TCHAR tcDate[20]; _stprintf_s(tcDate, _T("%04d-%02d-%02d"), m_oleBackDate.GetYear(), m_oleBackDate.GetMonth(), m_oleBackDate.GetDay());
	char beizhu[100]; Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szBackBeizhu, beizhu);
	if (m_nBackCount == 0) {
		AfxMessageBox(_T("填写返送数量"));
		return;
	}
	else {
		if (m_nBackCount > nPlaceCount) {
			AfxMessageBox(_T("审查填写数量。超过摆放数量。"));
			return;
		}
		if (MessageBox(_T("确定您要返送。"), _T("确认"), MB_YESNO) == IDYES) {
			int backId = get_last_id_from_table("back_info");
			char query[200];
			sprintf_s(query, "insert into back_info (id, asset_id, place, count, date, beizhu) values('%d', '%d', '%d', '%d', '%s', '%s')", backId, nAssetId, nPlaceId, m_nBackCount, date, beizhu);
			if (mysql_query(mysql, query) == 0) {
				sprintf_s(query, "update placement_info set count='%d' where id='%d'", nPlaceCount - m_nBackCount, nPlaceId);
				if (mysql_query(mysql, query) == 0) {
					if (m_nBackCount == nPlaceCount) {
						m_place_grid.DeleteRow(nMinRow);
					}
					else {
						m_place_grid.SetItemInt(nMinRow, 8, nPlaceCount - m_nBackCount);
					}
					m_place_grid.Refresh();
					loadBackData(nPlaceId);
				}
			}
		}
	}
}


void CPlaceDlg::OnBnClickedButtonBackCancel()
{
	CCellRange selectedRange = m_back_grid.GetSelectedCellRange();
	if (selectedRange.Count() == 0) {
		AfxMessageBox(_T("请选择要取消的资料。"));
		return;
	}
	int nMinRow = selectedRange.GetMinRow();
	if (MessageBox(_T("确定您要取消。"), _T("确认"), MB_YESNO) == IDYES) {
		int back_id = CStringToInt(m_back_grid.GetItemText(nMinRow, 2));
		int place_id = CStringToInt(m_back_grid.GetItemText(nMinRow, 1));
		int count = CStringToInt(m_back_grid.GetItemText(nMinRow, 5));
		char query[200];
		sprintf_s(query, "update placement_info a, back_info b set a.count=a.count+%d where b.id='%d' and a.id=b.place", count, back_id);
		if (mysql_query(mysql, query) == 0) {
			sprintf_s(query, "delete from back_info where id='%d'", back_id);
			if (mysql_query(mysql, query) == 0) {
				m_back_grid.DeleteRow(nMinRow);
				m_back_grid.Refresh();
			}
		}
		int placeIdx = findInPlaceGrid(place_id);
		if (placeIdx == -1) {
			sprintf_s(query, "select b.id, a.id, b.name, a.place, b.in_date, a.date, b.model, b.field, a.count, b.price, a.remark from placement_info as a, assets_info as b where a.asset_id=b.id and a.id='%d'", place_id);
			if (mysql_query(mysql, query) == 0) {
				MYSQL_RES* res = mysql_store_result(mysql);
				if (res->row_count > 0) {
					TCHAR temp[200];
					int row_count = m_place_grid.GetRowCount();
					m_place_grid.SetRowCount(row_count + 1);
					MYSQL_ROW row = mysql_fetch_row(res);
					for (int j = 0; j < PLACE_GRID_COLUMN_COUNT; j++)
					{
						if (j != 1)
							Utf8_2_Unicode(row[j], temp);
						else
							_stprintf_s(temp, _T("%06d"), atoi(row[1]));
						m_place_grid.SetItemText(row_count, j, temp);
					}
					m_place_grid.Refresh();
				}
				mysql_free_result(res);
			}

		}
		else {
			int place_count = CStringToInt(m_place_grid.GetItemText(placeIdx, 7));
			m_place_grid.SetItemInt(placeIdx, 7, place_count + count);
			m_place_grid.Refresh();
		}
	}
}


void CPlaceDlg::OnBnClickedButtonExhaust()
{
	CCellRange selectedRange = m_place_grid.GetSelectedCellRange();
	if (selectedRange.Count() == 0) {
		AfxMessageBox(_T("请选择该处置的资料。"));
		return;
	}
	int nMinRow = selectedRange.GetMinRow();
	int nPlaceCount = CStringToInt(m_place_grid.GetItemText(nMinRow, 8));
	int nAssetId = CStringToInt(m_place_grid.GetItemText(nMinRow, 0));
	int nPlaceId = CStringToInt(m_place_grid.GetItemText(nMinRow, 1));
	UpdateData(TRUE);
	char date[20]; sprintf_s(date, "%04d-%02d-%02d", m_oleExhaustDate.GetYear(), m_oleExhaustDate.GetMonth(), m_oleExhaustDate.GetDay());
	TCHAR tcDate[20]; _stprintf_s(tcDate, _T("%04d-%02d-%02d"), m_oleExhaustDate.GetYear(), m_oleExhaustDate.GetMonth(), m_oleExhaustDate.GetDay());
	char beizhu[100]; Unicode_2_Utf8((TCHAR*)(LPCTSTR)m_szExhaustBeizhu, beizhu);
	if (m_nExhaustCount == 0) {
		AfxMessageBox(_T("填写处置数量"));
		return;
	}
	else {
		if (m_nExhaustCount > nPlaceCount) {
			AfxMessageBox(_T("审查填写数量。超过摆放数量。"));
			return;
		}
		if (MessageBox(_T("确定您要处置。"), _T("确认"), MB_YESNO) == IDYES) {
			int exhaustId = get_last_id_from_table("exhaust_info");
			char query[200];
			sprintf_s(query, "insert into exhaust_info (id, asset_id, place, count, date, beizhu) values('%d', '%d', '%d', '%d', '%s', '%s')", exhaustId, nAssetId, nPlaceId, m_nExhaustCount, date, beizhu);
			if (mysql_query(mysql, query) == 0) {
				sprintf_s(query, "update placement_info set count='%d' where id='%d'", nPlaceCount - m_nExhaustCount, nPlaceId);
				if (mysql_query(mysql, query) == 0) {
					if (m_nExhaustCount == nPlaceCount) {
						m_place_grid.DeleteRow(nMinRow);
					}
					else {
						m_place_grid.SetItemInt(nMinRow, 8, nPlaceCount - m_nExhaustCount);
					}
					m_place_grid.Refresh();
					loadExhaustData(nPlaceId);
				}
			}
		}
	}
}


void CPlaceDlg::OnBnClickedButtonExhaustCancel()
{
	CCellRange selectedRange = m_exhaust_grid.GetSelectedCellRange();
	if (selectedRange.Count() == 0) {
		AfxMessageBox(_T("请选择要取消的资料。"));
		return;
	}
	int nMinRow = selectedRange.GetMinRow();
	if (MessageBox(_T("确定您要取消。"), _T("确认"), MB_YESNO) == IDYES) {
		int exhaust_id = CStringToInt(m_exhaust_grid.GetItemText(nMinRow, 2));
		int place_id = CStringToInt(m_exhaust_grid.GetItemText(nMinRow, 1));
		int count = CStringToInt(m_exhaust_grid.GetItemText(nMinRow, 5));
		char query[200];
		sprintf_s(query, "update placement_info a, exhaust_info b set a.count=a.count+%d where b.id='%d' and a.id=b.place", count, exhaust_id);
		if (mysql_query(mysql, query) == 0) {
			sprintf_s(query, "delete from back_info where id='%d'", exhaust_id);
			if (mysql_query(mysql, query) == 0) {
				m_exhaust_grid.DeleteRow(nMinRow);
				m_exhaust_grid.Refresh();
			}
		}
		int placeIdx = findInPlaceGrid(place_id);
		if (placeIdx == -1) {
			sprintf_s(query, "select b.id, a.id, b.name, a.place, b.in_date, a.date, b.model, b.field, a.count, b.price, a.remark from placement_info as a, assets_info as b where a.asset_id=b.id and a.id='%d'", place_id);
			if (mysql_query(mysql, query) == 0) {
				MYSQL_RES* res = mysql_store_result(mysql);
				if (res->row_count > 0) {
					TCHAR temp[200];
					int row_count = m_place_grid.GetRowCount();
					m_place_grid.SetRowCount(row_count + 1);
					MYSQL_ROW row = mysql_fetch_row(res);
					for (int j = 0; j < PLACE_GRID_COLUMN_COUNT; j++)
					{
						if(j != 1)
							Utf8_2_Unicode(row[j], temp);
						else
							_stprintf_s(temp, _T("%06d"), atoi(row[1]));
						m_place_grid.SetItemText(row_count, j, temp);
					}
					m_place_grid.Refresh();
				}
				mysql_free_result(res);
			}

		}
		else {
			int place_count = CStringToInt(m_place_grid.GetItemText(placeIdx, 7));
			m_place_grid.SetItemInt(placeIdx, 7, place_count + count);
			m_place_grid.Refresh();
		}
	}
}


bool CPlaceDlg::check_modify_inputs()
{
	UpdateData(TRUE);
	if (m_szPlace == _T("")) {
		AfxMessageBox(_T("请填写变更/移动工程名。"));
		return false;
	}
	if (m_nModifyCount == 0) {
		AfxMessageBox(_T("请填写变更/移动数量。"));
		return false;
	}
	return true;
}


void CPlaceDlg::loadBackData(int placeId)
{
	char query[200];
	sprintf_s(query, "select b.id, a.place, b.date, b.count, b.beizhu from placement_info as a, back_info as b where a.id='%d' and a.id=b.place order by b.id", placeId);
	if (mysql_query(mysql, query) == 0) {
		m_back_grid.DeleteNonFixedRows();
		MYSQL_RES* res = mysql_store_result(mysql);
		int i = 0;
		TCHAR temp[100];
		m_back_grid.SetRowCount(res->row_count + 1);
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			i++;
			m_back_grid.SetItemInt(i, 0, i);
			m_back_grid.SetItemInt(i, 1, placeId);
			m_back_grid.SetItemInt(i, 2, atoi(row[0]));
			Utf8_2_Unicode(row[1], temp);
			m_back_grid.SetItemText(i, 3, temp);
			Utf8_2_Unicode(row[2], temp);
			m_back_grid.SetItemText(i, 4, temp);
			m_back_grid.SetItemInt(i, 5, atoi(row[3]));
			Utf8_2_Unicode(row[4], temp);
			m_back_grid.SetItemText(i, 6, temp);
		}
		mysql_free_result(res);
		m_back_grid.Refresh();
	}
}


int CPlaceDlg::findInPlaceGrid(int place_id)
{
	int i = 0;
	int row_count = m_place_grid.GetRowCount();
	for (i = 1; i < row_count; i++)
	{
		int id = CStringToInt(m_place_grid.GetItemText(i, 1));
		if (id == place_id)
			return i;
	}
	return -1;
}


void CPlaceDlg::loadExhaustData(int placeId)
{
	char query[200];
	sprintf_s(query, "select b.id, a.place, b.date, b.count, b.beizhu from placement_info as a, exhaust_info as b where a.id='%d' and a.id=b.place order by b.id", placeId);
	if (mysql_query(mysql, query) == 0) {
		m_exhaust_grid.DeleteNonFixedRows();
		MYSQL_RES* res = mysql_store_result(mysql);
		int i = 0;
		TCHAR temp[100];
		m_exhaust_grid.SetRowCount(res->row_count + 1);
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			i++;
			m_exhaust_grid.SetItemInt(i, 0, i);
			m_exhaust_grid.SetItemInt(i, 1, placeId);
			m_exhaust_grid.SetItemInt(i, 2, atoi(row[0]));
			Utf8_2_Unicode(row[1], temp);
			m_exhaust_grid.SetItemText(i, 3, temp);
			Utf8_2_Unicode(row[2], temp);
			m_exhaust_grid.SetItemText(i, 4, temp);
			m_exhaust_grid.SetItemInt(i, 5, atoi(row[3]));
			Utf8_2_Unicode(row[4], temp);
			m_exhaust_grid.SetItemText(i, 6, temp);
		}
		mysql_free_result(res);
		m_exhaust_grid.Refresh();
	}
}


void CPlaceDlg::addCtrlIdsToResizeController()
{
	CRect rect;
	GetClientRect(rect);
	m_resizeController.m_pParent = this;
	m_resizeController.setInitialSize(rect);
	m_resizeController.addId(IDC_STATIC_PLACE);
	m_resizeController.addId(IDC_PLACE_GRID);
	m_resizeController.addId(IDC_STATIC_BACK);
	m_resizeController.addId(IDC_BACK_GRID);
	m_resizeController.addId(IDC_STATIC_EXHAUST);
	m_resizeController.addId(IDC_EXHAUST_GRID);
	m_resizeController.addId(IDC_STATIC_SIMPLE_SEARCH);
	m_resizeController.addId(IDC_STATIC_NO);
	m_resizeController.addId(IDC_EDIT_NO);
	m_resizeController.addId(IDC_BUTTON_SIMPLE_SEARCH);
	m_resizeController.addId(IDC_STATIC_MODIFY_MOVE_DELETE);
	m_resizeController.addId(IDC_STATIC_MODIFY_MOVE_PLACE_NAME);
	m_resizeController.addId(IDC_EDIT2);
	m_resizeController.addId(IDC_STATIC_MODIFY_MOVE_DATE);
	m_resizeController.addId(IDC_DATETIMEPICKER1);
	m_resizeController.addId(IDC_STATIC_MODIFY_MOVE_AMOUNT);
	m_resizeController.addId(IDC_EDIT_MODIFY_MOVE_AMOUNT);
	m_resizeController.addId(IDC_STATIC_BEIZHU);
	m_resizeController.addId(IDC_EDIT_BEIZHU);
	m_resizeController.addId(IDC_BUTTON_MODIFY);
	m_resizeController.addId(IDC_BUTTON_DELETE);
	m_resizeController.addId(IDC_BUTTON_MOVE);
	m_resizeController.addId(IDC_STATIC_BACK_GROUP);
	m_resizeController.addId(IDC_STATIC_BACK_AMOUNT);
	m_resizeController.addId(IDC_EDIT_BACK_AMOUNT);
	m_resizeController.addId(IDC_STATIC_BACK_DATE);
	m_resizeController.addId(IDC_DATETIMEPICKER_BACK);
	m_resizeController.addId(IDC_STATIC_BACK_BEIZHU);
	m_resizeController.addId(IDC_EDIT_BACK_BEIZHU);
	m_resizeController.addId(IDC_BUTTON_BACK);
	m_resizeController.addId(IDC_BUTTON_BACK_CANCEL);
	m_resizeController.addId(IDC_STATIC_EXHAUST_GROUP);
	m_resizeController.addId(IDC_STATIC_EXHAUST_AMOUNT);
	m_resizeController.addId(IDC_EDIT_EXHAUST_AMOUNT);
	m_resizeController.addId(IDC_STATIC_EXHAUST_DATE);
	m_resizeController.addId(IDC_DATETIMEPICKER_EXHAUST);
	m_resizeController.addId(IDC_STATIC_EXHAUST_BEIZHU);
	m_resizeController.addId(IDC_EDIT_EXHAUST_BEIZHU);
	m_resizeController.addId(IDC_BUTTON_EXHAUST);
	m_resizeController.addId(IDC_BUTTON_EXHAUST_CANCEL);
}


void CPlaceDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	m_resizeController.resize(cx, cy);
}
