#pragma once


// CPlacePopupDlg dialog

class CPlacePopupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPlacePopupDlg)

public:
	CPlacePopupDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPlacePopupDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLACE_POPUP_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	int m_nRemainCount;
	CString m_szPlace;
	int m_nPlaceCount;
	int m_nAssetId;
	COleDateTime m_oleDate;
	virtual BOOL OnInitDialog();
	void set_remain_count(int asset_id);
	bool check_inputs();
	CString m_szBeizhu;
};
