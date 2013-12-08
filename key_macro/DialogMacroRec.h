#pragma once
#include "macro_def.h"

class CDialogMacroRec : public CDialog
{
	DECLARE_DYNAMIC(CDialogMacroRec)

public:
	CDialogMacroRec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogMacroRec();

	void OnKeyboardEvent (int vk, WPARAM wParam, KBDLLHOOKSTRUCT *stat);
	void OnMouseEvent (int vk, WPARAM wParam, MSLLHOOKSTRUCT *stat);

	sMacro *_macro;
	CListBox _listItems;

	enum { IDD = IDD_DIALOG_MACRO_REC };

private:
	bool _macro_rec;
	DWORD _rec_time;

	void SetTextRecCount();

	void MacroRecStateChange(bool rec);
	void MacroRecOptionChanged();

	void AddKey (long vk, long flags);
	void AddMouse (long x, long y, long flags);
	void AddTimeDelay (long delay);
	void AddTimeDelayIf ();

	void EnableWindowItem(BOOL enable);
	void UpdateLastItem();
	int MouseDistance (int px, int py, int mx, int my);
	int MouseDistance (int px, int py);
	bool LastIsSameKeyDown (int vk);
	bool LastIsSameMouseDown (long x, long y, long flags);
	long GetMouseButtonFlags (long message, MSLLHOOKSTRUCT *stat);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCheckRecStat();
	afx_msg void OnBnClickedCheckRecKey();
	afx_msg void OnBnClickedCheckRecMouse();
	afx_msg void OnBnClickedCheckRecMousePos();
	afx_msg void OnBnClickedCheckTime();
	afx_msg void OnBnClickedCheckRecMouseWheel();
	afx_msg void OnBnClickedCheckMerge();
	afx_msg void OnBnClickedCheckMousePosScrindep();
	afx_msg void OnBnClickedCheckMousePosAbsolute();
	afx_msg void OnBnClickedCheckRecButtonMove();
};
