#include "stdafx.h"
#include "TrayIcon.h"

#define ID_TRAY_LOAD		4567

static HICON hIcon;
static int icon = 0;

int TrayIconCreate(HICON _hIcon, HWND hWnd, char *str) 
{
	hIcon = _hIcon;

	NOTIFYICONDATA tnd;
	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd = hWnd;
	tnd.uID = ID_TRAY_LOAD;
	tnd.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
	tnd.uCallbackMessage = WM_TRAY_ICON;
	tnd.hIcon = hIcon;
	strncpy(tnd.szTip,str,min(64,1+strlen(str)));
	tnd.szTip[63] = '\0';

	Shell_NotifyIcon(NIM_ADD,&tnd);

	return 0;
}

void TrayIconClose(HWND hWnd) 
{
	NOTIFYICONDATA tnd;

	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd = hWnd;
	tnd.uID = ID_TRAY_LOAD;

	Shell_NotifyIcon(NIM_DELETE,&tnd);

	DestroyIcon(hIcon);
}

void TrayIconChange(HWND hWnd, char *str) 
{
	NOTIFYICONDATA tnd;

	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd = hWnd;
	tnd.uID = ID_TRAY_LOAD;
	tnd.hIcon = hIcon;
	tnd.uFlags = NIF_ICON|NIF_TIP;
	strncpy(tnd.szTip,str,min(64,1+strlen(str)));
	tnd.szTip[63] = '\0';

	Shell_NotifyIcon(NIM_MODIFY,&tnd);
}
