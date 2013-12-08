#pragma once
#include "macro_def.h"
#include "ListBoxEx.h"

class CDialogMacroEdit : public CDialog
{
	DECLARE_DYNAMIC(CDialogMacroEdit)

public:
	CDialogMacroEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogMacroEdit();

	void OnKeyMouseEvent (int vk, bool key);

	sMacro *_macro;
	CListBoxEx _listItems;

	enum { IDD = IDD_DIALOG_MACRO_EDIT };

private:
	DWORD _tp;
	void SetTextRecCount();

	int LbGetAddIndex ();
	void AddItem (int sel, sMacroItem &_item);
	void EditItem (int sel, sMacroItem &_item);
	void EnableWindowItem(BOOL enable);
	void ReadStartStopVk();

	void MessagePump();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonAddKey();
	afx_msg void OnBnClickedButtonAddMouse();
	afx_msg void OnBnClickedButtonAddDelay();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnBnClickedCheckStartOption();
	afx_msg void OnBnClickedCheckStopOption();
	afx_msg void OnBnClickedButtonDisplayMouse();
	afx_msg void OnBnClickedButtonEraseMouse();
	afx_msg void OnBnClickedButtonAddString();
	afx_msg void OnBnClickedButtonTestRun();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonAddMacro();
	afx_msg void OnLbnDblclkListItems();
	afx_msg void OnBnClickedButtonAddCs();
};
