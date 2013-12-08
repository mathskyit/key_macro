#pragma once
#include "macro_def.h"

class CDialogKeyboard : public CDialog
{
	DECLARE_DYNAMIC(CDialogKeyboard)

public:
	CDialogKeyboard(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogKeyboard();

	void OnKeyEvent (int vk);

	int _vk;
	sMacroItem _item;

	enum { IDD = IDD_DIALOG_ADD_KEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
