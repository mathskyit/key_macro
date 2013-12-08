#pragma once
#include <vector>
#include "macro_item.h"

using namespace std;


extern int cs_count;

class sMacro {
public:
	char name[256];
	long start_key;
	long stop_key;
	long options;

	vector<sMacroItem> _item;

public:
	// 매크로 실행시 필요한 변수들
	long index;
	long run_count;

private:
	long str_index;
	long remain_time;
	bool key_pressed[256];
	bool cs_enter;

	//CDeviceLock lock;

	void init_runtime_value () 
	{
		index = -1;
		str_index = 0;
		remain_time = 0;
		run_count = 0;

		for (int i=0; i<256; i++) key_pressed[i] = false;
		cs_enter = false;
	}

public:
	long ExecTime ();
	void MacroStep (int dt);

	void start (int index_ = 1) 
	{
		init_runtime_value();
		index = index_;
	}

	void stop () 
	{
		index = _item.size();
		run_count = HIWORD(options);
		for (int vk=0; vk<256; vk++) {
			UINT sc_vk = MapVirtualKey (vk, MAPVK_VK_TO_VSC);

			if (key_pressed[vk]) {
				extern bool g_keyboardMouseHookDisable;

				g_keyboardMouseHookDisable = true;
				if      (vk == VK_LBUTTON)  mouse_event (MOUSEEVENTF_LEFTUP,   0, 0, 0, 0);
				else if (vk == VK_RBUTTON)  mouse_event (MOUSEEVENTF_RIGHTUP,  0, 0, 0, 0);
				else if (vk == VK_MBUTTON)  mouse_event (MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
				else if (vk == VK_XBUTTON1) mouse_event (MOUSEEVENTF_XUP,      0, 0, XBUTTON1, 0);
				else if (vk == VK_XBUTTON2) mouse_event (MOUSEEVENTF_XUP,      0, 0, XBUTTON2, 0);
				else keybd_event ((BYTE)vk, sc_vk, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
				g_keyboardMouseHookDisable = false;
			}
		}
		if (cs_enter) {
			if (cs_count > 0) cs_count--;
			cs_enter = false;
		}
	}

	bool is_running() 
	{
		return 0 <= index && index < (int)_item.size();
	}

	sMacro () 
		: start_key(0), stop_key(0), options(0) 
	{
		init_runtime_value();
		name[0] = '\0';
	}
	
	sMacro (const sMacro &macro) 
	{
		strncpy (name, macro.name, 256);
		name[256-1] = '\0';
		start_key   = macro.start_key;
		stop_key    = macro.stop_key;
		options     = macro.options;
		_item       = macro._item;

		init_runtime_value ();
	}

	sMacro &operator = (const sMacro &macro) 
	{
		if (this != &macro) {
			strncpy (name, macro.name, 256);
			name[256-1] = '\0';
			start_key   = macro.start_key;
			stop_key    = macro.stop_key;
			options     = macro.options;
			_item       = macro._item;

			init_runtime_value ();
		}
		return *this;
	}
};

extern vector<sMacro> g_macros;

extern long ExecTime (sMacro *macro);

extern void MacroInit ();
extern void MacroTerm ();

extern void MacroStartStop (int vk);
extern void MacroStart (int id, int index = 1);
extern void MacroStop (int id);
extern bool IsMacroRunning ();
extern int  GetMacroID (const char *name);

extern void AllMacroStep (int dt);
