#include "stdafx.h"
#include "key_macro.h"
#include "DialogDelay.h"
#include "Common.h"

// 프로그램이 실행되는 동안 대화상자 아이템에 
// 설정된 값을 저장하고 있는 변수들을 static으로 선언
static long _delay = 10;
static long _flags = 0;


IMPLEMENT_DYNAMIC(CDialogDelay, CDialog)

CDialogDelay::CDialogDelay(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDelay::IDD, pParent)
{
}

void CDialogDelay::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_RANDOM, _comboRandom);
}


BEGIN_MESSAGE_MAP(CDialogDelay, CDialog)
	ON_BN_CLICKED(IDOK, &CDialogDelay::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_DELAY, &CDialogDelay::OnBnClickedRadioDelay)
	ON_BN_CLICKED(IDC_RADIO_TIME, &CDialogDelay::OnBnClickedRadioTime)
END_MESSAGE_MAP()


BOOL CDialogDelay::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (_item.type == sMacroItem::NONE) {
		SetWindowText ("시간 지연 추가");
		// 기본값 설정
		_item.type = sMacroItem::TIME;
		_item.time.delay = _delay;
		_item.time.flags = _flags;
	}
	else {
		SetWindowText ("시간 지연 편집");
	}

	CheckDlgButton (IDC_RADIO_DELAY, !(_item.time.flags&0x01) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_RADIO_TIME,   (_item.time.flags&0x01) ? BST_CHECKED : BST_UNCHECKED);
	
	char text[32];
	for (int i=0; i<=100; i+=10) {
		sprintf (text, "+- %d%%", i);
		_comboRandom.AddString (text);
	}
	_comboRandom.SetCurSel (BOUND(_item.time.flags >> 16, 0, 10));

	for (int i=-1; i<60; ++i) {
		if (i < 0) sprintf (text, "--");
		else       sprintf (text, "%d", i);

		if (i<24) SendDlgItemMessage (IDC_COMBO_HOUR, CB_ADDSTRING, 0, (LPARAM)text);
		SendDlgItemMessage (IDC_COMBO_MIN, CB_ADDSTRING, 0, (LPARAM)text);
		SendDlgItemMessage (IDC_COMBO_SEC, CB_ADDSTRING, 0, (LPARAM)text);
	}

	SetDlgItemDouble (IDC_EDIT_DELAY, (double)_item.time.delay/1000.);

	int flag = (_item.time.flags>>1)&0x03;
	SendDlgItemMessage (IDC_COMBO_HOUR, CB_SETCURSEL, (flag >= 3) ? (_item.time.delay/(60*60*1000))%24 + 1 : 0);
	SendDlgItemMessage (IDC_COMBO_MIN,  CB_SETCURSEL, (flag >= 2) ? (_item.time.delay/(   60*1000))%60 + 1 : 0);
	SendDlgItemMessage (IDC_COMBO_SEC,  CB_SETCURSEL, (flag >= 1) ? (_item.time.delay/(      1000))%60 + 1 : 0);

	OnBnClickedRadioDelay();
	OnBnClickedRadioTime();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogDelay::OnBnClickedOk()
{
	// flags에 대한 설명:
	// - bit 0    : 0인 경우 delay 설정, 1인 경우 time 설정
	// - bit 1, 2 : 1인 경우 초만 설정, 2인경우 초와 분 설정, 3인 경우 시 분 초 설정

	if (IsDlgButtonChecked (IDC_RADIO_DELAY)) {
		// delay는 밀리초 단위로 설정
		_delay = (int)(1000*GetDlgItemDouble (IDC_EDIT_DELAY));
		_delay = BOUND(_delay, 0L, 86400000L);

		_flags = 0x00;
		_flags |= _comboRandom.GetCurSel () << 16;

	}
	else if (IsDlgButtonChecked (IDC_RADIO_TIME)) {
		int hour = SendDlgItemMessage (IDC_COMBO_HOUR, CB_GETCURSEL) - 1;
		int min_ = SendDlgItemMessage (IDC_COMBO_MIN,  CB_GETCURSEL) - 1;
		int sec_ = SendDlgItemMessage (IDC_COMBO_SEC,  CB_GETCURSEL) - 1;

		// 시/분/초 로 표시되는 시각을 msec 단위의 시간으로 바꾼다.
		_delay = 0;
		int time_flags = 0;
		if (sec_ >= 0) {	
			_delay += sec_*1000;	 
			time_flags = 1; 
		
			if (min_ >= 0) {	
				_delay += min_*60*1000;	 
				time_flags = 2; 
			
				if (hour >= 0) {	
					_delay += hour*60*60*1000;	 
					time_flags = 3; 
				}
			}
		}
		_flags = 0x01 | (time_flags<<1);
	}

	_item.time.delay = _delay;
	_item.time.flags = _flags;

	OnOK();
}

void CDialogDelay::OnBnClickedRadioDelay()
{
	if (IsDlgButtonChecked (IDC_RADIO_DELAY)) {
		GetDlgItem (IDC_EDIT_DELAY)->EnableWindow (TRUE);
		GetDlgItem (IDC_COMBO_HOUR)->EnableWindow (FALSE);
		GetDlgItem (IDC_COMBO_MIN)->EnableWindow (FALSE);
		GetDlgItem (IDC_COMBO_SEC)->EnableWindow (FALSE);
	}
}

void CDialogDelay::OnBnClickedRadioTime()
{
	if (IsDlgButtonChecked (IDC_RADIO_TIME)) {
		GetDlgItem (IDC_EDIT_DELAY)->EnableWindow (FALSE);
		GetDlgItem (IDC_COMBO_HOUR)->EnableWindow (TRUE);
		GetDlgItem (IDC_COMBO_MIN)->EnableWindow (TRUE);
		GetDlgItem (IDC_COMBO_SEC)->EnableWindow (TRUE);
	}
}
