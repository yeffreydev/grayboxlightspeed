#pragma once


// MainDlg dialog

class MainDlg : public CDialog, public L_Observer
{
	DECLARE_DYNAMIC(MainDlg)

public:
	MainDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MainDlg();

// Dialog Data
	enum { IDD = IDD_MAINDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	BOOL OnInitDialog() override;

	// L_Observer
	void HandleMessage(L_Message const *msg) override;

	void OnOK() override {}
	void OnCancel() override {}

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNcDestroy();
	afx_msg void OnClose();
	afx_msg void OnBnClickedSendOrder();

	L_Account *account;
};
