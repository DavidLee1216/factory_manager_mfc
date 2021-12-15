#pragma once

#include "GridCtrl_src/GridCtrl.h"
#include "CResizeController.h"
// CSearchDlg dialog

class CSearchDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchDlg)

public:
	CSearchDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSearchDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SEARCH_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CGridCtrl m_inputGrid;
	CGridCtrl m_placeGrid;
	CGridCtrl m_moveGrid;
	CGridCtrl m_backGrid;
	CGridCtrl m_exhaustGrid;
	CGridCtrl m_delHistoryGrid;
	virtual BOOL OnInitDialog();
	void setInputGrid();
	void setPlaceGrid();
	void setMoveGrid();
	void setBackGrid();
	void setExhaustGrid();
	void setDelHistoryGrid();
	CComboBox m_comboSearchScope;
	void setComboSearchScope();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelchangeComboSearchScope();
	afx_msg void OnBnClickedButtonSearch();
	void searchAssetData(bool bSearch=true);
	void searchPlaceData(bool bSearch = true);
	void searchBackData(bool bSearch = true);
	void searchMoveData(bool bSearch = true);
	void searchExhaustData(bool bSearch = true);
	void searchDeleteHistoryData(bool bSearch = true);
	CString m_szName;
	CString m_szPlace;
	COleDateTime m_oleDateFrom;
	COleDateTime m_oleDateTo;
	CString m_szModel;
	void make_query_for_asset_search(char* name, char* model, char* date_from, char* date_to, char* query);
	void make_query_for_place_search(char* name, char* date_from, char* date_to, char* place, char* model, char* query);
	void make_query_for_movement_search(char* name, char* date_from, char* date_to, char* place, char* model, char* query);
	void make_query_for_back_search(char* name, char* date_from, char* date_to, char* place, char* model, char* query);
	void make_query_for_exhaust_search(char* name, char* date_from, char* date_to, char* place, char* model, char* query);
	void make_query_for_delete_history_search(char* name, char* date_from, char* date_to, char* model, char* user, char* query);

	CResizeController m_resizeController;
	void addCtrlIdsToResizeController();
	afx_msg void OnBnClickedButtonView();
	BOOL m_bDateSearch;
};
