#include "stdafx.h"
#include "key_macro.h"
#include "DialogMacro.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 프로그램이 실행되는 동안 대화상자 아이템에 
// 설정된 값을 저장하고 있는 변수들을 static으로 선언
static int _macro_ID = 0;

IMPLEMENT_DYNAMIC(CDialogMacro, CDialog)

CDialogMacro::CDialogMacro(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMacro::IDD, pParent)
{
}

void CDialogMacro::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogMacro, CDialog)
	ON_BN_CLICKED(IDOK, &CDialogMacro::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL CDialogMacro::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (_item.type == sMacroItem::NONE) {
		SetWindowText ("매크로 추가");
		// 기본값 설정
		_item.type          = sMacroItem::MACRO;
		_item.mcall.name    = new char [255+1];
		_item.mcall.name[0] = '\0';
		
		if (0 <= _macro_ID && _macro_ID < (int)g_macros.size()) {
			strncpy (_item.mcall.name, g_macros[_macro_ID].name, 255);
			_item.mcall.name[255] = '\0';
		}
	}
	else {
		SetWindowText ("매크로 편집");
	}

	for (unsigned int i=0; i<g_macros.size(); ++i) {
		SendDlgItemMessage (IDC_COMBO_MACRO, CB_ADDSTRING, 0, (LPARAM)g_macros[i].name);
	}
	SendDlgItemMessage (IDC_COMBO_MACRO, CB_SETCURSEL, GetMacroID(_item.mcall.name));

	GetDlgItem(IDC_COMBO_MACRO)->SetFocus ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogMacro::OnBnClickedOk()
{
	int index = SendDlgItemMessage (IDC_COMBO_MACRO, CB_GETCURSEL, 0, 0);
	if (index < 0) {
		AfxMessageBox ("호출할 매크로가 선택되지 않았습니다.");
		return;
	}

	strncpy (_item.mcall.name, g_macros[index].name, 255);
	_item.mcall.name[255] = '\0';

	_macro_ID = index;

	OnOK();
}
