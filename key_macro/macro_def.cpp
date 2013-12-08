#include "stdafx.h"
#include "macro_def.h"
#include "ini.h"

extern bool g_keyboardMouseHookDisable;

static bool _running = false;
int cs_count = 0;
vector<sMacro> g_macros;


long sMacro::ExecTime ()
{
	long sum = ini_eventMinDelay*(_item.size() - 1);

	for (unsigned int i=0; i<_item.size(); ++i) {
		sMacroItem &mi = _item[i];

		if (mi.type == sMacroItem::TIME) {
			if (mi.time.flags == 0) {
				sum += mi.time.delay;
			}
		}
		else if (mi.type == sMacroItem::STRING) {
			sum += ini_eventMinDelay*(mi.str.length - 1);
		}
		else if (mi.type == sMacroItem::MACRO) {
			int id = GetMacroID (mi.mcall.name);
			if (0 <= id) {
				sum += g_macros[id].ExecTime ();
			}
		}
	}
	return sum;
}

void sMacro::MacroStep (int dt)
{
	sMacroItem &mi = _item[index];

	switch (mi.type) {
	case sMacroItem::KEY:
		mi.key.Step (key_pressed);
		index ++;
		break;
	case sMacroItem::MOUSE: 
		mi.mouse.Step (key_pressed);
		index ++;
		break; 
	case sMacroItem::TIME:
		if (mi.time.Step (remain_time, dt)) {
			index++;
		}
		break;
	case sMacroItem::STRING:
		if (mi.str.Step (str_index)) {
			index++;
		}
		break;
	case sMacroItem::MACRO:
		if (mi.mcall.Step (str_index)) {
			index ++;
		}
		break;
	case sMacroItem::CS:
		if(mi.lock.Step (cs_enter)) {
			index ++;
		}
		break;
	default:
		index ++;
		break;
	}
}


void AllMacroStep (int dt)
{
	if (!_running) return;

	for (unsigned int i=0; i<g_macros.size(); ++i) {
		sMacro &m = g_macros[i];

		if (0 < m.index && m.index < (int)m._item.size()) {
			m.MacroStep (dt);
			if ((int)m._item.size() <= m.index) {
				m.run_count++;

				if (HIWORD(m.options) == 0 || m.run_count < HIWORD(m.options)) {
					m.index = 1; // 처음부터 재시작
				}
			}
		}
	}
}

void MacroInit ()
{
	_running = true;
	cs_count = 0;

	for (unsigned int i=0; i<g_macros.size(); ++i) {
		sMacro &m = g_macros[i];

		// 매크로 실행과 함께 자동 시작이 설정되어 있음.
		// 매크로를 실행한다.
		if (m.options & 0x02) m.start (1);
	}
}

void MacroTerm ()
{
	_running = false;
	cs_count = 0;

	for (unsigned int i=0; i<g_macros.size(); ++i) {
		g_macros[i].stop ();
	}

	// 모든 눌린 키를 해제 한다.
	GetKeyState(0); // Windows - Dev Center 도움말에 의하면... 
	// GetKeyboardState()를 실행하기 전에 GetKeyState() 를 호출해야 정상 동작한다고 함.

	BYTE keyState[256];
	GetKeyboardState((LPBYTE)&keyState);

	for (int vk=0; vk<256; vk++) {
		if (keyState[vk] & 1){
			UINT sc_vk = MapVirtualKey (vk, MAPVK_VK_TO_VSC);

			keybd_event (vk, sc_vk, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
		}
	}
}

void MacroStartStop (int vk)
{
	if (!_running) return;

	for (unsigned int i=0; i<g_macros.size(); ++i) {
		sMacro &m = g_macros[i];

		// 매크로가 비실행상태에서 시작키가 눌렸을 때, 첫 _item부터 시작한다.
		if (!m.is_running() && m.start_key == vk)     m.start (1);
		// 매크로가 실행상태에서 중지키가 눌렸을 때, 중지 상태로 만든다.
		else if (m.is_running() && m.stop_key == vk)  m.stop ();
	}
}

void MacroStart (int id, int index)
{
	if (0 <= id && id < (int)g_macros.size()) {
		sMacro &m = g_macros[id];

		// 매크로가 비실행 상태이거나 실행종료 상태이면 첫 _item부터 시작한다.
		if (!m.is_running()) m.start (index);
	}
}

void MacroStop (int id)
{
	if (0 <= id && id < (int)g_macros.size()) {
		sMacro &m = g_macros[id];

		// 현재 실행 중이면 실행 단계를 끝으로 보낸다.
		if (m.is_running()) m.stop ();
	}
}

bool IsMacroRunning ()
{
	return _running;
}

int GetMacroID (const char *name)
{
	for (unsigned int i=0; i<g_macros.size(); ++i) {
		if (strcmp (g_macros[i].name, name) == 0) {
			return i;
		}
	}
	return -1;
}
