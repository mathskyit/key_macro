#pragma once
#include "macro_def.h"
#include "afxwin.h"

class CDialogDelay : public CDialog
{
	DECLARE_DYNAMIC(CDialogDelay)

public:
	CDialogDelay(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_DIALOG_ADD_DELAY };

	sMacroItem _item;

private:
	CComboBox _comboRandom;

	inline double GetDlgItemDouble (int nIDDlgItem)
	{
		char text[256+1];

		GetDlgItemText (nIDDlgItem, text, 256);
		text[256] = 0;

		return strtod (text, NULL);
	}

	inline void SetDlgItemDouble (int nIDDlgItem, double value)
	{
		char text[256+1];

		sprintf (text, "%0.3f", value);
		text[256] = 0;

		SetDlgItemText (nIDDlgItem, text);
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadioDelay();
	afx_msg void OnBnClickedRadioTime();
};
