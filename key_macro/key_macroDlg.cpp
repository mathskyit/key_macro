// key_macroDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "key_macro.h"
#include "key_macroDlg.h"
#include "DialogMacroEdit.h"
#include "DialogMacroRec.h"
#include "DialogConfig.h"
#include "macro_def.h"
#include "key_hook.h"
#include "virtual_key.h"
#include "TrayIcon.h"
#include "ScopedLock.h"
#include "MmTimer.h"
#include "common.h"
#include "etc.h"
#include "ini.h"


// defined in macro_file.cpp
extern void LoadMacros (const char *fileName);
extern void SaveMacro (const char *fileName);

Ckey_macroDlg *g_macroDlg = NULL;

// Multimedia timer로 Macro 실행에 실시간성 확보
static class CMmtMacros: public CMmTimer {
public:
	virtual void OnTimer (UINT timerId, UINT msg) 
	{
		// Multimedia timer는 새로운 thread로 동작하기 때문에 Windows thread와 
		// 자원 공유에 충돌이 발생할 수 있다. 공유 자원에 Lock을 걸어야 한다.
		if (g_macroDlg && ini_useMmTimer) {
			g_macroDlg->PostMessage (WM_MM_TIMER, 0, 0);
		}
	}
} _mmtms;


// Ckey_macroDlg 대화 상자
Ckey_macroDlg::Ckey_macroDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ckey_macroDlg::IDD, pParent)
{
	_cmd_line[0] = '\0';
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	g_macroDlg = this;

	srand((unsigned int)time(0)+(unsigned int)getpid());
}

Ckey_macroDlg::~Ckey_macroDlg()
{
	g_macroDlg = NULL;
}

void Ckey_macroDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MACROS, _listMacros);
	DDX_Control(pDX, IDC_LIST_MACROS2, _listMacros2);
}

BEGIN_MESSAGE_MAP(Ckey_macroDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_COPYDATA()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_MACRO_RUN, &Ckey_macroDlg::OnBnClickedRadioRun)
	ON_BN_CLICKED(IDC_RADIO_MACRO_EDIT, &Ckey_macroDlg::OnBnClickedRadioEdit)
	ON_BN_CLICKED(IDC_BUTTON_ADD_MACRO, &Ckey_macroDlg::OnBnClickedButtonAddMacro)
	ON_BN_CLICKED(IDC_BUTTON_COPY_MACRO, &Ckey_macroDlg::OnBnClickedButtonCopyMacro)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_MACRO, &Ckey_macroDlg::OnBnClickedButtonDeleteMacro)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_MACRO, &Ckey_macroDlg::OnBnClickedButtonEditMacro)
	ON_BN_CLICKED(IDC_BUTTON_MACRO_FILE, &Ckey_macroDlg::OnBnClickedButtonMacroFile)
	ON_BN_CLICKED(IDC_BUTTON_REC_MACRO, &Ckey_macroDlg::OnBnClickedButtonRecMacro)
	ON_BN_CLICKED(IDC_BUTTON_UP2, &Ckey_macroDlg::OnBnClickedButtonUp2)
	ON_BN_CLICKED(IDC_BUTTON_DOWN2, &Ckey_macroDlg::OnBnClickedButtonDown2)
	ON_LBN_DBLCLK(IDC_LIST_MACROS, &Ckey_macroDlg::OnLbnDblclkListMacros)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &Ckey_macroDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDOK, &Ckey_macroDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &Ckey_macroDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG, &Ckey_macroDlg::OnBnClickedButtonConfig)
END_MESSAGE_MAP()

void Ckey_macroDlg::DlgInScreen ()
{
	RECT rect;
	GetWindowRect (&rect);

	int cxscreen = GetSystemMetrics(SM_CXSCREEN) - (rect.right - rect.left) - 1;
	int cyscreen = GetSystemMetrics(SM_CYSCREEN) - (rect.bottom - rect.top) - 1;

	if (ini_windowSX < 0) ini_windowSX = 0;
	if (ini_windowSX > cxscreen) ini_windowSX = cxscreen;

	if (ini_windowSY < 0) ini_windowSY = 0;
	if (ini_windowSY > cyscreen) ini_windowSY = cyscreen;
}

// Ckey_macroDlg 메시지 처리기
BOOL Ckey_macroDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	if (__argc > 1) ini_macroRun = 1;

	DlgInScreen ();

	char message[1024];
	sprintf (message, "%s로 전체 매크로 실행/편집을 토글합니다.", g_vk_list[getVkIndex (ini_macroRunEditKey)].desc);
	SetDlgItemText (IDC_STATIC_MESSAGE, message);

	SetWindowPos (NULL, ini_windowSX, ini_windowSY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	CheckDlgButton (IDC_RADIO_MACRO_RUN,    ini_macroRun ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_RADIO_MACRO_EDIT,  !ini_macroRun ? BST_CHECKED : BST_UNCHECKED);

	LoadMacros (ini_fileName);
	SetDlgItemText (IDC_EDIT_MACRO_FILE, ini_fileName);

	if (ini_trayAtStartup) PostMessage (WM_SYSCOMMAND, SC_MINIMIZE, 0);

	_listMacros.ResetContent();
	for (unsigned int i=0; i<g_macros.size(); ++i) {
		_listMacros.InsertString (i, g_macros[i].name);
	}
	_listMacros.SetCurSel(ini_macroSel);

	EnableDlgItem (TRUE);

	_listMacros.ShowWindow (SW_SHOW);
	_listMacros2.ShowWindow (SW_HIDE);

	OnBnClickedRadioRun ();
	OnBnClickedRadioEdit ();

	SetTextRecCount();

	// Command line argument로 넘어온 매크로 인덱스를 실행한다.
	for (int i=1; i<__argc; i++) {
		MacroStart (atoi (__argv[i]) - 1);
	}

	InstallHook ();

	SetTimer (1000, 33, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void Ckey_macroDlg::OnPaint()
{
	if (IsIconic()) {
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else {
		CDialog::OnPaint();
	}

	static bool init = false;
	if (!init) { CheckAlwaysontop();  init = true; }
}

HCURSOR Ckey_macroDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL Ckey_macroDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	if (pCopyDataStruct->dwData == 0) {
		strncpy (_cmd_line, (char *)pCopyDataStruct->lpData, min(1024, pCopyDataStruct->cbData));
		
		//TRACE ("%s \n", pCopyDataStruct->lpData);
	}
	
	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}

void Ckey_macroDlg::MacroRestart ()
{
	int nArgs = 0;
	LPSTR *szArglist = CommandLineToArgvA(_cmd_line, &nArgs);
	if (szArglist && 1 < nArgs) {

		ini_macroRun = 1;
		CheckDlgButton (IDC_RADIO_MACRO_RUN,   ini_macroRun ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton (IDC_RADIO_MACRO_EDIT, !ini_macroRun ? BST_CHECKED : BST_UNCHECKED);

		OnBnClickedRadioRun();

		for (int i=1; i<nArgs; i++) {
			MacroStart (atoi (szArglist[i]) - 1);
		}
	}

	GlobalFree(szArglist);
}

LRESULT Ckey_macroDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message){
	case WM_MM_TIMER: {
		DWORD t = CMmTimer::GetTime ();
		AllMacroStep (t - _tp);
		_tp = t;
		break;
	}
	case WM_RESTART:
		MacroRestart ();
		break;
	case WM_TRAY_ICON:
		if((lParam&0x0F)==0x03){
			// Double click
			::ShowWindow(m_hWnd,SW_SHOW);
			TrayIconClose(m_hWnd);
		}
		return 0;
	case WM_SYSCOMMAND:
		if(wParam==SC_MINIMIZE){
			HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
			TrayIconCreate(hIcon, m_hWnd, "키보드/마우스 매크로 프로그램 - 더블클릭 하세요.");
			::ShowWindow(m_hWnd,SW_HIDE);		
		}
		else if(wParam==SC_RESTORE){
			::ShowWindow(m_hWnd,SW_SHOW);
			TrayIconClose(m_hWnd);
		}
		break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void Ckey_macroDlg::OnDestroy()
{
	UninstallHook ();

	RECT rectWnd;
	GetWindowRect (&rectWnd);
	ini_windowSX = rectWnd.left;
	ini_windowSY = rectWnd.top;

	CDialog::OnDestroy();
}

void Ckey_macroDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1000) {
		for (unsigned int i=0; i<g_macros.size(); ++i) {
			_listMacros2.SetSel (i, g_macros[i].is_running());
		}
	}
	else if (nIDEvent == 1001) {
		if (!ini_useMmTimer) {
			DWORD t = CMmTimer::GetTime ();
			AllMacroStep (t - _tp);
			_tp = t;
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}

void Ckey_macroDlg::OnKeyMouseEvent (int vk, bool key)
{
	if (vk == ini_macroRunEditKey) {
		if (IsDlgButtonChecked (IDC_RADIO_MACRO_EDIT)) {
			CheckDlgButton (IDC_RADIO_MACRO_RUN, BST_CHECKED);
			CheckDlgButton (IDC_RADIO_MACRO_EDIT, BST_UNCHECKED);
			OnBnClickedRadioRun();
		}
		else if (IsDlgButtonChecked (IDC_RADIO_MACRO_RUN)) {
			CheckDlgButton (IDC_RADIO_MACRO_RUN, BST_UNCHECKED);
			CheckDlgButton (IDC_RADIO_MACRO_EDIT, BST_CHECKED);
			OnBnClickedRadioEdit();
		}
	}
	else {
		MacroStartStop (vk);
	}
}

void Ckey_macroDlg::OnBnClickedButtonMacroFile()
{
	OnBnClickedButtonSave ();

	///////////////////////////////////////////////////////////////////////////////
	char szFilter[] = "Macro file (*.m)|*.m|All Files(*.*)|*.*||";
	CFileDialog dlg(TRUE, NULL, ini_fileName, OFN_HIDEREADONLY, szFilter);
	
	if(IDOK == dlg.DoModal()) {
		char curPath[MAX_PATH+1] = "";
		GetCurrentDirectory (MAX_PATH, curPath);
		curPath[MAX_PATH] = '\0';

		CString fileNameNew = "";

		if (strnicmp(curPath, (LPCSTR)dlg.GetPathName (), strlen(curPath)) == 0) {
			fileNameNew = (LPCSTR)dlg.GetPathName () + strlen(curPath) + 1;
		}
		else {
			fileNameNew = dlg.GetPathName();
		}

		if (dlg.GetFileExt ().GetLength () == 0) {
			fileNameNew += ".m";
		}

		LoadMacros ((LPCSTR)fileNameNew);

		_listMacros.ResetContent();
		for (unsigned int i=0; i<g_macros.size(); ++i) {
			_listMacros.InsertString (i, g_macros[i].name);
		}
		_listMacros.SetCurSel(0);

		strncpy (ini_fileName, (LPCSTR)fileNameNew, MAX_PATH);
		ini_fileName[MAX_PATH] = '\0';
		SetDlgItemText (IDC_EDIT_MACRO_FILE, ini_fileName);
	}
}

void Ckey_macroDlg::EnableDlgItem(BOOL enable)
{
	GetDlgItem (IDC_BUTTON_SAVE)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_REC_MACRO)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_ADD_MACRO)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_EDIT_MACRO)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_COPY_MACRO)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_DELETE_MACRO)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_MACRO_FILE)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_UP2)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_DOWN2)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_CONFIG)->EnableWindow (enable);
}

void Ckey_macroDlg::OnBnClickedButtonSave()
{
	GetDlgItemText (IDC_EDIT_MACRO_FILE, ini_fileName, MAX_PATH);
	ini_fileName[MAX_PATH] = '\0';
	
	SaveMacro (ini_fileName);
}

void Ckey_macroDlg::OnBnClickedButtonConfig()
{
	CDialogConfig dlg;
	dlg.DoModal ();

	CheckAlwaysontop();
}

void Ckey_macroDlg::OnBnClickedOk()
{
	ini_macroSel = _listMacros.GetCurSel ();

	OnBnClickedButtonSave ();

	OnOK();
}

void Ckey_macroDlg::OnBnClickedCancel()
{
	OnCancel();
}

void Ckey_macroDlg::OnBnClickedRadioRun()
{
	if (!IsMacroRunning() && IsDlgButtonChecked (IDC_RADIO_MACRO_RUN)) {
		ini_macroRun = 1;
		MacroInit ();
		EnableDlgItem (FALSE);
		
		_listMacros2.ResetContent();
		for (unsigned int i=0; i<g_macros.size(); ++i) {
			_listMacros2.InsertString (i, g_macros[i].name);
		}

		_listMacros.ShowWindow (SW_HIDE);
		_listMacros2.ShowWindow (SW_SHOW);

		if (ini_useMmTimer) {
			_mmtms.Start (ini_eventMinDelay);
		}
		else {
			SetTimer (1001, ini_eventMinDelay, NULL);
			_tp = CMmTimer::GetTime ();
		}

		if (ini_runSelMacro) {
			int sel = _listMacros.GetCurSel ();
			MacroStart (sel);
		}
	}
}

void Ckey_macroDlg::OnBnClickedRadioEdit()
{
	if (IsMacroRunning() && IsDlgButtonChecked (IDC_RADIO_MACRO_EDIT)) {
		ini_macroRun = 0;
		MacroTerm ();
		EnableDlgItem (TRUE);

		_listMacros.ShowWindow (SW_SHOW);
		_listMacros2.ShowWindow (SW_HIDE);

		//KillTimer (1000);
		if (ini_useMmTimer) {
			_mmtms.Stop ();
		}
		else {
			KillTimer (1001);
		}
	}
}

void Ckey_macroDlg::CheckAlwaysontop()
{
	if(ini_alwaysOnTop) {
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}
	else {
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}
}

void Ckey_macroDlg::SetTextRecCount()
{
	char text[256];

	sprintf (text, "%d개 매크로 기록됨.", g_macros.size());

	SetDlgItemText (IDC_STATIC_REC_COUNT2, text);
}

void Ckey_macroDlg::OnBnClickedButtonAddMacro()
{
	CDialogMacroEdit dlg;

	sMacro temp;
	dlg._macro = &temp;

	if (dlg.DoModal () == IDOK) {
		int count = _listMacros.GetCount ();

		_listMacros.InsertString (count, temp.name);
		_listMacros.SetCurSel (count);

		g_macros.push_back (temp);
	}

	SetTextRecCount();
}

void Ckey_macroDlg::OnBnClickedButtonEditMacro()
{
	int sel = _listMacros.GetCurSel ();
	if (sel < 0) return;

	CDialogMacroEdit dlg;

	sMacro temp = g_macros[sel];
	dlg._macro = &temp;

	if (dlg.DoModal () == IDOK) {
		_listMacros.DeleteString (sel);
		_listMacros.InsertString (sel, temp.name);
		_listMacros.SetCurSel (sel);

		g_macros[sel] = *dlg._macro;
	}
}

void Ckey_macroDlg::OnBnClickedButtonRecMacro()
{
	CDialogMacroRec dlg;

	sMacro temp;
	dlg._macro = &temp;

	if (dlg.DoModal () == IDOK) {
		int count = _listMacros.GetCount ();

		_listMacros.InsertString (count, temp.name);
		_listMacros.SetCurSel (count);

		g_macros.push_back (temp);
	}

	SetTextRecCount();
}

void Ckey_macroDlg::OnBnClickedButtonCopyMacro()
{
	int sel = _listMacros.GetCurSel ();
	if (sel < 0) return;

	int count = _listMacros.GetCount ();
	sMacro &macro = g_macros[sel];

	_listMacros.InsertString (count, macro.name);
	_listMacros.SetCurSel (count);

	g_macros.push_back (macro);

	SetTextRecCount();
}

void Ckey_macroDlg::OnBnClickedButtonDeleteMacro()
{
	int sel = _listMacros.GetCurSel ();
	if (sel < 0) return;

	_listMacros.DeleteString (sel);
	_listMacros.SetCurSel (sel);

	g_macros.erase (g_macros.begin() + sel);

	SetTextRecCount();
}

void Ckey_macroDlg::OnLbnDblclkListMacros()
{
	OnBnClickedButtonEditMacro();
}

void Ckey_macroDlg::OnBnClickedButtonUp2()
{
	int sel = _listMacros.GetCurSel ();
	if (1 <= sel) {
	   _listMacros.DeleteString (sel);
	   _listMacros.InsertString (sel-1, g_macros[sel].name);
	   _listMacros.SetCurSel (sel-1);

	   std::swap (g_macros[sel], g_macros[sel-1]);
	}
}

void Ckey_macroDlg::OnBnClickedButtonDown2()
{
	int sel = _listMacros.GetCurSel ();
	if (sel < _listMacros.GetCount ()-1) { 
	   _listMacros.DeleteString (sel);
	   _listMacros.InsertString (sel+1, g_macros[sel].name);
	   _listMacros.SetCurSel (sel+1);

		std::swap (g_macros[sel], g_macros[sel+1]);
	}
}
