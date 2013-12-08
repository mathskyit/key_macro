#pragma once

class CDialogConfig : public CDialog
{
	DECLARE_DYNAMIC(CDialogConfig)

public:
	CDialogConfig(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_DIALOG_CONFIG };

private:
	void EnableDlgItem(BOOL enable);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
