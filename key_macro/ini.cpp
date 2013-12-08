#include "StdAfx.h"
#include "ini.h"

// 프로그램이 종료되기 전에 ini 파일에 보관되는 변수들.
// 프로그램 내에서 전역 변수와 같이 사용된다.

int ini_recOption	= 0x83; 

int ini_windowSX	= 100;
int ini_windowSY	= 100;  

char ini_fileName[MAX_PATH+1] = "macro.m";

int ini_macroRun		= 0;     
int ini_alwaysOnTop		= 0;      
int ini_trayAtStartup	= 0;
int ini_keyReentrance	= 0;
int ini_useMmTimer		= 0;
int ini_eventMinDelay	= 10;

int ini_macroRunEditKey		= VK_F12;
int ini_recordStartStopKey	= VK_F11;
int ini_mouseCaptureKey		= VK_F10;

int ini_recordMinMouseDistance = 10;
int ini_recordMinTimeInterval  = 300;

int ini_options		= 0x03;
int ini_runSelMacro	= 0;
int ini_macroSel	= 0;

static const char *_ini_property = "Property";
static const char *_ini_file = ".\\key_macro.ini";


static void WritePrivateProfileInt (LPCTSTR lpAppName, LPCTSTR lpKeyName, DWORD value, LPCTSTR lpFileName)
{
	char text[256];
	sprintf (text, "%d", value);
	WritePrivateProfileString(lpAppName, lpKeyName, text, lpFileName);
}

void IniLoad ()
{
	ini_recOption  = GetPrivateProfileInt (_ini_property, "RecOption", ini_recOption, _ini_file);	

	ini_windowSX   = GetPrivateProfileInt (_ini_property, "WindowSX", ini_windowSX, _ini_file);	
	ini_windowSY   = GetPrivateProfileInt (_ini_property, "WindowSY", ini_windowSY, _ini_file);	

	GetPrivateProfileString(_ini_property, "FileName", ini_fileName, ini_fileName, MAX_PATH, _ini_file);	

	ini_macroRun      = GetPrivateProfileInt (_ini_property, "MacroRun",		ini_macroRun,		_ini_file);	
	ini_alwaysOnTop   = GetPrivateProfileInt (_ini_property, "AlwaysOnTop",		ini_alwaysOnTop,	_ini_file);	
	ini_trayAtStartup = GetPrivateProfileInt (_ini_property, "TrayAtStartup",	ini_trayAtStartup,	_ini_file);	
	ini_keyReentrance = GetPrivateProfileInt (_ini_property, "KeyReentrance",	ini_keyReentrance,	_ini_file);	
	ini_useMmTimer    = GetPrivateProfileInt (_ini_property, "UseMmTimer",		ini_useMmTimer,		_ini_file);	
	ini_eventMinDelay = GetPrivateProfileInt (_ini_property, "EventMinDelay",	ini_eventMinDelay,	_ini_file);	

	ini_macroRunEditKey    = GetPrivateProfileInt (_ini_property, "MacroRunEditKey",    ini_macroRunEditKey,	_ini_file);	
	ini_recordStartStopKey = GetPrivateProfileInt (_ini_property, "RecordStartStopKey", ini_recordStartStopKey, _ini_file);	
	ini_mouseCaptureKey    = GetPrivateProfileInt (_ini_property, "MouseCaptureKey",	ini_mouseCaptureKey,	_ini_file);	

	ini_recordMinMouseDistance = GetPrivateProfileInt (_ini_property, "RecordMinMouseDistance",	ini_recordMinMouseDistance, _ini_file);	
	ini_recordMinTimeInterval  = GetPrivateProfileInt (_ini_property, "RecordMinTimeInterval",  ini_recordMinTimeInterval,  _ini_file);	

	ini_options     = GetPrivateProfileInt (_ini_property, "options",		ini_options,		_ini_file);	
	ini_runSelMacro = GetPrivateProfileInt (_ini_property, "runSelMacro",	ini_runSelMacro,	_ini_file);	
	ini_macroSel    = GetPrivateProfileInt (_ini_property, "macroSel",		ini_macroSel,		_ini_file);	
}

void IniSave ()
{
	WritePrivateProfileInt(_ini_property, "RecOption",	ini_recOption,	_ini_file);

	WritePrivateProfileInt(_ini_property, "WindowSX",	ini_windowSX,	_ini_file);
	WritePrivateProfileInt(_ini_property, "WindowSY",	ini_windowSY,	_ini_file);

	WritePrivateProfileString(_ini_property, "FileName", ini_fileName,	_ini_file);

  	WritePrivateProfileInt(_ini_property, "MacroRun",		ini_macroRun,		_ini_file);
	WritePrivateProfileInt(_ini_property, "AlwaysOnTop",	ini_alwaysOnTop,	_ini_file);
	WritePrivateProfileInt(_ini_property, "TrayAtStartup",	ini_trayAtStartup,	_ini_file);
	WritePrivateProfileInt(_ini_property, "KeyReentrance",	ini_keyReentrance,	_ini_file);
	WritePrivateProfileInt(_ini_property, "UseMmTimer",		ini_useMmTimer,		_ini_file);
	WritePrivateProfileInt(_ini_property, "EventMinDelay",	ini_eventMinDelay,	_ini_file);

	WritePrivateProfileInt(_ini_property, "MacroRunEditKey",    ini_macroRunEditKey,    _ini_file);
	WritePrivateProfileInt(_ini_property, "RecordStartStopKey", ini_recordStartStopKey, _ini_file);
	WritePrivateProfileInt(_ini_property, "MouseCaptureKey",    ini_mouseCaptureKey,    _ini_file);

	WritePrivateProfileInt(_ini_property, "RecordMinMouseDistance", ini_recordMinMouseDistance, _ini_file);
	WritePrivateProfileInt(_ini_property, "RecordMinTimeInterval",  ini_recordMinTimeInterval,	_ini_file);

	WritePrivateProfileInt(_ini_property, "options",		ini_options,		_ini_file);
	WritePrivateProfileInt(_ini_property, "runSelMacro",	ini_runSelMacro,	_ini_file);
	WritePrivateProfileInt(_ini_property, "macroSel",		ini_macroSel,		_ini_file);
}
