#pragma once
#include <afxwin.h>

class Ticket
{
public:
    Ticket();
	~Ticket();

    void OnPrintDraw(CDC* pDC, CPrintInfo* pInfo, int left, int top, int width, int heigth);

    TCHAR name[100];
    int id;
    TCHAR place[100];
private:

};

class CPrinterView :
    public CView
{
	DECLARE_DYNAMIC(CPrinterView)
public:
    CPrinterView(CWnd* pParent = nullptr);
    ~CPrinterView();
	BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
		DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);
	DECLARE_MESSAGE_MAP()
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
public:
    virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    void OnPrintDraw(CDC* pDC, CPrintInfo* pInfo);
    void OnFilePrint();
    CDC* CreatePrintDC();
	int OnPrintCalcPage(CDC* pDC, CPrintInfo* pInfo);

	CDC* m_pPrintDC;
	LPPRINTDLG m_pPD;

    CArray<Ticket> m_arrTickets;
    int m_nTicketCntPerPage;
    int m_nTicketCntPerLine;
    int m_nTicketLinePerPage;


    virtual void OnDraw(CDC* /*pDC*/);
};

