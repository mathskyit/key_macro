#pragma once

// 프로그램이 종료되기 전에 ini 파일에 보관되는 변수들.
// 프로그램 내에서 전역 변수와 같이 사용된다.
extern int ini_recOption; 
// 0x01 - Keyboard 입력 기록
// 0x02 - Mouse Button 입력 기록
// 0x04 - Mouse 위치 변화 기록
// 0x08 - Mouse 휠 변화 기록
// 0x10 - Keyboard나 Mouse 이벤트간 시간 간격 기록
// 0x20 - Button Up/Down 이벤트 합치기 허용
// 0x40 - Mouse 위치 변화를 화면 크기에 독립적으로 기록(0 ~ 65535)

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
