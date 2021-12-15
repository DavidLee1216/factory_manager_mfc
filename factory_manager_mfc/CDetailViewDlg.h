#pragma once

#include "GridCtrl_src/GridCtrl.h"
// CDetailViewDlg dialog

class CDetailViewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDetailViewDlg)

public:
	CDetailViewDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDetailViewDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DETAIL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CGridCtrl m_placeGrid;
	CGridCtrl m_backGrid;
	CGridCtrl m_exhaustGrid;
	int m_nAssetId;
	virtual BOOL OnInitDialog();
	void setPlaceGrid();
	void setBackGrid();
	void setExhaustGrid();
	void loadPlaceInfo();
	void loadBackInfo();
	void loadExhaustInfo();
};
