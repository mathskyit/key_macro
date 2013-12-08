#pragma once

#define WM_TRAY_ICON	(WM_USER+1435)

// Tray에 아이콘화 하여 보여주기위한 함수들 
int TrayIconCreate(HICON _hIcon, HWND hWnd, char *str);
void TrayIconClose(HWND hWnd);
void TrayIconChange(HWND hWnd, char *str);
