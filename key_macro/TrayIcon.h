#pragma once

#define WM_TRAY_ICON	(WM_USER+1435)

// Tray�� ������ȭ �Ͽ� �����ֱ����� �Լ��� 
int TrayIconCreate(HICON _hIcon, HWND hWnd, char *str);
void TrayIconClose(HWND hWnd);
void TrayIconChange(HWND hWnd, char *str);
