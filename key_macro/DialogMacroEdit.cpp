#include "stdafx.h"
#include "key_macro.h"
#include "DialogMacroEdit.h"
#include "DialogString.h"
#include "DialogMacro.h"
#include "DialogDelay.h"
#include "DialogKeyboard.h"
#include "DialogMouse.h"
#include "DialogCS.h"
#include "virtual_key.h"
#include "key_hook.h"
#include "ini.h"
#include "etc.h"
#include "MmTimer.h"
#include "Common.h"
#include "ScopedLock.h"

CDialogMacroEdit *g_editDlg = NULL;
static CDialogMouse *_mouseDlg = NULL;
extern const char *GetItemDescription (const sMacroItem &item);

// Multimedia timer로 Macro 실행에 실시간성 확보
static class CMmtMacro: public CMmTimer {
public:
	virtual void OnTimer (UINT timerId, UINT msg) 
	{ 
		if (g_editDlg && ini_useMmTimer) {
			g_editDlg->PostMessage (WM_MM_TIMER, 0, 0);
		}
	}
} _mmtm;


// 프로그램이 실행되는 동안 대화상자 아이템에 
// 설정된 값을 저장하고 있는 변수들을 static으로 선언
int _macro_name_ID = 0;
static long _macro_start_key = VK_F2;
static long _macro_stop_key  = VK_F3;
static long _macro_options = MAKELONG(0, 1);


IMPLEMENT_DYNAMIC(CDialogMacroEdit, CDialog)

CDialogMacroEdit::CDialogMacroEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMacroEdit::IDD, pParent)
{
	g_editDlg = this;
	_macro = NULL;
}

CDialogMacroEdit::~CDialogMacroEdit()
{
	g_editDlg = NULL;
}

void CDialogMacroEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ITEMS, _listItems);
}


BEGIN_MESSAGE_MAP(CDialogMacroEdit, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDialogMacroEdit::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_ADD_KEY, &CDialogMacroEdit::OnBnClickedButtonAddKey)
	ON_BN_CLICKED(IDC_BUTTON_ADD_MOUSE, &CDialogMacroEdit::OnBnClickedButtonAddMouse)
	ON_BN_CLICKED(IDC_BUTTON_ADD_DELAY, &CDialogMacroEdit::OnBnClickedButtonAddDelay)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &CDialogMacroEdit::OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CDialogMacroEdit::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CDialogMacroEdit::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CDialogMacroEdit::OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_CHECK_START_OPTION, &CDialogMacroEdit::OnBnClickedCheckStartOption)
	ON_BN_CLICKED(IDC_CHECK_STOP_OPTION, &CDialogMacroEdit::OnBnClickedCheckStopOption)
	ON_BN_CLICKED(IDC_BUTTON_DISPLAY_MOUSE, &CDialogMacroEdit::OnBnClickedButtonDisplayMouse)
	ON_BN_CLICKED(IDC_BUTTON_ERASE_MOUSE, &CDialogMacroEdit::OnBnClickedButtonEraseMouse)
	ON_BN_CLICKED(IDC_BUTTON_ADD_STRING, &CDialogMacroEdit::OnBnClickedButtonAddString)
	ON_BN_CLICKED(IDC_BUTTON_TEST_RUN, &CDialogMacroEdit::OnBnClickedButtonTestRun)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CDialogMacroEdit::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_ADD_MACRO, &CDialogMacroEdit::OnBnClickedButtonAddMacro)
	ON_LBN_DBLCLK(IDC_LIST_ITEMS, &CDialogMacroEdit::OnLbnDblclkListItems)
	ON_BN_CLICKED(IDC_BUTTON_ADD_CS, &CDialogMacroEdit::OnBnClickedButtonAddCs)
END_MESSAGE_MAP()


BOOL CDialogMacroEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if (!_macro->name[0]) {
		SetWindowText ("매크로 호출 추가");
		sprintf (_macro->name, "Macro %d", ++_macro_name_ID);
		_macro->start_key = _macro_start_key;
		_macro->stop_key  = _macro_stop_key;
		_macro->options   = _macro_options;

		// 제일 첫줄에 아무것도 아닌 항목을 삽입해 둔다.
		// 이유는, 선택한 항목 뒤에다가 새로운 항목을 추가할 수 있기때문에,
		// 이 NONE이 제일 처음에 없으면 첫줄에 새로운 항목을 추가할 수 없다.
		_macro->_item.resize(1);
		_macro->_item[0].type = sMacroItem::NONE;
	}
	else {
		SetWindowText ("매크로 호출 편집");
	}

	char message[1024];
	sprintf (message, "%s를 눌러 현재 마우스 위치를 매크로 항목에 추가합니다.", g_vk_list[getVkIndex (ini_mouseCaptureKey)].desc);
	SetDlgItemText (IDC_STATIC_MESSAGE1, message);

	for (int i=0; i<256 && g_vk_list[i].vk != -1; ++i) {
		SendDlgItemMessage (IDC_COMBO_MACRO_START, CB_ADDSTRING, 0, (LPARAM)g_vk_list[i].desc);
		SendDlgItemMessage (IDC_COMBO_MACRO_STOP,  CB_ADDSTRING, 0, (LPARAM)g_vk_list[i].desc);
	}

	SetDlgItemText (IDC_EDIT_MACRO_NAME, _macro->name);
	SendDlgItemMessage (IDC_COMBO_MACRO_START, CB_SETCURSEL, getVkIndex (_macro->start_key & 0xFF));
	SendDlgItemMessage (IDC_COMBO_MACRO_STOP, CB_SETCURSEL,  getVkIndex (_macro->stop_key & 0xFF));

	CheckDlgButton (IDC_CHECK_START_OPTION,	  (_macro->start_key & VK_KEY_UP) ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton (IDC_CHECK_STOP_OPTION,    (_macro->stop_key  & VK_KEY_UP) ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton (IDC_CHECK_AUTO_START,     (_macro->options   & 0x2)       ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_START_STOP_KEY, (ini_options & 0x1) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_SELECTED_ITEM,  (ini_options & 0x2) ? BST_CHECKED : BST_UNCHECKED);

	SetDlgItemInt (IDC_EDIT_MACRO_REPEAT_COUNT, (int)(unsigned short)HIWORD(_macro->options));

	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow (FALSE);

	OnBnClickedCheckStartOption();
	OnBnClickedCheckStopOption();

	_listItems.SetMacroItems (&_macro->_item);

	for (unsigned int i=0; i<_macro->_item.size(); ++i) {
	   _listItems.InsertString (i, GetItemDescription (_macro->_item[i]));
	}
	_listItems.SetSel(0, TRUE);

	SetTextRecCount();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogMacroEdit::SetTextRecCount()
{
	char text[256];
	sprintf (text, "%d개 항목이 기록됨 (%g 초)", 
		_macro->_item.size()-1, _macro->ExecTime ()/1000.);

	SetDlgItemText (IDC_STATIC_REC_COUNT, text);
}

void CDialogMacroEdit::OnKeyMouseEvent (int vk, bool key)
{
	if (IsDlgButtonChecked (IDC_CHECK_START_STOP_KEY) == BST_CHECKED) {
		ReadStartStopVk();

		if (_macro_start_key == vk) {
			if (GetDlgItem(IDC_BUTTON_TEST_RUN)->IsWindowEnabled ()) {
				OnBnClickedButtonTestRun();
			}
		}
		else if (_macro_stop_key == vk) {
			if (GetDlgItem(IDC_BUTTON_STOP)->IsWindowEnabled ()) {
				OnBnClickedButtonStop ();
			}
		}
		else if (ini_mouseCaptureKey == vk) {
			if (_mouseDlg) {
				_mouseDlg->OnMouseCapture ();
			}
			else {
				int sel = _listItems.GetLastSel ();
				if (0 <= sel) sel++; // 만일 선택된 경우, 선택된 항목 뒤에다 추가하려고 1 증가
				else sel = _listItems.GetCount ();

				if (0 <= sel) {
					POINT cp;
					GetCursorPos(&cp);

					sMacroItem item;
					item.type        = sMacroItem::MOUSE;
					item.mouse.flags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
					item.mouse.x     = cp.x;
					item.mouse.y     = cp.y;
					// if (GetAsyncKeyState(VK_LBUTTON)) item.mouse.flags |= MOUSEEVENTF_LEFTDOWN;

					AddItem (sel, item);
				}
			}
		}
	}
}


LRESULT CDialogMacroEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message){
	case WM_MM_TIMER: {
		if (_macro && _macro->is_running ()) {
			DWORD t = CMmTimer::GetTime ();
			_macro->MacroStep (t - _tp);
			_tp = t;
		}
		break;
	}
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CDialogMacroEdit::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1000) {
		int index = _macro->index;
		int size = _macro->_item.size();

		if (0 <= index && index < size) {
			_listItems.SelItemRange (FALSE, 0, size);
		   _listItems.SetSel (index, TRUE);
		}
		else {
			OnBnClickedButtonStop();
			_listItems.SelItemRange (FALSE, 0, size);
		   _listItems.SetSel (0, TRUE);
		}
	}
	else if (nIDEvent == 1001) {
		if (!ini_useMmTimer) {
			if (_macro && _macro->is_running ()) {
				DWORD t = CMmTimer::GetTime ();
				_macro->MacroStep (t - _tp);
				_tp = t;
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CDialogMacroEdit::AddItem (int sel, sMacroItem &_item)
{
	_macro->_item.insert (_macro->_item.begin() + sel, _item);

	_listItems.InsertString (sel, GetItemDescription (_item));
	_listItems.SelItemRange (FALSE, 0, _listItems.GetCount ());
	_listItems.SetSel(sel, TRUE);
}

void CDialogMacroEdit::EditItem (int sel, sMacroItem &_item)
{
	_macro->_item[sel] = _item;

	_listItems.DeleteString (sel);
	_listItems.InsertString (sel, GetItemDescription (_item));
	_listItems.SelItemRange (FALSE, 0, _listItems.GetCount ());
	_listItems.SetSel(sel, TRUE);
}

int CDialogMacroEdit::LbGetAddIndex ()
{
	int sel = _listItems.GetLastSel ();
	if (0 <= sel) {
		return sel+1; // 만일 선택된 경우, 선택된 항목 뒤에다 추가하려고 1 증가
	}
	else {
		return _listItems.GetCount ();
	}
}

void CDialogMacroEdit::OnBnClickedButtonAddString()
{
	int sel = LbGetAddIndex ();
	CDialogString dlg;
	if (dlg.DoModal () == IDOK) AddItem (sel, dlg._item);
	SetTextRecCount();
	_listItems.SetFocus ();
}

void CDialogMacroEdit::OnBnClickedButtonAddKey()
{
	int sel = LbGetAddIndex ();
	CDialogKeyboard dlg;
	if (dlg.DoModal () == IDOK) AddItem (sel, dlg._item);
	SetTextRecCount();
	_listItems.SetFocus ();
}

void CDialogMacroEdit::OnBnClickedButtonAddMouse()
{
	int sel = LbGetAddIndex ();
	CDialogMouse dlg;
	_mouseDlg = &dlg;
	if (dlg.DoModal () == IDOK) AddItem (sel, dlg._item);
	_mouseDlg = NULL;
	SetTextRecCount();
	_listItems.SetFocus ();
}

void CDialogMacroEdit::OnBnClickedButtonAddDelay()
{
	int sel = LbGetAddIndex ();
	CDialogDelay dlg;
	if (dlg.DoModal () == IDOK) AddItem (sel, dlg._item);
	SetTextRecCount();
	_listItems.SetFocus ();
}

void CDialogMacroEdit::OnBnClickedButtonAddMacro()
{
	int sel = LbGetAddIndex ();
	CDialogMacro dlg;
	if (dlg.DoModal () == IDOK) AddItem (sel, dlg._item);
	SetTextRecCount();
	_listItems.SetFocus ();
}

void CDialogMacroEdit::OnBnClickedButtonAddCs()
{
	int sel = LbGetAddIndex ();
	CDialogCS dlg;
	if (dlg.DoModal () == IDOK) AddItem (sel, dlg._item);
	SetTextRecCount();
	_listItems.SetFocus ();
}

void CDialogMacroEdit::OnBnClickedButtonEdit()
{
	int sel = _listItems.GetFirstSel ();
	if (1 <= sel) { // 처음 아이템은 편집하지 못하도록!
		switch (_macro->_item[sel].type) {
		case sMacroItem::KEY: {
			CDialogKeyboard dlg;
			dlg._item = _macro->_item[sel];

			if (dlg.DoModal () == IDOK) EditItem (sel, dlg._item);
			break; }
		case sMacroItem::MOUSE: {
			CDialogMouse dlg;
			_mouseDlg = &dlg;
			dlg._item = _macro->_item[sel];

			if (dlg.DoModal () == IDOK) EditItem (sel, dlg._item);
			_mouseDlg = NULL;
			break; }
		case sMacroItem::TIME: {
			CDialogDelay dlg;
			dlg._item = _macro->_item[sel];

			if (dlg.DoModal () == IDOK) EditItem (sel, dlg._item);
			break; }
		case sMacroItem::STRING: {
			CDialogString dlg;
			dlg._item = _macro->_item[sel];

			if (dlg.DoModal () == IDOK) EditItem (sel, dlg._item);
			break; }
		case sMacroItem::MACRO: {
			CDialogMacro dlg;
			dlg._item = _macro->_item[sel];

			if (dlg.DoModal () == IDOK) EditItem (sel, dlg._item);
			break; }
		case sMacroItem::CS: {
			CDialogCS dlg;
			dlg._item = _macro->_item[sel];

			if (dlg.DoModal () == IDOK) EditItem (sel, dlg._item);
			break; }
		}
	}
	_listItems.SetFocus ();
}

void CDialogMacroEdit::OnBnClickedButtonDelete()
{
	int count = _listItems.GetSelCount();
	CArray<int,int> arr;

	arr.SetSize(count);
	_listItems.GetSelItems(count, arr.GetData()); 

	// 지울 때는 마지막 선택 항목부터 거꾸로 지운다.
	for (int i=arr.GetSize()-1; i>=0; i--) {
		int sel = arr.GetAt(i);
		if (1 <= sel) { // 처음 아이템은 지우지 못하도록!
			_listItems.DeleteString (sel);

			_macro->_item.erase (_macro->_item.begin() + sel);
		}
	}

	SetTextRecCount();
	_listItems.SetFocus ();
}

void CDialogMacroEdit::OnBnClickedButtonUp()
{
	int count = _listItems.GetSelCount();
	CArray<int,int> arr;

	arr.SetSize(count);
	_listItems.GetSelItems(count, arr.GetData()); 

	// 세 번째 아이템부터 위로 올릴 수 있다.
	if (arr.GetSize() == 0) return;
	if (arr.GetAt(0) < 2) return;

	for (int i=0; i<arr.GetSize(); i++) {
		int sel = arr.GetAt(i);

		// sel-1과 sel item을 스왑 한다.
		int ss = _listItems.GetSel (sel-1);
		_listItems.DeleteString (sel);
		_listItems.InsertString (sel-1, GetItemDescription (_macro->_item[sel]));
		_listItems.SetSel (sel-1, TRUE);
		if (ss) _listItems.SetSel (sel, TRUE);

		std::swap (_macro->_item[sel], _macro->_item[sel-1]);
	}

	_listItems.SetFocus ();
}

void CDialogMacroEdit::OnBnClickedButtonDown()
{
	int count = _listItems.GetSelCount();
	CArray<int,int> arr;

	arr.SetSize(count);
	_listItems.GetSelItems(count, arr.GetData()); 

	// 처음 아이템과 마지막 아이템은 아래로 내릴 수 없다.
	if (arr.GetSize() == 0) return;
	if (arr.GetAt(0) == 0) return;
	if (arr.GetAt(arr.GetSize()-1) >= _listItems.GetCount ()-1) return;

	for (int i=arr.GetSize()-1; i>=0; i--) {
		int sel = arr.GetAt(i);

		// sel+1과 sel item을 스왑 한다.
		int ss = _listItems.GetSel (sel+1);
	   _listItems.DeleteString (sel);
	   _listItems.InsertString (sel+1, GetItemDescription (_macro->_item[sel]));
	   _listItems.SetSel (sel+1, TRUE);
		if (ss) _listItems.SetSel (sel, TRUE);

	   std::swap (_macro->_item[sel], _macro->_item[sel+1]);
	}

	_listItems.SetFocus ();
}

void CDialogMacroEdit::OnLbnDblclkListItems()
{
	OnBnClickedButtonEdit();
}

void CDialogMacroEdit::OnBnClickedCheckStartOption()
{
	if (IsDlgButtonChecked (IDC_CHECK_START_OPTION) == BST_CHECKED) {
		SetDlgItemText (IDC_CHECK_START_OPTION, "누를 때");
		_macro->start_key &= VK_KEY_MASK;
	}
	else {
		SetDlgItemText (IDC_CHECK_START_OPTION, "뗄 때");
		_macro->start_key |= VK_KEY_UP;
	}

	_listItems.SetFocus ();
}

void CDialogMacroEdit::OnBnClickedCheckStopOption()
{
	if (IsDlgButtonChecked (IDC_CHECK_STOP_OPTION) == BST_CHECKED) {
		SetDlgItemText (IDC_CHECK_STOP_OPTION, "누를 때");
		_macro->stop_key &= VK_KEY_MASK;
	}
	else {
		SetDlgItemText (IDC_CHECK_STOP_OPTION, "뗄 때");
		_macro->stop_key |= VK_KEY_UP;
	}

	_listItems.SetFocus ();
}

void CDialogMacroEdit::ReadStartStopVk()
{
	int index = SendDlgItemMessage (IDC_COMBO_MACRO_START, CB_GETCURSEL, 0, 0);
	if (index != -1) {
		_macro_start_key = g_vk_list[index].vk;
	}
	if (IsDlgButtonChecked (IDC_CHECK_START_OPTION) == BST_UNCHECKED) {
		_macro_start_key |= VK_KEY_UP;
	}

	index = SendDlgItemMessage (IDC_COMBO_MACRO_STOP, CB_GETCURSEL, 0, 0);
	if (index != -1) {
		_macro_stop_key = g_vk_list[index].vk;
	}
	if (IsDlgButtonChecked (IDC_CHECK_STOP_OPTION) == BST_UNCHECKED) {
		_macro_stop_key |= VK_KEY_UP;
	}
}

void CDialogMacroEdit::OnBnClickedOk()
{
	GetDlgItemText (IDC_EDIT_MACRO_NAME, _macro->name, 255);
	_macro->name[255] = 0;

	int repeat_count = GetDlgItemInt (IDC_EDIT_MACRO_REPEAT_COUNT);
	repeat_count = BOUND(repeat_count, 0, 65535);

	_macro->options = MAKELONG(0, repeat_count);

	int index = SendDlgItemMessage (IDC_COMBO_MACRO_START, CB_GETCURSEL, 0, 0);
	if (index != -1) {
		_macro->start_key = g_vk_list[index].vk;
	}
	if (IsDlgButtonChecked (IDC_CHECK_START_OPTION) == BST_UNCHECKED) {
		_macro->start_key |= VK_KEY_UP;
	}

	index = SendDlgItemMessage (IDC_COMBO_MACRO_STOP, CB_GETCURSEL, 0, 0);
	if (index != -1) {
		_macro->stop_key = g_vk_list[index].vk;
	}
	if (IsDlgButtonChecked (IDC_CHECK_STOP_OPTION) == BST_UNCHECKED) {
		_macro->stop_key |= VK_KEY_UP;
	}

	if (IsDlgButtonChecked (IDC_CHECK_AUTO_START) == BST_CHECKED) {
		_macro->options |= 0x2;
	}
	
	if (IsDlgButtonChecked (IDC_CHECK_START_STOP_KEY) == BST_CHECKED) {
		ini_options |= 0x1;
	}
	else {
		ini_options &= ~0x1;
	}
	if (IsDlgButtonChecked (IDC_CHECK_SELECTED_ITEM) == BST_CHECKED) {
		ini_options |= 0x2;
	}
	else {
		ini_options &= ~0x2;
	}

	_macro_options   = _macro->options;
	_macro_start_key = _macro->start_key;
	_macro_stop_key  = _macro->stop_key;

	if (!_macro->name[0]) {
		AfxMessageBox ("매크로 이름이 지정되지 않았습니다. 이름을 입력하시기 바랍니다.");
	}
	else {
		OnOK();
	}
}

void CDialogMacroEdit::EnableWindowItem(BOOL enable)
{
	GetDlgItem(IDC_EDIT_MACRO_NAME)->EnableWindow (enable);
	GetDlgItem(IDC_COMBO_MACRO_START)->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_START_OPTION)->EnableWindow (enable);
	GetDlgItem (IDC_COMBO_MACRO_START) ->EnableWindow (enable);
	GetDlgItem (IDC_COMBO_MACRO_STOP)  ->EnableWindow (enable);
	GetDlgItem (IDC_CHECK_START_OPTION)->EnableWindow (enable);
	GetDlgItem (IDC_CHECK_STOP_OPTION) ->EnableWindow (enable);
	GetDlgItem (IDC_CHECK_AUTO_START) ->EnableWindow (enable);
	GetDlgItem(IDC_COMBO_MACRO_STOP)->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_STOP_OPTION)->EnableWindow (enable);
	GetDlgItem(IDC_BUTTON_ADD_STRING)->EnableWindow (enable);
	GetDlgItem(IDC_BUTTON_ADD_KEY)->EnableWindow (enable);
	GetDlgItem(IDC_BUTTON_ADD_MOUSE)->EnableWindow (enable);
	GetDlgItem(IDC_BUTTON_ADD_DELAY)->EnableWindow (enable);
	GetDlgItem(IDC_BUTTON_ADD_MACRO)->EnableWindow (enable);
	GetDlgItem(IDC_BUTTON_ADD_CS)->EnableWindow (enable);
	GetDlgItem(IDC_BUTTON_EDIT)->EnableWindow (enable);
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow (enable);
	GetDlgItem(IDC_BUTTON_UP)->EnableWindow (enable);
	GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow (enable);
	GetDlgItem(IDC_EDIT_MACRO_REPEAT_COUNT)->EnableWindow (enable);
	GetDlgItem(IDC_BUTTON_TEST_RUN)->EnableWindow (enable);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow (enable);
	GetDlgItem(IDC_BUTTON_DISPLAY_MOUSE)->EnableWindow (enable);
	GetDlgItem(IDC_BUTTON_ERASE_MOUSE)->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_START_STOP_KEY)->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_SELECTED_ITEM)->EnableWindow (enable);
	GetDlgItem(IDOK)->EnableWindow (enable);
	GetDlgItem(IDCANCEL)->EnableWindow (enable);
}

void CDialogMacroEdit::MessagePump()
{
	MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {   
        TranslateMessage(&msg);   
        DispatchMessage(&msg);   
		Sleep (0);
    }   
}

void CDialogMacroEdit::OnBnClickedButtonDisplayMouse()
{
	CDC *dc = CDC::FromHandle(::GetDC(NULL));   
	if (!dc) return;

	EnableWindowItem (FALSE);

	CPen pen_blue (PS_SOLID, 2, RGB(0,0,255));
	CPen pen_red (PS_SOLID, 2, RGB(255,0,0));
	CBrush brush_red (RGB(128,0,0));

	dc->SelectObject (&pen_red);
	dc->SelectObject (&brush_red);

	int count = _listItems.GetSelCount();
	CArray<int,int> arrayListSel;

	arrayListSel.SetSize(count);
	_listItems.GetSelItems(count, arrayListSel.GetData()); 

	int mouse_count = 0;
	for (unsigned int i=0; i<_macro->_item.size(); ++i) {
		sMacroItem &mi = _macro->_item[i];

		if (mi.type == sMacroItem::MOUSE) {
			int mx = mi.mouse.x;
			int my = mi.mouse.y;
			
			if (mi.mouse.flags&MOUSEEVENTF_SCREEN_INDEPENDENT_POS){
				ScreenDependentMousePos (mx, my);
			}

			if (!mouse_count) dc->MoveTo (mx, my);
			else              dc->LineTo (mx, my);
			mouse_count++;

			const int MOUSE_BUTTON_DOWN = MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_RIGHTDOWN|MOUSEEVENTF_MIDDLEDOWN|MOUSEEVENTF_XDOWN;
			if (mi.mouse.flags & MOUSE_BUTTON_DOWN) {
				const int r = 4;
				dc->Rectangle (mx-r, my-r, mx+r, my+r);
			}

			if (IsDlgButtonChecked (IDC_CHECK_SELECTED_ITEM) == BST_CHECKED) {
				for (int j=0; j<arrayListSel.GetSize(); j++) {
					if (arrayListSel.GetAt(j) == i) {
						dc->SelectObject (&pen_blue);
						dc->SelectObject (GetStockObject(NULL_BRUSH));

						const int r1 = 8, r2 = 12;
						dc->Rectangle (mx-r1, my-r1, mx+r1, my+r1);
						dc->Rectangle (mx-r2, my-r2, mx+r2, my+r2);

						dc->SelectObject (&pen_red);
						dc->SelectObject (&brush_red);
					}
				}
			}
		}
		MessagePump ();
	}
	ReleaseDC (dc);

	EnableWindowItem (TRUE);

	_listItems.SetFocus ();
}

void CDialogMacroEdit::OnBnClickedButtonEraseMouse()
{
	::InvalidateRect (NULL, NULL, FALSE);

	_listItems.SetFocus ();
}

void CDialogMacroEdit::OnBnClickedButtonTestRun()
{
	EnableWindowItem (FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow (TRUE);

	SetTimer (1000, 33, NULL);
	if (ini_useMmTimer) {
		_mmtm.Start (ini_eventMinDelay);
	}
	else {
		SetTimer (1001, ini_eventMinDelay, NULL);
		_tp = CMmTimer::GetTime ();
	}

	int sel = _listItems.GetLastSel ();
	_macro->start ((sel < 1) ? 1 : sel);

	_listItems.SetFocus ();
}

void CDialogMacroEdit::OnBnClickedButtonStop()
{
	_macro->stop ();
	
	KillTimer (1000);
	if (ini_useMmTimer) {
		_mmtm.Stop ();
	}
	else {
		KillTimer (1001);
	}

	EnableWindowItem (TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow (FALSE);

	_listItems.SetFocus ();
}
