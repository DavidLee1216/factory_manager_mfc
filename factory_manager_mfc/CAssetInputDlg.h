#pragma once

#include "GridCtrl_src/GridCtrl.h"
#include "CResizeController.h"
#include "CPrinterView.h"
// CAssetInputDlg dialog

class CAssetInputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAssetInputDlg)

public:
	CAssetInputDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAssetInputDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HOME_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
// 	afx_msg void OnPaint();
	CGridCtrl m_grid;
	virtual BOOL OnInitDialog();
private:
	void setGrid();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	BOOL m_bNoPaid;
	BOOL m_bPaid;
	afx_msg void OnBnClickedRadioYes();
	afx_msg void OnBnClickedRadioNo();
	CString m_name;
	CString m_model;
	CString m_owner;
	COleDateTime m_oleInputDate;
	int m_nAmount;
	double m_dblPrice;
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonPlace();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonExcel();
	bool check_inputs();
	void LoadInputData();

	afx_msg void OnGridSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGridDoubleClicked(NMHDR* pNMHDR, LRESULT* pResult);
	void addToDeleteHistory(int id);
	void readExcelFile(CString filePath);
	void writeExcelFile(CString filePath);
	void insertExcelItems(CString* items);
	CString m_szField;
	CResizeController m_resizeController;
	void addCtrlIdsToResizeController();
	afx_msg void OnBnClickedButtonExport();

	CPrinterView* m_pPrintView;

// 	CDC* CreatePrintDC();
// 	int OnPrintCalcPage(CDC* pDC, CPrintInfo* pInfo);
// 	void OnPrintDraw(CDC* pDC, CPrintInfo* pInfo);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButton();
	bool checkItemFormats(CString* items);
};
