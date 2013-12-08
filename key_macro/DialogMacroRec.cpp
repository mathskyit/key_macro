#include "stdafx.h"
#include <math.h>
#include "key_macro.h"
#include "DialogMacroRec.h"
#include "virtual_key.h"
#include "key_hook.h"
#include "ini.h"
#include "MmTimer.h"
#include "Common.h"

extern int _macro_name_ID;
extern const char *GetItemDescription (const sMacroItem &item);
CDialogMacroRec *g_recDlg = NULL;

IMPLEMENT_DYNAMIC(CDialogMacroRec, CDialog)

CDialogMacroRec::CDialogMacroRec(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMacroRec::IDD, pParent)
{
	g_recDlg = this;
}

CDialogMacroRec::~CDialogMacroRec()
{
	g_recDlg = NULL;
}

void CDialogMacroRec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ITEMS, _listItems);
}


BEGIN_MESSAGE_MAP(CDialogMacroRec, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDialogMacroRec::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDialogMacroRec::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_REC_STAT, &CDialogMacroRec::OnBnClickedCheckRecStat)
	ON_BN_CLICKED(IDC_CHECK_REC_KEY, &CDialogMacroRec::OnBnClickedCheckRecKey)
	ON_BN_CLICKED(IDC_CHECK_REC_MOUSE, &CDialogMacroRec::OnBnClickedCheckRecMouse)
	ON_BN_CLICKED(IDC_CHECK_REC_MOUSE_POS, &CDialogMacroRec::OnBnClickedCheckRecMousePos)
	ON_BN_CLICKED(IDC_CHECK_REC_MOUSE_WHEEL, &CDialogMacroRec::OnBnClickedCheckRecMouseWheel)
	ON_BN_CLICKED(IDC_CHECK_TIME, &CDialogMacroRec::OnBnClickedCheckTime)
	ON_BN_CLICKED(IDC_CHECK_MERGE, &CDialogMacroRec::OnBnClickedCheckMerge)
	ON_BN_CLICKED(IDC_CHECK_MOUSE_POS_SCRINDEP, &CDialogMacroRec::OnBnClickedCheckMousePosScrindep)
	ON_BN_CLICKED(IDC_CHECK_MOUSE_POS_ABSOLUTE, &CDialogMacroRec::OnBnClickedCheckMousePosAbsolute)
	ON_BN_CLICKED(IDC_CHECK_REC_BUTTON_MOVE, &CDialogMacroRec::OnBnClickedCheckRecButtonMove)
END_MESSAGE_MAP()

BOOL CDialogMacroRec::OnInitDialog()
{
	CDialog::OnInitDialog();

	char message[256];
	sprintf (message, "%s로 매크로 기록을 시작하거나 중지합니다.", g_vk_list[getVkIndex (ini_recordStartStopKey)].desc);
	SetDlgItemText (IDC_STATIC_MESSAGE, message);

	if (!_macro->name[0]) {
		sprintf (_macro->name, "Macro Rec. %d", ++_macro_name_ID);
		_macro->start_key = VK_F2;
		_macro->stop_key  = VK_F3;
		_macro->options   = MAKELONG(0, 1);

		// 제일 첫줄에 아무것도 아닌 항목을 삽입해 둔다.
		// 이유는, 선택한 항목 뒤에다가 새로운 항목을 추가할 수 있기때문에,
		// 이 NONE이 제일 처음에 없으면 첫줄에 새로운 항목을 추가할 수 없다.
		_macro->_item.resize(1);
		_macro->_item[0].type = sMacroItem::NONE;
	}

	for (unsigned int i=0; i<_macro->_item.size(); ++i) {
	   _listItems.InsertString (i, GetItemDescription (_macro->_item[i]));
	}
	_listItems.SetCurSel(0);

	SetTextRecCount();

	CheckDlgButton (IDC_CHECK_REC_KEY,				(ini_recOption & 0x01) ? BST_CHECKED : BST_UNCHECKED);	// Keyboard 입력 기록
	CheckDlgButton (IDC_CHECK_REC_MOUSE,			(ini_recOption & 0x02) ? BST_CHECKED : BST_UNCHECKED);	// Mouse Button 입력 기록
	CheckDlgButton (IDC_CHECK_REC_MOUSE_POS,		(ini_recOption & 0x04) ? BST_CHECKED : BST_UNCHECKED);	// Mouse 위치 변화 기록
	CheckDlgButton (IDC_CHECK_REC_MOUSE_WHEEL,		(ini_recOption & 0x08) ? BST_CHECKED : BST_UNCHECKED);	// Mouse 휠 변화 기록
	CheckDlgButton (IDC_CHECK_TIME,					(ini_recOption & 0x10) ? BST_CHECKED : BST_UNCHECKED);	// Keyboard나 Mouse 이벤트간 시간 간격 기록
	CheckDlgButton (IDC_CHECK_MERGE,				(ini_recOption & 0x20) ? BST_CHECKED : BST_UNCHECKED);	// Button Up/Down 이벤트 합치기 허용
	CheckDlgButton (IDC_CHECK_MOUSE_POS_ABSOLUTE,   (ini_recOption & 0x80) ? BST_CHECKED : BST_UNCHECKED);	// Check일때는 Mouse의 절대 위치를 기록, Uncheck일때는 상대적 위치를 기록
	CheckDlgButton (IDC_CHECK_MOUSE_POS_SCRINDEP,	(ini_recOption & 0x40) ? BST_CHECKED : BST_UNCHECKED);	// Mouse 위치 변화를 화면 크기에 독립적으로 기록(0 ~ 65535)
	CheckDlgButton (IDC_CHECK_REC_BUTTON_MOVE,		(ini_recOption & 0x100) ? BST_CHECKED : BST_UNCHECKED);	// Mouse Button 입력 기록시 커서 위치도 같이 기록
	
	SetDlgItemInt (IDC_EDIT_MOVE, ini_recordMinMouseDistance);
	SetDlgItemInt (IDC_EDIT_TIME, ini_recordMinTimeInterval);

	_macro_rec = false;
	_rec_time = 0;

	OnBnClickedCheckRecStat();
	OnBnClickedCheckMousePosAbsolute();

	SetTimer (1000, 33, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogMacroRec::SetTextRecCount()
{
	char text[256];

	sprintf (text, "%d개 항목이 기록됨 (%g 초)", _macro->_item.size()-1, _macro->ExecTime ()/1000.);

	SetDlgItemText (IDC_STATIC_REC_COUNT, text);
}

void CDialogMacroRec::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1000) {
		if (_listItems.GetCount() < (int)_macro->_item.size()) {
			for (unsigned int i=_listItems.GetCount(); i<_macro->_item.size(); ++i) {
			   _listItems.InsertString (i, GetItemDescription (_macro->_item[i]));
			}
			_listItems.SetCurSel(_macro->_item.size()-1);
			_listItems.ShowCaret ();

			SetTextRecCount();
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CDialogMacroRec::OnKeyboardEvent (int vk, WPARAM wParam, KBDLLHOOKSTRUCT *stat)
{
	if (vk == ini_recordStartStopKey) {
		MacroRecStateChange (!_macro_rec);
	}

	if (_macro_rec) {
		if (ini_recOption & 0x01) {	// 0x01 - Keyboard 입력 기록
			if      (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) AddKey (stat->vkCode, 1);
			else if (wParam == WM_KEYUP   || wParam == WM_SYSKEYUP)   AddKey (stat->vkCode, 2);
		}
	}
}

long CDialogMacroRec::GetMouseButtonFlags (long message, MSLLHOOKSTRUCT *stat)
{
	if (message == WM_LBUTTONDOWN)      return MOUSEEVENTF_LEFTDOWN;
	else if (message == WM_LBUTTONUP)   return MOUSEEVENTF_LEFTUP;
	else if (message == WM_RBUTTONDOWN) return MOUSEEVENTF_RIGHTDOWN;
	else if (message == WM_RBUTTONUP)   return MOUSEEVENTF_RIGHTUP;
	else if (message == WM_MBUTTONDOWN) return MOUSEEVENTF_MIDDLEDOWN;
	else if (message == WM_MBUTTONUP)   return MOUSEEVENTF_MIDDLEUP;
	else if (message == WM_XBUTTONDOWN) return MAKELONG(MOUSEEVENTF_XDOWN, HIWORD(stat->mouseData));
	else if (message == WM_XBUTTONUP)   return MAKELONG(MOUSEEVENTF_XUP,   HIWORD(stat->mouseData));
	else return 0;
}

void CDialogMacroRec::OnMouseEvent (int vk, WPARAM wParam, MSLLHOOKSTRUCT *stat)
{
	if (vk == ini_recordStartStopKey) {
		MacroRecStateChange (!_macro_rec);
	}

	if (_macro_rec) {
		static int sx = 0;
		static int sy = 0;
		int mx = stat->pt.x;
		int my = stat->pt.y;
		int flags = 0;

		if (ini_recOption & 0x02) {	// 0x02 - Mouse Button 입력 기록
			long button = GetMouseButtonFlags (wParam, stat);
			if (button) {
				if (ini_recOption & 0x100) { // 0x100 - Mouse Button 입력 기록시 커서 위치도 같이 기록
					flags |= MOUSEEVENTF_MOVE;
					flags |= button;

					if (ini_recOption & 0x80) {	// 0x80 - Check일때는 Mouse의 절대 위치를 기록, Uncheck일때는 상대적 위치를 기록
						flags |= MOUSEEVENTF_ABSOLUTE;

						if (ini_recOption & 0x40) {	// 0x40 - Mouse 위치 변화를 화면 크기에 독립적으로 기록(0 ~ 65535)
							ScreenIndependentMousePos (mx, my);
							flags |= MOUSEEVENTF_SCREEN_INDEPENDENT_POS;
						}
						AddMouse (mx, my, flags);
						sx = mx; sy = my;
					}
					else {
						AddMouse (mx-sx, my-sy, flags);
						sx = mx; sy = my;
					}
				}
				else {
					flags |= button;
					AddMouse (mx, my, flags);
				}
			}
		}
		if (ini_recOption & 0x04) {	// 0x04 - Mouse 위치 변화 기록
			if (wParam == WM_MOUSEMOVE) { 
				flags |= MOUSEEVENTF_MOVE;

				if (ini_recOption & 0x80) {	// 0x80 - Check일때는 Mouse의 절대 위치를 기록, Uncheck일때는 상대적 위치를 기록
					flags |= MOUSEEVENTF_ABSOLUTE;

					if (ini_recOption & 0x40) {	// 0x40 - Mouse 위치 변화를 화면 크기에 독립적으로 기록(0 ~ 65535)
						ScreenIndependentMousePos (mx, my);
						flags |= MOUSEEVENTF_SCREEN_INDEPENDENT_POS;
					}
					if (MouseDistance(stat->pt.x, stat->pt.y) >= ini_recordMinMouseDistance) {
						AddMouse (mx, my, flags);
						sx = mx; sy = my;
					}
				}
				else {
					if (MouseDistance(mx, my, sx, sy) >= ini_recordMinMouseDistance) {
						AddMouse (mx-sx, my-sy, flags);
						sx = mx; sy = my;
					}
				}
			}
		}
		if (ini_recOption & 0x08) {	// 0x08 - Mouse 휠 변화 기록
			if (wParam == WM_MOUSEWHEEL) {
				AddMouse (mx, my, MAKELONG(MOUSEEVENTF_WHEEL, HIWORD(stat->mouseData)));
			}
		}
	}
}

int CDialogMacroRec::MouseDistance (int px, int py)
{
	if (_macro->_item.size() > 0) {
		sMacroItem &_item = _macro->_item[_macro->_item.size()-1];
		if (_item.type == sMacroItem::MOUSE && (_item.mouse.flags&MOUSEEVENTF_MOVE)) {
			int mx = _item.mouse.x;
			int my = _item.mouse.y;

			if (_item.mouse.flags&MOUSEEVENTF_SCREEN_INDEPENDENT_POS){
				ScreenDependentMousePos (mx, my);
			}

			int dx = mx - px;
			int dy = my - py;

			return (int)sqrt((double)dx*dx + dy*dy);
		}
	}
	return 1000000;
}

int CDialogMacroRec::MouseDistance (int px, int py, int mx, int my)
{
	int dx = mx - px;
	int dy = my - py;

	return (int)sqrt((double)dx*dx + dy*dy);
}

void CDialogMacroRec::AddTimeDelayIf ()
{
	if (ini_recOption & 0x10) {	// 0x10 - Keyboard나 Mouse 이벤트간 시간 간격 기록
		DWORD cur_time = CMmTimer::GetTime ();
		if ((int)(cur_time - _rec_time) >= ini_recordMinTimeInterval) {
			AddTimeDelay (cur_time - _rec_time);
		}
		_rec_time = cur_time;
	}
}

bool CDialogMacroRec::LastIsSameKeyDown (int vk)
{
	if (_macro->_item.size() > 0) {
		sMacroItem &_item = _macro->_item[_macro->_item.size() - 1];
		if (_item.type == sMacroItem::KEY &&
			_item.key.vk == vk &&
			_item.key.flags == 1) return true;
	}
	return false;
}

void CDialogMacroRec::AddKey (long vk, long flags)
{
	AddTimeDelayIf ();

	if ((ini_recOption&0x20) && flags==2 && LastIsSameKeyDown(vk)) {	// 0x20 - Button Up/Down 이벤트 합치기 허용
		// 이전 키가 눌린 상태라서 누르고 떼기 상태로 바꾼다.
		_macro->_item[_macro->_item.size()-1].key.flags = 3;

		_listItems.DeleteString (_macro->_item.size()-1);
	}
	else {
		sMacroItem _item;

		_item.type      = sMacroItem::KEY;
		_item.key.vk    = vk;
		_item.key.flags = flags;

		_macro->_item.push_back (_item);
	}
}

bool CDialogMacroRec::LastIsSameMouseDown (long x, long y, long flags)
{
	const int MOUSEEVENTF_DOWN = MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_RIGHTDOWN|MOUSEEVENTF_MIDDLEDOWN|MOUSEEVENTF_XDOWN;

	if (_macro->_item.size() > 0) {
		sMacroItem &_item = _macro->_item[_macro->_item.size() - 1];
		if (_item.type == sMacroItem::MOUSE) {
			if ((_item.mouse.flags&MOUSEEVENTF_LEFTDOWN   && flags&MOUSEEVENTF_LEFTUP) ||
				(_item.mouse.flags&MOUSEEVENTF_RIGHTDOWN  && flags&MOUSEEVENTF_RIGHTUP) ||
				(_item.mouse.flags&MOUSEEVENTF_MIDDLEDOWN && flags&MOUSEEVENTF_MIDDLEUP) ||
				(_item.mouse.flags&MOUSEEVENTF_XDOWN      && flags&MOUSEEVENTF_XUP) ){

				if (_item.mouse.x == x && _item.mouse.y == y) return true;
			}
		}
	}
	return false;
}

void CDialogMacroRec::AddMouse (long mx, long my, long flags)
{
	AddTimeDelayIf ();

	const int MOUSEEVENTF_UP = MOUSEEVENTF_LEFTUP|MOUSEEVENTF_RIGHTUP|MOUSEEVENTF_MIDDLEUP|MOUSEEVENTF_XUP;

	if ((ini_recOption&0x20) && (flags&MOUSEEVENTF_UP) && LastIsSameMouseDown(mx, my, flags)) {	// 0x20 - Button Up/Down 이벤트 합치기 허용
		// 이전 마우스 버튼이 눌린 상태라서 클릭 상태로 바꾼다.
		_macro->_item[_macro->_item.size()-1].mouse.flags |= flags;

		_listItems.DeleteString (_macro->_item.size()-1);
	}
	else {
		sMacroItem _item;

		_item.type = sMacroItem::MOUSE;
		_item.mouse.flags = flags;
		_item.mouse.x = mx;
		_item.mouse.y = my;

		_macro->_item.push_back (_item);
	}
}

void CDialogMacroRec::AddTimeDelay (long delay)
{
	sMacroItem _item;

	_item.type = sMacroItem::TIME;
	_item.time.delay = delay;
	_item.time.flags = 0;

	_macro->_item.push_back (_item);
}

void CDialogMacroRec::EnableWindowItem(BOOL enable)
{
	GetDlgItem(IDC_CHECK_REC_KEY)->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_REC_MOUSE)->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_REC_MOUSE_POS)->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_REC_MOUSE_WHEEL)->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_TIME)->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_MERGE)->EnableWindow (enable);
	GetDlgItem(IDC_EDIT_MOVE)->EnableWindow (enable);
	GetDlgItem(IDC_EDIT_TIME)->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_MOUSE_POS_ABSOLUTE)->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_MOUSE_POS_SCRINDEP)->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_REC_BUTTON_MOVE)->EnableWindow (enable);

	if (enable) OnBnClickedCheckMousePosAbsolute();
}

void CDialogMacroRec::MacroRecStateChange(bool rec)
{
	ini_recordMinMouseDistance = GetDlgItemInt (IDC_EDIT_MOVE);
	ini_recordMinTimeInterval  = GetDlgItemInt (IDC_EDIT_TIME);

	if (rec) {
		CheckDlgButton (IDC_CHECK_REC_STAT, BST_CHECKED);
		SetDlgItemText (IDC_CHECK_REC_STAT, "매크로 기록 중");

		EnableWindowItem (FALSE);

		_macro_rec = true;
		_rec_time = CMmTimer::GetTime();
	}
	else {
		_macro_rec = false;

		CheckDlgButton (IDC_CHECK_REC_STAT, BST_UNCHECKED);
		SetDlgItemText (IDC_CHECK_REC_STAT, "기록 대기 중");

		EnableWindowItem (TRUE);
	}
}

void CDialogMacroRec::MacroRecOptionChanged()
{
	ini_recOption = 0;

	if (IsDlgButtonChecked (IDC_CHECK_REC_KEY)				== BST_CHECKED) ini_recOption |= 0x01;	// Keyboard 입력 기록
	if (IsDlgButtonChecked (IDC_CHECK_REC_MOUSE)			== BST_CHECKED) ini_recOption |= 0x02;	// Mouse Button 입력 기록
	if (IsDlgButtonChecked (IDC_CHECK_REC_MOUSE_POS)		== BST_CHECKED) ini_recOption |= 0x04;	// Mouse 위치 변화 기록
	if (IsDlgButtonChecked (IDC_CHECK_REC_MOUSE_WHEEL)		== BST_CHECKED) ini_recOption |= 0x08;	// Mouse 휠 변화 기록
	if (IsDlgButtonChecked (IDC_CHECK_TIME)					== BST_CHECKED) ini_recOption |= 0x10;	// Keyboard나 Mouse 이벤트간 시간 간격 기록
	if (IsDlgButtonChecked (IDC_CHECK_MERGE)				== BST_CHECKED) ini_recOption |= 0x20;	// Button Up/Down 이벤트 합치기 허용
	if (IsDlgButtonChecked (IDC_CHECK_MOUSE_POS_ABSOLUTE)	== BST_CHECKED) ini_recOption |= 0x80;	// Check일때는 Mouse의 절대 위치를 기록, Uncheck일때는 상대적 위치를 기록
	if (IsDlgButtonChecked (IDC_CHECK_MOUSE_POS_SCRINDEP)	== BST_CHECKED) ini_recOption |= 0x40;	// Mouse 위치 변화를 화면 크기에 독립적으로 기록(0 ~ 65535)
	if (IsDlgButtonChecked (IDC_CHECK_REC_BUTTON_MOVE)		== BST_CHECKED) ini_recOption |= 0x100;	// Mouse Button 입력 기록시 커서 위치도 같이 기록
}


void CDialogMacroRec::OnBnClickedCheckRecStat()
{
	MacroRecStateChange (IsDlgButtonChecked (IDC_CHECK_REC_STAT) == BST_CHECKED);

	GetDlgItem (IDC_LIST_ITEMS)->SetFocus ();
}

void CDialogMacroRec::OnBnClickedCheckRecKey()
{
	MacroRecOptionChanged();
}

void CDialogMacroRec::OnBnClickedCheckRecMouse()
{
	MacroRecOptionChanged();
}

void CDialogMacroRec::OnBnClickedCheckRecMousePos()
{
	MacroRecOptionChanged();
}

void CDialogMacroRec::OnBnClickedCheckRecMouseWheel()
{
	MacroRecOptionChanged();
}

void CDialogMacroRec::OnBnClickedCheckTime()
{
	MacroRecOptionChanged();
}

void CDialogMacroRec::OnBnClickedCheckMerge()
{
	MacroRecOptionChanged();
}

void CDialogMacroRec::OnBnClickedCheckMousePosScrindep()
{
	MacroRecOptionChanged();
}

void CDialogMacroRec::OnBnClickedCheckRecButtonMove()
{
	MacroRecOptionChanged();
}

void CDialogMacroRec::OnBnClickedCheckMousePosAbsolute()
{
	if (IsDlgButtonChecked(IDC_CHECK_MOUSE_POS_ABSOLUTE)) {
		GetDlgItem(IDC_CHECK_MOUSE_POS_SCRINDEP)->EnableWindow (TRUE);
	}
	else {
		CheckDlgButton (IDC_CHECK_MOUSE_POS_SCRINDEP, BST_UNCHECKED);
		GetDlgItem(IDC_CHECK_MOUSE_POS_SCRINDEP)->EnableWindow (FALSE);
	}
	MacroRecOptionChanged();
}

void CDialogMacroRec::OnBnClickedOk()
{
	MacroRecStateChange (false);

	OnOK();
}

void CDialogMacroRec::OnBnClickedCancel()
{
	MacroRecStateChange (false);

	OnCancel();
}
