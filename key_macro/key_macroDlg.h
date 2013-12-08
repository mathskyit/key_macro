#pragma once
#include "afxwin.h"

class Ckey_macroDlg : public CDialog
{
public:
	Ckey_macroDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	~Ckey_macroDlg();

	enum { IDD = IDD_KEY_MACRO_DIALOG };

	void OnKeyMouseEvent (int vk, bool key);

private:
	char _cmd_line[1024+1];
	DWORD _tp;
	
	void DlgInScreen ();
	void MacroRestart ();
	void SetTextRecCount();
	void EnableDlgItem(BOOL enable);
	void CheckAlwaysontop();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	HICON m_hIcon;

	DECLARE_MESSAGE_MAP()
public:
	CListBox _listMacros;
	CListBox _listMacros2;

	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedRadioRun();
	afx_msg void OnBnClickedRadioEdit();
	afx_msg void OnBnClickedButtonAddMacro();
	afx_msg void OnBnClickedButtonDeleteMacro();
	afx_msg void OnBnClickedButtonCopyMacro();
	afx_msg void OnBnClickedButtonEditMacro();
	afx_msg void OnBnClickedButtonMacroFile();
	afx_msg void OnBnClickedButtonRecMacro();
	afx_msg void OnBnClickedButtonUp2();
	afx_msg void OnBnClickedButtonDown2();
	afx_msg void OnLbnDblclkListMacros();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonConfig();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
