#include "CPrinterView.h"
#include "framework.h"
#include <mysql.h>

#define TICKET_START_HORZ_POS 15
#define TICKET_START_VERT_POS 15
#define PRINT_TICKET_HORZ_SPACE 15
#define PRINT_TICKET_VERT_SPACE 15


IMPLEMENT_DYNAMIC(CPrinterView, CView)

extern MYSQL* mysql;
extern MYSQL_RES* myRes;
extern MYSQL_ROW myRow;
extern TCHAR g_szFactoryname[100];

int get_total_placed_count();
wchar_t* Utf8_2_Unicode(char* row_i, wchar_t* wszStr);

CPrinterView::CPrinterView(CWnd* pParent)
{
	m_nTicketCntPerPage = 1;
}

CPrinterView::~CPrinterView()
{

}

BEGIN_MESSAGE_MAP(CPrinterView, CWnd)
END_MESSAGE_MAP()

BOOL CPrinterView::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
	if (!CWnd::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;
	return TRUE;
}


void CPrinterView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	OnPrintDraw(pDC, pInfo);

	CView::OnPrint(pDC, pInfo);
}


BOOL CPrinterView::OnPreparePrinting(CPrintInfo* pInfo)
{

	// TODO:  call DoPreparePrinting to invoke the Print dialog box

	return CView::OnPreparePrinting(pInfo);
}


void CPrinterView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	pInfo->m_rectDraw.left += 100;
	pInfo->m_rectDraw.right -= 100;
	pInfo->m_rectDraw.top += 100;
	pInfo->m_rectDraw.bottom -= 100;
	pInfo->m_rectDraw.NormalizeRect();
	m_pPrintDC->DPtoLP(&pInfo->m_rectDraw);

	CView::OnPrepareDC(pDC, pInfo);
}


void CPrinterView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CView::OnEndPrinting(pDC, pInfo);
}


void CPrinterView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	m_arrTickets.RemoveAll();
	char query[200];
	strcpy_s(query, "select a.id, b.name, a.place, a.count from assets_info as b, placement_info as a where a.asset_id=b.id and a.count>0 order by a.id");
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		while (MYSQL_ROW row = mysql_fetch_row(res))
		{
			int cnt = atoi(row[3]);
			for (int i = 0; i < cnt; i++)
			{
				Ticket ticket;
				Utf8_2_Unicode(row[1], ticket.name);
				Utf8_2_Unicode(row[2], ticket.place);
				ticket.id = atoi(row[0]);
				m_arrTickets.Add(ticket);
			}
		}
		mysql_free_result(res);
	}

	CView::OnBeginPrinting(pDC, pInfo);
}


CDC* CPrinterView::CreatePrintDC()
{
	if (m_pPD)
	{
		CDC* pDC = new CDC();
		pDC->Attach(m_pPD->hDC);
		return pDC;
	}

	return NULL;
}

int CPrinterView::OnPrintCalcPage(CDC* pDC, CPrintInfo* pInfo)
{
	CRect rect;
	rect.left = PRINT_TICKET_HORZ_SPACE; rect.right = PRINT_TICKET_VERT_SPACE; rect.top = TICKET_START_HORZ_POS; rect.bottom = TICKET_START_VERT_POS;
	pDC->LPtoDP(rect);
	int ticket_start_horz_pos = rect.top;
	int ticket_start_vert_pos = rect.bottom;
	int ticket_print_ticket_horz_space = rect.left;
	int ticket_print_ticket_vert_space = rect.right;
	int horz = pDC->GetDeviceCaps(HORZRES);
	int vert = pDC->GetDeviceCaps(VERTRES);
	int horzsize = pDC->GetDeviceCaps(HORZSIZE);
	int vertsize = pDC->GetDeviceCaps(VERTSIZE);

	int total_place_count = m_arrTickets.GetCount();// get_total_placed_count();

	int ticket_width = 60 * 10;//(70mm)
	int ticket_height = 48 * 10;
	int ticket_cnt_in_a_line = (horzsize*10 - abs(ticket_start_horz_pos) * 2 - ticket_width) / (ticket_width + abs(ticket_print_ticket_horz_space)) + 1;
	int ticket_line_in_a_page = (vertsize*10 - abs(ticket_start_vert_pos)*2 - ticket_height) / (ticket_height + abs(ticket_print_ticket_vert_space)) + 1;
	int ticket_cnt_in_a_page = ticket_cnt_in_a_line * ticket_line_in_a_page;
	m_nTicketCntPerPage = ticket_cnt_in_a_page;
	m_nTicketLinePerPage = ticket_line_in_a_page;
	m_nTicketCntPerLine = ticket_cnt_in_a_line;
	int extra_page_cnt = (total_place_count % ticket_cnt_in_a_page == 0) ? 0 : 1;
	int page = total_place_count / ticket_cnt_in_a_page + extra_page_cnt;
	return page;
}

void CPrinterView::OnFilePrint()
{
	CPrintDialog printDlg(FALSE);
	if (printDlg.DoModal() == IDCANCEL) return;

	m_pPD = &printDlg.m_pd;

	m_pPrintDC = CreatePrintDC();
	m_pPrintDC->m_bPrinting = TRUE;
// 	m_pPrintDC->SetMapMode(MM_TEXT);
	m_pPrintDC->SetMapMode(MM_LOMETRIC);
	// 	m_pPrintDC->SetMapMode(MM_HIMETRIC);
	// 	m_pPrintDC->SetMapMode(MM_TWIPS);

	CString strTitle;
	strTitle.LoadString(AFX_IDS_APP_TITLE);
	DOCINFO di;
	::ZeroMemory(&di, sizeof(DOCINFO));
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = strTitle;

	BOOL bPrintingOK = m_pPrintDC->StartDoc(&di);

	CPrintInfo Info;
	Info.m_rectDraw.SetRect(0, 0, m_pPrintDC->GetDeviceCaps(HORZRES),
		m_pPrintDC->GetDeviceCaps(VERTRES));
	// 	dc.DPtoLP(&Info.m_rectDraw);
	OnPreparePrinting(&Info);

	OnBeginPrinting(m_pPrintDC, &Info);
	int nPage = OnPrintCalcPage(m_pPrintDC, &Info);
	Info.SetMaxPage(nPage);
	int maxpage = Info.GetMaxPage();
	for (UINT page = Info.GetMinPage();
		page <= Info.GetMaxPage() && bPrintingOK; page++)
	{
		m_pPrintDC->StartPage();
		Info.m_nCurPage = page;

		OnPrint(m_pPrintDC, &Info);

		bPrintingOK = (m_pPrintDC->EndPage() > 0);
	}

	OnEndPrinting(m_pPrintDC, &Info);

	if (bPrintingOK) m_pPrintDC->EndDoc();
	else m_pPrintDC->AbortDoc();

	m_pPrintDC->Detach();
	AfxMessageBox(_T("导出成功"));
}

void CPrinterView::OnPrintDraw(CDC* pDC, CPrintInfo* pInfo)
{
	CRect rect;
	GetClientRect(rect);
	int horz = pDC->GetDeviceCaps(HORZRES);
	int vert = pDC->GetDeviceCaps(VERTRES);
	int horzsize = pDC->GetDeviceCaps(HORZSIZE);
	int vertsize = pDC->GetDeviceCaps(VERTSIZE);

	LPtoDP(pDC->m_hDC, (LPPOINT)&rect, 2);
	pDC->SetBkMode(TRANSPARENT);
	COLORREF Clr = RGB(255, 255, 255);
	CBrush brush(Clr);
	pDC->FillRect(rect, &brush);
	DPtoLP(pDC->m_hDC, (LPPOINT)&rect, 2);
	int ticket_width = 60 * 10;//50mm
	int ticket_height = 48 * 10;//40mm
	CRect rect_temp;
	rect_temp.left = 0; rect_temp.top = ticket_height;
	rect_temp.right = ticket_width; rect_temp.bottom = 0;
	DPtoLP(pDC->m_hDC, (LPPOINT)&rect_temp, 2);
	ticket_width = rect_temp.Width();
	ticket_height = rect_temp.Height();
	rect_temp.left = 0; rect_temp.top = vertsize * 10; rect_temp.right = horzsize * 10; rect_temp.bottom = 0;//pixel
	DPtoLP(pDC->m_hDC, (LPPOINT)&rect_temp, 2);
	int page_width = rect_temp.Width();
	int page_height = rect_temp.Height();
	for (int i = 0; i < m_nTicketCntPerPage && ((pInfo->m_nCurPage - 1) * m_nTicketCntPerPage + i) < m_arrTickets.GetCount(); i++)
	{
		int ticket_id = (pInfo->m_nCurPage - 1) * m_nTicketCntPerPage + i;
		int left = TICKET_START_HORZ_POS + (ticket_width + PRINT_TICKET_HORZ_SPACE) * (i % m_nTicketCntPerLine);
		int top = TICKET_START_VERT_POS + (ticket_height + PRINT_TICKET_VERT_SPACE) * (i / m_nTicketCntPerLine);
		m_arrTickets[ticket_id].OnPrintDraw(pDC, pInfo, left, top, ticket_width, ticket_height);
	}
}



void CPrinterView::OnDraw(CDC* /*pDC*/)
{
	// TODO: Add your specialized code here and/or call the base class
}


Ticket::Ticket()
{
	name[0] = 0;
	id = 0;
}

Ticket::~Ticket()
{
}

void Ticket::OnPrintDraw(CDC* pDC, CPrintInfo* pInfo, int left, int top, int width, int height)
{
	COLORREF Clr = RGB(255, 255, 255);
	COLORREF TextClr = RGB(50, 50, 50);// ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	CBrush brush(Clr);
	CRect rect;
	CRect rect1;
	rect1.left = left; rect1.right = rect1.left + width; rect1.top = top; rect1.bottom = rect1.top + height;
// 	LPtoDP(pDC->m_hDC, (LPPOINT)&rect1, 2);
// 	pDC->FillRect(rect1, &brush);
	HPEN pen1 = CreatePen(PS_SOLID, 10, RGB(50, 50, 50));
	HPEN pen2 = CreatePen(PS_SOLID, 1, RGB(50, 50, 50));
	LOGFONT lf; CFont font1, font2, font3;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 60; lf.lfWeight = FW_BOLD; lf.lfWidth = 0;
	font1.CreateFontIndirect(&lf);
	lf.lfHeight = 40; lf.lfWeight = FW_THIN; lf.lfWidth = 0;
	font2.CreateFontIndirect(&lf);
	lf.lfHeight = 50; lf.lfWeight = FW_THIN; lf.lfWidth = 0;
	font3.CreateFontIndirect(&lf);
	CFont* oldfont = pDC->SelectObject(&font1);
	CBrush* oldBrush = pDC->SelectObject(&brush);
	SelectObject(pDC->m_hDC, pen1);
	rect = rect1;
	pDC->LPtoDP(rect);
	pDC->Rectangle(rect);
	rect1.bottom = rect1.top + height / 6;
	rect = rect1;
	pDC->LPtoDP(rect);
	pDC->SelectObject(&font1);
	pDC->DrawText(_T("资产管理表"), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	SelectObject(pDC->m_hDC, pen2);
	rect1.top = top + height / 6; rect1.bottom = rect1.top + height * 4 / 6;
	rect1.left = left + 10; rect1.right = left + width - 10;
	rect = rect1;
	pDC->LPtoDP(rect);
	pDC->Rectangle(rect);
	rect1.top = top + height * 2 / 6; rect1.bottom = rect1.top;
	rect = rect1;
	pDC->LPtoDP(rect);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.right, rect.top);
	rect1.top = top + height * 3 / 6; rect1.bottom = rect1.top;
	rect = rect1;
	pDC->LPtoDP(rect);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.right, rect.top);
	rect1.top = top + height * 4 / 6; rect1.bottom = rect1.top;
	rect = rect1;
	pDC->LPtoDP(rect);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.right, rect.top);
	rect1.left = left + width / 4; rect1.right = rect1.left;
	rect1.top = top + height / 6; rect1.bottom = rect1.top + height * 4 / 6;
	rect = rect1;
	pDC->LPtoDP(rect);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom);
	pDC->SelectObject(font2);
	rect1.left = left + 10; rect1.right = left + width / 4;
	rect1.top = top + height / 6; rect1.bottom = top + height * 2 / 6;
	rect = rect1;
	pDC->LPtoDP(rect);
	pDC->DrawText(_T("名称"), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect1.left = left + width / 4; rect1.right = left + width - 10;
	rect1.top = top + height / 6; rect1.bottom = top + height * 2 / 6;
	rect = rect1;
	pDC->LPtoDP(rect);
	rect1 = rect;
	pDC->DrawText(name, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	rect1.left = left + 10; rect1.right = left + width / 4;
	rect1.top = top + height*2 / 6; rect1.bottom = top + height * 3 / 6;
	rect = rect1;
	pDC->LPtoDP(rect);
	pDC->DrawText(_T("No."), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect1.left = left + width / 4; rect1.right = left + width - 10;
	rect1.top = top + height*2 / 6; rect1.bottom = top + height * 3 / 6;
	rect = rect1;
	pDC->LPtoDP(rect);
	rect1 = rect;
	TCHAR temp[100];
	_stprintf_s(temp, _T("%06d"), id);
	pDC->DrawText(temp, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	rect1.left = left + 10; rect1.right = left + width / 4;
	rect1.top = top + height * 3 / 6; rect1.bottom = top + height * 4 / 6;
	rect = rect1;
	pDC->LPtoDP(rect);
	pDC->DrawText(_T("工程名"), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect1.left = left + width / 4; rect1.right = left + width - 10;
	rect1.top = top + height * 3 / 6; rect1.bottom = top + height * 4 / 6;
	rect = rect1;
	pDC->LPtoDP(rect);
	rect1 = rect;
	pDC->DrawText(place, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	rect1.left = left + 10; rect1.right = left + width / 4;
	rect1.top = top + height * 4 / 6; rect1.bottom = top + height * 5 / 6;
	rect = rect1;
	pDC->LPtoDP(rect);
	pDC->DrawText(_T("日期"), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect1.left = left + width / 4; rect1.right = left + width - 10;
	rect1.top = top + height * 4 / 6; rect1.bottom = top + height * 5 / 6;
	rect = rect1;
	pDC->LPtoDP(rect);
	rect1 = rect;
	CTime time = CTime::GetCurrentTime();
	_stprintf_s(temp, _T("  %04d 年    月    日"), time.GetYear());
	pDC->DrawText(temp, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

// 	int text_h = rect.Height();
// 	int text_w = rect.Width();
// 	rect.top = (rect1.top + rect1.Height() / 2) - text_h /2; //centering
// 	rect.bottom = (rect1.top + rect1.Height() / 2) + text_h / 2;
// 	rect.left = (rect1.left + rect1.Width() / 2) - text_w / 2;
// 	rect.right = (rect1.left + rect1.Width() / 2) + text_w / 2;
// 	pDC->DrawText(name, rect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
// 	rect1.left = left + 10; rect1.right = left + width / 4;
// 	rect1.top = top + height*4 / 6; rect1.bottom = top + height * 5 / 6;
// 	rect = rect1;
// 	pDC->LPtoDP(rect);
// 	pDC->DrawText(_T("No."), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
// 	_stprintf_s(temp, _T("%06d"), id);
// 	rect1.left = left + width / 4; rect1.right = left + width - 10;
// 	rect1.top = top + height * 5 / 6; rect1.bottom = top + height * 5 / 6;
// 	rect = rect1;
// 	pDC->LPtoDP(rect);
// 	pDC->DrawText(temp, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	_stprintf_s(temp, g_szFactoryname);
	pDC->SelectObject(font3);
	rect1.left = left + 10; rect1.right = left + width - 10;
	rect1.top = top + height * 5 / 6; rect1.bottom = top + height;
	rect = rect1;
	pDC->LPtoDP(rect);
	pDC->DrawText(temp, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	pDC->SelectObject(&oldfont);
	pDC->SelectObject(oldBrush);
	font1.DeleteObject();
	font2.DeleteObject();
	font3.DeleteObject();
	DeleteObject(pen1);
	DeleteObject(pen2);
}