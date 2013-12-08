// DialogString.cpp : implementation file
//
#include "stdafx.h"
#include "key_macro.h"
#include "DialogString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���α׷��� ����Ǵ� ���� ��ȭ���� �����ۿ� 
// ������ ���� �����ϰ� �ִ� �������� static���� ����
static long _flags = 0;

// CDialogString dialog
IMPLEMENT_DYNAMIC(CDialogString, CDialog)

CDialogString::CDialogString(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogString::IDD, pParent)
{
}

void CDialogString::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogString, CDialog)
	ON_BN_CLICKED(IDOK, &CDialogString::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDialogString::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDialogString message handlers
BOOL CDialogString::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if (_item.type == sMacroItem::NONE) {
		SetWindowText ("���ڿ� �߰�");
		// �⺻�� ����
		_item.type        = sMacroItem::STRING;
		_item.str.length  = 0;
		_item.str.flags   = _flags;
		_item.str.text    = new char [1];
		_item.str.text[0] = '\0';
	}
	else {
		SetWindowText ("���ڿ� ����");
		SetDlgItemText (IDC_EDIT_STRING, _item.str.text);
	}

	CheckDlgButton (IDC_CHECK_RANDOM_CHAR, (_item.str.flags & 0x01) ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogString::OnBnClickedOk()
{
	int length = GetDlgItem(IDC_EDIT_STRING)->GetWindowTextLength ();
	if (length <= 0) {
		AfxMessageBox ("���ڿ��� ��� �ֽ��ϴ�.");
		return;
	}

	delete [] _item.str.text;
	_item.str.length = length;
	_item.str.text = new char[length+1];
	
	GetDlgItemText (IDC_EDIT_STRING, _item.str.text, length+1);
	_item.str.text[length] = '\0';

	if (IsDlgButtonChecked (IDC_CHECK_RANDOM_CHAR)) _item.str.flags = 0x01;
	else _item.str.flags = 0x00;

	_flags = _item.str.flags;

	OnOK();
}

void CDialogString::OnBnClickedCancel()
{
	OnCancel();
}
