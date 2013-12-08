#include "stdafx.h"
#include "key_macro.h"
#include "DialogCS.h"

// 프로그램이 실행되는 동안 대화상자 아이템에 
// 설정된 값을 저장하고 있는 변수들을 static으로 선언
static long _flags = 0x01;


IMPLEMENT_DYNAMIC(CDialogCS, CDialog)

CDialogCS::CDialogCS(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogCS::IDD, pParent)
{
}

void CDialogCS::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogCS, CDialog)
	ON_BN_CLICKED(IDOK, &CDialogCS::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDialogCS::OnBnClickedCancel)
END_MESSAGE_MAP()


BOOL CDialogCS::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (_item.type == sMacroItem::NONE) {
		SetWindowText ("독립실행구간 추가");
		// 기본값 설정
		_item.type = sMacroItem::CS;
		_item.lock.flags = _flags;
	}
	else {
		SetWindowText ("독립실행구간 편집");
	}

	CheckDlgButton (IDC_RADIO_CS_ENTER,  (_item.lock.flags&0x01) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_RADIO_CS_LEAVE, !(_item.lock.flags&0x01) ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;
}

void CDialogCS::OnBnClickedOk()
{
	_item.lock.flags = 0;
	if (IsDlgButtonChecked (IDC_RADIO_CS_ENTER)) _item.lock.flags = 0x01;
	if (IsDlgButtonChecked (IDC_RADIO_CS_LEAVE)) _item.lock.flags = 0x00;

	_flags = (_item.lock.flags&0x01) ? 0x00 : 0x01;

	OnOK();
}

void CDialogCS::OnBnClickedCancel()
{
	OnCancel();
}
