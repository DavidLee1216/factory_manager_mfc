
// factory_manager_mfcDlg.h : header file
//
#include "CAssetInputDlg.h"
#include "CPlaceDlg.h"
#include "CSearchDlg.h"
#include "CUserManageDlg.h"

#pragma once


// CfactorymanagermfcDlg dialog
class CfactorymanagermfcDlg : public CDialogEx
{
// Construction
public:
	CfactorymanagermfcDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FACTORY_MANAGER_MFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CTabCtrl m_tabControl;
	CAssetInputDlg* m_pAssetInputDlg;
	CPlaceDlg* m_pPlaceDlg;
	CSearchDlg* m_pSearchDlg;
	CUserManageDlg* m_pUserManageDlg;

	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedClose();
};
