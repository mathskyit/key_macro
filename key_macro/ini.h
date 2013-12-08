#pragma once

// ���α׷��� ����Ǳ� ���� ini ���Ͽ� �����Ǵ� ������.
// ���α׷� ������ ���� ������ ���� ���ȴ�.
extern int ini_recOption; 
// 0x01 - Keyboard �Է� ���
// 0x02 - Mouse Button �Է� ���
// 0x04 - Mouse ��ġ ��ȭ ���
// 0x08 - Mouse �� ��ȭ ���
// 0x10 - Keyboard�� Mouse �̺�Ʈ�� �ð� ���� ���
// 0x20 - Button Up/Down �̺�Ʈ ��ġ�� ���
// 0x40 - Mouse ��ġ ��ȭ�� ȭ�� ũ�⿡ ���������� ���(0 ~ 65535)

extern int ini_windowSX;
extern int ini_windowSY;  
extern char ini_fileName[MAX_PATH+1];
extern int ini_macroRun;     
extern int ini_alwaysOnTop;      
extern int ini_trayAtStartup;
extern int ini_keyReentrance;
extern int ini_eventMinDelay;
extern int ini_useMmTimer;

extern int ini_macroRunEditKey;
extern int ini_recordStartStopKey;
extern int ini_mouseCaptureKey;

extern int ini_recordMinMouseDistance;
extern int ini_recordMinTimeInterval;

extern int ini_options;
extern int ini_runSelMacro;
extern int ini_macroSel;

extern void IniLoad ();
extern void IniSave ();
