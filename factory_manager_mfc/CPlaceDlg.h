#pragma once

#include "GridCtrl_src/GridCtrl.h"
#include "CResizeController.h"
// CPlaceDlg dialog

class CPlaceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPlaceDlg)

public:
	CPlaceDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPlaceDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLACE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CGridCtrl m_place_grid;
	CGridCtrl m_back_grid;
	CGridCtrl m_exhaust_grid;
	virtual BOOL OnInitDialog();
	void setPlaceGrid();
	void setBackGrid();
	void setExhaustGrid();
	afx_msg void OnBnClickedButtonSimpleSearch();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonMove();
	afx_msg void OnBnClickedButtonBack();
	afx_msg void OnBnClickedButtonBackCancel();
	afx_msg void OnBnClickedButtonExhaust();
	afx_msg void OnBnClickedButtonExhaustCancel();
	afx_msg void OnGridSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	int m_nSearchId;
	bool check_modify_inputs();
	CString m_szPlace;
	COleDateTime m_olePlaceDate;
	int m_nModifyCount;
	CString m_szBeizhu;
	int m_nBackCount;
	COleDateTime m_oleBackDate;
	int m_nExhaustCount;
	COleDateTime m_oleExhaustDate;
	void loadBackData(int placeId);
	int findInPlaceGrid(int place_id);
	void loadExhaustData(int placeId);

	CResizeController m_resizeController;
	void addCtrlIdsToResizeController();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CString m_szBackBeizhu;
	CString m_szExhaustBeizhu;
};
