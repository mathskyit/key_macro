#include "stdafx.h"
#include "macro_item.h"
#include "macro_def.h"
#include "ini.h"
#include "Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern bool g_keyboardMouseHookDisable;

void sKey::Step (bool key_pressed[256])
{
	if (flags & 1) {
		UINT sc_vk = MapVirtualKey ((BYTE)vk, MAPVK_VK_TO_VSC);

		g_keyboardMouseHookDisable = true;
		keybd_event ((BYTE)vk, sc_vk, KEYEVENTF_EXTENDEDKEY, 0);
		g_keyboardMouseHookDisable = false;

		key_pressed[(BYTE)vk] = true;
	}
	if (flags & 2) {
		UINT sc_vk = MapVirtualKey ((BYTE)vk, MAPVK_VK_TO_VSC);

		g_keyboardMouseHookDisable = true;
		keybd_event ((BYTE)vk, sc_vk, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
		g_keyboardMouseHookDisable = false;

		key_pressed[(BYTE)vk] = false;
	}
	if (ini_keyReentrance) {
		// 여기서 매크로 시작 중지 키를 체크하고 호출한다.
		if (flags & 1) MacroStartStop ((BYTE)vk);
		if (flags & 2) MacroStartStop ((BYTE)vk | VK_KEY_UP);
	}
}

BYTE sMouse::GetMouseVK (unsigned long button)
{
	if      (button & MOUSEEVENTF_LEFTDOWN)   return VK_LBUTTON;
	else if (button & MOUSEEVENTF_LEFTUP)     return VK_LBUTTON;
	else if (button & MOUSEEVENTF_RIGHTDOWN)  return VK_RBUTTON;
	else if (button & MOUSEEVENTF_RIGHTUP)    return VK_RBUTTON;
	else if (button & MOUSEEVENTF_MIDDLEDOWN) return VK_MBUTTON;
	else if (button & MOUSEEVENTF_MIDDLEUP)   return VK_MBUTTON;
	else if (button & MOUSEEVENTF_XDOWN) {
		if (HIWORD(button) & XBUTTON1) return VK_XBUTTON1;
		if (HIWORD(button) & XBUTTON2) return VK_XBUTTON2;
	}
	else if (button & MOUSEEVENTF_XUP) {
		if (HIWORD(button) & XBUTTON1) return VK_XBUTTON1;
		if (HIWORD(button) & XBUTTON2) return VK_XBUTTON2;
	}
	return 0;
}

void sMouse::Step (bool key_pressed[256])
{
	int mx = x;
	int my = y;

	if (flags&MOUSEEVENTF_ABSOLUTE) {
		if (flags&MOUSEEVENTF_SCREEN_INDEPENDENT_POS);
		else ScreenIndependentMousePos (mx, my);
	}
	else {
		// 상대적인 움직임일 때는 마우스 커서의 현재 위치에 상대 위치를 더해준다.
		POINT cp;
		GetCursorPos(&cp);

		mx = cp.x + mx;
		my = cp.y + my;
		
		ScreenIndependentMousePos (mx, my);
	}

	BYTE vk = GetMouseVK (flags);
	
	DWORD flag1 = flags & (
		MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_RIGHTDOWN | 
		MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_XDOWN | 
		MOUSEEVENTF_MOVE | MOUSEEVENTF_WHEEL);

	if (flag1 & MOUSEEVENTF_MOVE) flag1 |= MOUSEEVENTF_ABSOLUTE;
	if (flag1) {
		g_keyboardMouseHookDisable = true;
		mouse_event (flag1, mx, my, (short)HIWORD(flags), 0);
		g_keyboardMouseHookDisable = false;
	
		key_pressed[vk] = true;
	}
	
	DWORD flag2 = flags & (
		MOUSEEVENTF_LEFTUP | MOUSEEVENTF_RIGHTUP | 
		MOUSEEVENTF_MIDDLEUP | MOUSEEVENTF_XUP );
	
	if (flag2) {
		g_keyboardMouseHookDisable = true;
		mouse_event (flag2, mx, my, 0, 0);
		g_keyboardMouseHookDisable = false;
		
		key_pressed[vk] = false;
	}

	if (ini_keyReentrance) {
		// 여기서 매크로 시작 중지 키를 체크하고 호출한다.
		if (flag1) MacroStartStop (vk);
		if (flag2) MacroStartStop (vk | VK_KEY_UP);
	}
}

int sTime::GetTimeDelay (int flag, int delay)
{
	time_t t_ = time(NULL);
	struct tm *t = localtime (&t_);

	if (flag == 3) {
		int tt = t->tm_hour*60*60*1000 + t->tm_min*60*1000 + t->tm_sec*1000;
		return (delay > tt) ? delay - tt : 24*60*60*1000 + (delay - tt);
	}
	else if (flag == 2) {
		int tt = t->tm_min*60*1000 + t->tm_sec*1000;
		return (delay > tt) ? delay - tt : 60*60*1000 + (delay - tt);
	}
	else if (flag == 1) {
		int tt = t->tm_sec*1000;
		return (delay > tt) ? delay - tt : 60*1000 + (delay - tt);
	}
	else {
		return 1000;
	}
}

int sTime::GetRandomDelay (int delay, int random)
{
	if (random > 0) {
		double random_delay = (rand()%(2*random+1) - random)/1000.;
		return delay + (int)(delay*random_delay);
	}
	else {
		return delay;
	}
}

bool sTime::Step (long &remain_time, int dt)
{
	if (remain_time == 0) {
		if (flags & 1) {
			remain_time = GetTimeDelay ((flags>>1)&0x03, delay);
		}
		else {
			int random_delay = BOUND (flags >> 16, 0, 10);
			remain_time = GetRandomDelay (delay, random_delay*100); // random_delay: 0 ~ 100 %
		}
	}
	else {
		remain_time -= dt;
	}
	
	if (remain_time <= 0) { 
		remain_time = 0;
		return true;
	}
	return false;
}

BYTE sString::GetVK (unsigned char ch, bool *shift_key)
{
	if ('a' <= ch && ch <= 'z') {
		*shift_key = false;
		return ch + 'A' - 'a';
	}
	if ('A' <= ch && ch <= 'Z') {
		*shift_key = true;
		return ch;
	}
	switch (ch) {
	case '`': *shift_key = false; return 0xC0;
	case '~': *shift_key = true;  return 0xC0;
	case '!': *shift_key = true;  return 0x31;
	case '@': *shift_key = true;  return 0x32;
	case '#': *shift_key = true;  return 0x33;
	case '$': *shift_key = true;  return 0x34;
	case '%': *shift_key = true;  return 0x35;
	case '^': *shift_key = true;  return 0x36;
	case '&': *shift_key = true;  return 0x37;
	case '*': *shift_key = true;  return 0x38;
	case '(': *shift_key = true;  return 0x39;
	case ')': *shift_key = true;  return 0x30;
	case '-': *shift_key = false; return 0xBD;
	case '_': *shift_key = true;  return 0xBD;
	case '=': *shift_key = false; return 0xBB;
	case '+': *shift_key = true;  return 0xBB;
	case '\\':*shift_key = false; return 0xDC;
	case '|': *shift_key = true;  return 0xDC;
	case '[': *shift_key = false; return 0xDB;
	case '{': *shift_key = true;  return 0xDB;
	case ']': *shift_key = false; return 0xDD;
	case '}': *shift_key = true;  return 0xDD;
	case ';': *shift_key = false; return 0xBA;
	case ':': *shift_key = true;  return 0xBA;
	case '\'':*shift_key = false; return 0xDE;
	case '"': *shift_key = true;  return 0xDE;
	case ',': *shift_key = false; return 0xBC;
	case '<': *shift_key = true;  return 0xBC;
	case '.': *shift_key = false; return 0xBE;
	case '>': *shift_key = true;  return 0xBE;
	case '/': *shift_key = false; return 0xBF;
	case '?': *shift_key = true;  return 0xBF;
	}

	*shift_key = false; 
	return ch;
}

bool sString::Step (long &str_index)
{
	if (str_index < length) {
		BYTE ch = text[str_index];

		if (flags & 0x01) {
			if      (ch == '#') ch = GetUpperRand ();
			else if (ch == '*') ch = GetLowerRand ();
			else if (ch == '?') ch = GetNumberRand ();
		}

		if ((ch&0x80) != 0x80) {
			bool shift_key = false;
			BYTE vk = GetVK (ch, &shift_key);
			UINT sc_vk = MapVirtualKey (vk, MAPVK_VK_TO_VSC);
			UINT sc_lshift = MapVirtualKey (VK_LSHIFT, MAPVK_VK_TO_VSC);
			
			g_keyboardMouseHookDisable = true;
			if (shift_key) keybd_event (VK_LSHIFT, sc_lshift, KEYEVENTF_EXTENDEDKEY, 0);
			keybd_event (vk, sc_vk, KEYEVENTF_EXTENDEDKEY, 0);
			keybd_event (vk, sc_vk, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			if (shift_key) keybd_event (VK_LSHIFT, sc_lshift, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			g_keyboardMouseHookDisable = false;
		}
		str_index ++;
		return false;
	}
	else {
		str_index = 0;
		return true;
	}
}

bool sMacroCall::Step (long &str_index)
{
	if (str_index == 0) {
		int id = GetMacroID (name);
		if (0 <= id) {
			if (!g_macros[id].is_running ()) {
				g_macros[id].start();
			}
		}
		str_index = 1;
	}
	else {
		int id = GetMacroID (name);
		if (0 <= id) {
			if (!g_macros[id].is_running ()) {
				str_index = 0;
				return true;
			}
		}
		else {
			str_index = 0;
			return true;
		}
	}
	return false;
}

bool sLock::Step (bool &cs_enter)
{
	if(flags & 0x01) {
		// Macro의 Critical Section으로 진입
		if (cs_count == 0) { 
			cs_count++;
			cs_enter = true;
			return true;
		}
		else if (cs_enter) {
			// 이미 Critical Section으로 들어와 있는 경우는 무시하고 지나간다.
			return true;
		}
	}
	else {
		// Macro의 Critical Section을 탈출
		if (cs_count > 0 && cs_enter) {
			cs_enter = false;
			cs_count--;
		}
		return true;
	}
	return false;
}

void sMacroItem::Delete () 
{
	if      (type == sMacroItem::STRING) delete [] str.text;
	else if (type == sMacroItem::MACRO)  delete [] mcall.name;
}

void sMacroItem::Copy (const sMacroItem &mi) 
{
	type = mi.type;
	memcpy (&data[0], &mi.data[0], 3*sizeof(long));

	if (type == sMacroItem::STRING) {
		str.text = new char [str.length + 1];
		memcpy (str.text, mi.str.text, str.length + 1);
	}
	else if (type == sMacroItem::MACRO) {
		mcall.name = new char [256];
		memcpy (mcall.name, mi.mcall.name, 256);
	}
}
