#include "stdafx.h"
#include "key_macro.h"
#include "DialogConfig.h"
#include "virtual_key.h"
#include "ini.h"


IMPLEMENT_DYNAMIC(CDialogConfig, CDialog)

CDialogConfig::CDialogConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConfig::IDD, pParent)
{
}

void CDialogConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogConfig, CDialog)
	ON_BN_CLICKED(IDOK, &CDialogConfig::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDialogConfig::OnBnClickedCancel)
END_MESSAGE_MAP()


BOOL CDialogConfig::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (int i=0; i<256 && g_vk_list[i].vk != -1; ++i) {
		SendDlgItemMessage (IDC_COMBO_MACRO_RUN_EDIT_KEY,    CB_ADDSTRING, 0, (LPARAM)g_vk_list[i].desc);
		SendDlgItemMessage (IDC_COMBO_RECORD_START_STOP_KEY, CB_ADDSTRING, 0, (LPARAM)g_vk_list[i].desc);
		SendDlgItemMessage (IDC_COMBO_MOUSE_CAPTURE_KEY,     CB_ADDSTRING, 0, (LPARAM)g_vk_list[i].desc);
	}
	SendDlgItemMessage (IDC_COMBO_MACRO_RUN_EDIT_KEY,    CB_SETCURSEL, getVkIndex (ini_macroRunEditKey));
	SendDlgItemMessage (IDC_COMBO_RECORD_START_STOP_KEY, CB_SETCURSEL, getVkIndex (ini_recordStartStopKey));
	SendDlgItemMessage (IDC_COMBO_MOUSE_CAPTURE_KEY,     CB_SETCURSEL, getVkIndex (ini_mouseCaptureKey));

	CheckDlgButton (IDC_CHECK_ALWAYS_ON_TOP,		ini_alwaysOnTop   ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_RUN_SEL_MACRO,		ini_runSelMacro   ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_TRAY_AT_STARTUP,		ini_trayAtStartup ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_MACRO_KEY_REENTRANCE,	ini_keyReentrance ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_USE_MM_TIMER,			ini_useMmTimer    ? BST_CHECKED : BST_UNCHECKED);

	if (ini_eventMinDelay < 1) ini_eventMinDelay = 1;
	SetDlgItemInt (IDC_EDIT_EVENT_MIN_DELAY, ini_eventMinDelay);

	return TRUE;
}

void CDialogConfig::EnableDlgItem(BOOL enable)
{
	GetDlgItem (IDC_COMBO_MACRO_RUN_EDIT_KEY)   ->EnableWindow (enable);
	GetDlgItem (IDC_COMBO_RECORD_START_STOP_KEY)->EnableWindow (enable);
	GetDlgItem (IDC_COMBO_MOUSE_CAPTURE_KEY)    ->EnableWindow (enable);

	GetDlgItem (IDC_CHECK_ALWAYS_ON_TOP)       ->EnableWindow (enable);
	GetDlgItem (IDC_CHECK_TRAY_AT_STARTUP)     ->EnableWindow (enable);
	GetDlgItem (IDC_CHECK_MACRO_KEY_REENTRANCE)->EnableWindow (enable);
	GetDlgItem (IDC_CHECK_RUN_SEL_MACRO)       ->EnableWindow (enable);
	GetDlgItem (IDC_EDIT_EVENT_MIN_DELAY)      ->EnableWindow (enable);
}

void CDialogConfig::OnBnClickedOk()
{
	ini_alwaysOnTop		= IsDlgButtonChecked (IDC_CHECK_ALWAYS_ON_TOP);
	ini_trayAtStartup	= IsDlgButtonChecked (IDC_CHECK_TRAY_AT_STARTUP);
	ini_keyReentrance	= IsDlgButtonChecked (IDC_CHECK_MACRO_KEY_REENTRANCE);
	ini_runSelMacro		= IsDlgButtonChecked (IDC_CHECK_RUN_SEL_MACRO);
	ini_useMmTimer	    = IsDlgButtonChecked (IDC_CHECK_USE_MM_TIMER);

	ini_eventMinDelay = GetDlgItemInt (IDC_EDIT_EVENT_MIN_DELAY);
	if (ini_eventMinDelay < 1) ini_eventMinDelay = 1;

	int index = SendDlgItemMessage (IDC_COMBO_MACRO_RUN_EDIT_KEY, CB_GETCURSEL, 0, 0);
	if (index != -1) ini_macroRunEditKey = g_vk_list[index].vk;

	index = SendDlgItemMessage (IDC_COMBO_RECORD_START_STOP_KEY, CB_GETCURSEL, 0, 0);
	if (index != -1) ini_recordStartStopKey = g_vk_list[index].vk;

	index = SendDlgItemMessage (IDC_COMBO_MOUSE_CAPTURE_KEY, CB_GETCURSEL, 0, 0);
	if (index != -1) ini_mouseCaptureKey = g_vk_list[index].vk;

	OnOK();
}

void CDialogConfig::OnBnClickedCancel()
{
	OnCancel();
}
