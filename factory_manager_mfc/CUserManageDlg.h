#pragma once


// CUserManageDlg dialog

class CUserManageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserManageDlg)

public:
	CUserManageDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CUserManageDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETTING_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CString m_szOldPassword;
	CString m_szNewPassword1;
	CString m_szNewPassword2;
	CListBox m_userList;
	CString m_szUsername;
	CString m_szUserPass;
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonChange();
	void loadUserList();
	afx_msg void OnLbnSelchangeList1();
};
