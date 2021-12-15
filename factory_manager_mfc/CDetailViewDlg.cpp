// CDetailViewDlg.cpp : implementation file
//

#include "pch.h"
#include "CDetailViewDlg.h"
#include "resource.h"
#include "afxdialogex.h"
#include <mysql.h>

#define PLACE_DETAIL_GRID_COLUMN_COUNT 9
#define BACK_DETAIL_GRID_COLUMN_COUNT 4
#define EXHAUST_DETAIL_GRID_COLUMN_COUNT 4

TCHAR placeDetailGridItems[PLACE_DETAIL_GRID_COLUMN_COUNT][30] = { _T("资产No"), _T("摆放No"), _T("品名"), _T("摆放工程"), _T("归属部门"), _T("摆放日期"), _T("规格/型号"), _T("数量"), _T("备注")};
TCHAR backDetailGridItems[BACK_DETAIL_GRID_COLUMN_COUNT][30] = { _T("遣返No"), _T("遣返工程"), _T("遣返日期"), _T("数量") };
TCHAR exhaustDetailGridItems[EXHAUST_DETAIL_GRID_COLUMN_COUNT][30] = { _T("处置No"), _T("处置工程"), _T("处置日期"), _T("数量") };

IMPLEMENT_DYNAMIC(CDetailViewDlg, CDialogEx)

extern MYSQL* mysql;

wchar_t* Utf8_2_Unicode(char* row_i, wchar_t* wszStr);

CDetailViewDlg::CDetailViewDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DETAIL, pParent)
{
	m_nAssetId = 0;
}

CDetailViewDlg::~CDetailViewDlg()
{
}

void CDetailViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLACE_DETAIL_GRID, m_placeGrid);
	DDX_Control(pDX, IDC_BACK_DETAIL_GRID, m_backGrid);
	DDX_Control(pDX, IDC_EXHAUST_DETAIL_GRID, m_exhaustGrid);
}


BEGIN_MESSAGE_MAP(CDetailViewDlg, CDialogEx)
END_MESSAGE_MAP()


// CDetailViewDlg message handlers


BOOL CDetailViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetBackgroundColor(RGB(152, 200, 240));
	setPlaceGrid();
	setBackGrid();
	setExhaustGrid();
	loadPlaceInfo();
	loadBackInfo();
	loadExhaustInfo();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDetailViewDlg::setPlaceGrid()
{
	m_placeGrid.EnableDragAndDrop(FALSE);
	m_placeGrid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_placeGrid.SetEditable(FALSE);
	m_placeGrid.SetListMode(TRUE);
	m_placeGrid.SetRowResize(FALSE);
	m_placeGrid.SetColumnResize(TRUE);
	m_placeGrid.SetFixedRowCount(1);
	m_placeGrid.SetRowHeight(0, 30);
	m_placeGrid.SetDefCellHeight(30);
	m_placeGrid.SetDefCellWidth(120);
	m_placeGrid.SetFixedColumnCount(1);
	m_placeGrid.SetColumnCount(PLACE_DETAIL_GRID_COLUMN_COUNT);
	m_placeGrid.SetColumnWidth(2, 250);
	m_placeGrid.SetItemFormat(0, 2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_placeGrid.SetColumnWidth(3, 150);
	m_placeGrid.SetItemFormat(0, 3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_placeGrid.SetColumnWidth(4, 150);
	m_placeGrid.SetItemFormat(0, 4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_placeGrid.SetFixedBkColor(RGB(0, 25, 255));
	m_placeGrid.SetBkColor(RGB(255, 255, 255));
	for (int i = 0; i < PLACE_DETAIL_GRID_COLUMN_COUNT; i++)
	{
		m_placeGrid.SetItemText(0, i, placeDetailGridItems[i]);
		m_placeGrid.SetItemFormat(0, i, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	m_placeGrid.SetFixedTextColor(RGB(255, 255, 255));
}


void CDetailViewDlg::setBackGrid()
{
	m_backGrid.EnableDragAndDrop(FALSE);
	m_backGrid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_backGrid.SetEditable(FALSE);
	m_backGrid.SetListMode(TRUE);
	m_backGrid.SetRowResize(FALSE);
	m_backGrid.SetColumnResize(TRUE);
	m_backGrid.SetFixedRowCount(1);
	m_backGrid.SetRowHeight(0, 30);
	m_backGrid.SetDefCellHeight(30);
	m_backGrid.SetDefCellWidth(120);
	m_backGrid.SetFixedColumnCount(1);
	m_backGrid.SetColumnCount(BACK_DETAIL_GRID_COLUMN_COUNT);
	m_backGrid.SetColumnWidth(0, 120);
	m_backGrid.SetItemFormat(0, 0, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_backGrid.SetColumnWidth(1, 120);
	m_backGrid.SetItemFormat(0, 1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_backGrid.SetColumnWidth(2, 150);
	m_backGrid.SetItemFormat(0, 2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_backGrid.SetColumnWidth(3, 120);
	m_backGrid.SetItemFormat(0, 3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_backGrid.SetFixedBkColor(RGB(0, 25, 255));
	m_backGrid.SetBkColor(RGB(255, 255, 255));
	for (int i = 0; i < BACK_DETAIL_GRID_COLUMN_COUNT; i++)
	{
		m_backGrid.SetItemText(0, i, backDetailGridItems[i]);
	}
	m_backGrid.SetFixedTextColor(RGB(255, 255, 255));
}


void CDetailViewDlg::setExhaustGrid()
{
	m_exhaustGrid.EnableDragAndDrop(FALSE);
	m_exhaustGrid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_exhaustGrid.SetEditable(FALSE);
	m_exhaustGrid.SetListMode(TRUE);
	m_exhaustGrid.SetRowResize(FALSE);
	m_exhaustGrid.SetColumnResize(TRUE);
	m_exhaustGrid.SetFixedRowCount(1);
	m_exhaustGrid.SetRowHeight(0, 30);
	m_exhaustGrid.SetDefCellHeight(30);
	m_exhaustGrid.SetDefCellWidth(120);
	m_exhaustGrid.SetFixedColumnCount(1);
	m_exhaustGrid.SetColumnCount(EXHAUST_DETAIL_GRID_COLUMN_COUNT);
	m_exhaustGrid.SetColumnWidth(0, 120);
	m_exhaustGrid.SetItemFormat(0, 0, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_exhaustGrid.SetColumnWidth(1, 120);
	m_exhaustGrid.SetItemFormat(0, 1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_exhaustGrid.SetColumnWidth(2, 150);
	m_exhaustGrid.SetItemFormat(0, 2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_exhaustGrid.SetColumnWidth(3, 120);
	m_exhaustGrid.SetItemFormat(0, 3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_exhaustGrid.SetFixedBkColor(RGB(0, 25, 255));
	m_exhaustGrid.SetBkColor(RGB(255, 255, 255));
	for (int i = 0; i < EXHAUST_DETAIL_GRID_COLUMN_COUNT; i++)
	{
		m_exhaustGrid.SetItemText(0, i, exhaustDetailGridItems[i]);
	}
	m_exhaustGrid.SetFixedTextColor(RGB(255, 255, 255));
}


void CDetailViewDlg::loadPlaceInfo()
{
	if (m_nAssetId == 0)
		return;
	char query[500];
	sprintf_s(query, "select b.id, a.id, b.name, a.place, b.field, a.date, b.model, a.count, a.remark from placement_info as a, assets_info as b where b.id=%d and a.asset_id=b.id and a.count > 0 order by a.id", m_nAssetId);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		int row_count = res->row_count;
		m_placeGrid.SetRowCount(row_count+1);
		TCHAR temp[100];
		int i = 0;
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			i++;
			m_placeGrid.SetItemInt(i, 0, atoi(row[0]));
			m_placeGrid.SetItemInt(i, 1, atoi(row[1]));
			Utf8_2_Unicode(row[2], temp);
			m_placeGrid.SetItemText(i, 2, temp);
			Utf8_2_Unicode(row[3], temp);
			m_placeGrid.SetItemText(i, 3, temp);
			Utf8_2_Unicode(row[4], temp);
			m_placeGrid.SetItemText(i, 4, temp);
			Utf8_2_Unicode(row[5], temp);
			m_placeGrid.SetItemText(i, 5, temp);
			Utf8_2_Unicode(row[6], temp);
			m_placeGrid.SetItemText(i, 6, temp);
			m_placeGrid.SetItemInt(i, 7, atoi(row[7]));
			Utf8_2_Unicode(row[8], temp);
			m_placeGrid.SetItemText(i, 8, temp);
		}
		mysql_free_result(res);
	}
}


void CDetailViewDlg::loadBackInfo()
{
	if (m_nAssetId == 0)
		return;
	char query[500];
	sprintf_s(query, "select b.id, a.place, b.date, b.count from placement_info as a, back_info as b where a.asset_id='%d' and b.asset_id='%d' and a.id=b.place order by b.id", m_nAssetId, m_nAssetId);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		int row_count = res->row_count;
		m_backGrid.SetRowCount(row_count + 1);
		TCHAR temp[100];
		int i = 0;
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			i++;
			m_backGrid.SetItemInt(i, 0, atoi(row[0]));
			Utf8_2_Unicode(row[1], temp);
			m_backGrid.SetItemText(i, 1, temp);
			Utf8_2_Unicode(row[2], temp);
			m_backGrid.SetItemText(i, 2, temp);
			m_backGrid.SetItemInt(i, 3, atoi(row[3]));
		}
		mysql_free_result(res);
	}
}


void CDetailViewDlg::loadExhaustInfo()
{
	if (m_nAssetId == 0)
		return;
	char query[500];
	sprintf_s(query, "select b.id, a.place, b.date, b.count from placement_info as a, exhaust_info as b where a.asset_id='%d' and b.asset_id='%d' and a.id=b.place order by b.id", m_nAssetId, m_nAssetId);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		int row_count = res->row_count;
		m_exhaustGrid.SetRowCount(row_count + 1);
		TCHAR temp[100];
		int i = 0;
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			i++;
			m_exhaustGrid.SetItemInt(i, 0, atoi(row[0]));
			Utf8_2_Unicode(row[1], temp);
			m_exhaustGrid.SetItemText(i, 1, temp);
			Utf8_2_Unicode(row[2], temp);
			m_exhaustGrid.SetItemText(i, 2, temp);
			m_exhaustGrid.SetItemInt(i, 3, atoi(row[3]));
		}
		mysql_free_result(res);
	}
}
