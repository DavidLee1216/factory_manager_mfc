#pragma once

#include "framework.h"

class CResizeController
{
public:
	CResizeController();
	~CResizeController();

	CWnd* m_pParent;
	CArray<int> m_arrWndIds;
	CArray<CRect> m_arrInitialRects;
	double m_dblInitialWidth;
	double m_dblInitialHeight;
	double m_dblWidthRate;
	double m_dblHeightRate;
	void addId(int ID);
	void resize(double width, double height);
	void setInitialSize(CRect rect);
private:
	void setResizedParentSize(double width, double height);
};

