#include "CResizeController.h"

CResizeController::CResizeController()
{
	m_dblHeightRate = 1;
	m_dblWidthRate = 1;
}

CResizeController::~CResizeController()
{

}

void CResizeController::resize(double width, double height)
{
	setResizedParentSize(width, height);
	int cnt = m_arrWndIds.GetCount();
	for (int i = 0; i < cnt; i++)
	{
		CWnd* wnd = m_pParent->GetDlgItem(m_arrWndIds.GetAt(i));
		if (wnd->m_hWnd) {
			CRect rect = m_arrInitialRects.GetAt(i);
			CRect new_rect;
			new_rect.left = rect.left * m_dblWidthRate;
			new_rect.right = rect.right * m_dblWidthRate;
			new_rect.top = rect.top * m_dblHeightRate;
			new_rect.bottom = rect.bottom * m_dblHeightRate;
			wnd->MoveWindow(new_rect);
		}
	}
}

void CResizeController::setInitialSize(CRect rect)
{
	m_dblInitialWidth = rect.Width();
	m_dblInitialHeight = rect.Height();
	if (m_dblInitialWidth < 500)
		m_dblInitialWidth = 500;
	if (m_dblInitialHeight < 300)
		m_dblInitialHeight = 300;
}

void CResizeController::setResizedParentSize(double width, double height)
{
	m_dblWidthRate = width / m_dblInitialWidth;
	m_dblHeightRate = height / m_dblInitialHeight;
}

void CResizeController::addId(int ID) 
{
	m_arrWndIds.Add(ID);
	CWnd* wnd = m_pParent->GetDlgItem(ID);
	CRect rect;
	wnd->GetWindowRect(rect);
	m_pParent->ScreenToClient(rect);
	m_arrInitialRects.Add(rect);
}