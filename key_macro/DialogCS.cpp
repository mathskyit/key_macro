#include "stdafx.h"
#include "key_macro.h"
#include "DialogCS.h"

// ���α׷��� ����Ǵ� ���� ��ȭ���� �����ۿ� 
// ������ ���� �����ϰ� �ִ� �������� static���� ����
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
		SetWindowText ("�������౸�� �߰�");
		// �⺻�� ����
		_item.type = sMacroItem::CS;
		_item.lock.flags = _flags;
	}
	else {
		SetWindowText ("�������౸�� ����");
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
