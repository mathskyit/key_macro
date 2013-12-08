#include "StdAfx.h"
#include "common.h"
#include "macro_def.h"
#include "virtual_key.h"


const char *strKeyboardOption (int flags)
{
	switch(flags) {
	case 0: return "";
	case 1: return "누르기";
	case 2: return "떼기";
	case 3: return "";
	default: return "(오류)";
	}
}

const char *strMouseOption (int flags)
{
	switch(flags) {
	case 0: return "";
	case 1: return "누르기";
	case 2: return "떼기";
	case 3: return "클릭";
	default: return "(오류)";
	}
}

const char *GetItemDescription (const sMacroItem &item)
{
	int n = 0;
	static char desc[256];
	desc[0] = '\0';

	switch (item.type) {
	case sMacroItem::NONE: 
		n = sprintf (desc, "(시작)");
		break;

	case sMacroItem::KEY: {
		int index = getVkIndex(item.key.vk);
		n = sprintf (desc, "키보드 %s ", g_vk_list[index].desc);
		if (item.key.flags) { 
			n += sprintf (desc + n, "%s", strKeyboardOption(item.key.flags));
		}
		break; }

	case sMacroItem::MOUSE: {
		n = sprintf (desc, "마우스 ");
		if (item.mouse.flags & MOUSEEVENTF_MOVE) {
			if (item.mouse.flags & MOUSEEVENTF_ABSOLUTE) {
				n += sprintf (desc + n, "절대위치(%d, %d) %s", 
					item.mouse.x, item.mouse.y, 
					(item.mouse.flags & MOUSEEVENTF_SCREEN_INDEPENDENT_POS) ? "*SI" : "");
			}
			else {
				n += sprintf (desc + n, "상대위치(%d, %d) %s", 
					item.mouse.x, item.mouse.y, 
					(item.mouse.flags & MOUSEEVENTF_SCREEN_INDEPENDENT_POS) ? "*SI" : "");
			}
		}

		if (item.mouse.flags & MOUSEEVENTF_WHEEL) {
			n += sprintf (desc + n, "휠(%d) ", (int)(short)HIWORD(item.mouse.flags));
		}

		int flags = (item.mouse.flags>>1) & 0x03;
		if (flags) {
			n += sprintf (desc + n, "왼버튼_%s ", strMouseOption(flags));
		}
		flags = (item.mouse.flags>>3) & 0x03;
		if (flags) {
			n += sprintf (desc + n, "오른버튼_%s ", strMouseOption(flags));
		}
		flags = (item.mouse.flags>>5) & 0x03;
		if (flags) {
			n += sprintf (desc + n, "중앙버튼_%s ", strMouseOption(flags));
		}
		flags = (item.mouse.flags>>7) & 0x03;
		if (flags) {
			if (HIWORD(item.mouse.flags) == XBUTTON1) {
				n += sprintf (desc + n, "X1버튼_%s ", strMouseOption(flags));
			}
			if (HIWORD(item.mouse.flags) == XBUTTON2) {
				n += sprintf (desc + n, "X2버튼_%s ", strMouseOption(flags));
			}
		}
		break; }

	case sMacroItem::TIME:
		if (!(item.time.flags&0x01)) {
			n = sprintf (desc, "시간지연 %0.3f 초", (double)item.time.delay/1000.);
			
			int random_delay = item.time.flags >> 16;
			if (random_delay > 0) {
				random_delay = BOUND (random_delay, 0, 10);
				n += sprintf (desc + n, ", 랜덤 지연 +- %d%% 추가", random_delay*10);
			}
		}
		else {
			int flag = (item.time.flags>>1) & 0x03;

			if (flag == 3) {
				n = sprintf (desc, "지연시각 %d시 %d분 %d초 까지", 
					(item.time.delay/(60*60*1000))%24, 
					(item.time.delay/(60*1000))%60, 
					(item.time.delay/(1000))%60);
			}
			else if (flag == 2) { 
				n = sprintf (desc, "지연시각 --시 %d분 %d초 까지", 
					(item.time.delay/(60*1000))%60, 
					(item.time.delay/(1000))%60);
			}
			else if (flag == 1) {
				n = sprintf (desc, "지연시각 --시 --분 %d초 까지", 
					(item.time.delay/(1000))%60);
			}
			else if (flag == 0) {
				n = sprintf (desc, "지연시각 --시 --분 --초 까지");
			}
		}
		break;

	case sMacroItem::STRING: {
		char str[64+1];

		strncpy (str, item.str.text, 64);
		str[64] = '\0';

		if (item.str.flags & 0x01) {
			n = sprintf (desc, "문자열(랜덤) '%s'", str);
		}
		else {
			n = sprintf (desc, "문자열 '%s'", str);
		}
		break; }

	case sMacroItem::MACRO: {
		char str[64+1];

		strncpy (str, item.mcall.name, 64);
		str[64] = '\0';

		n = sprintf (desc, "매크로 호출 '%s'", str);
		break; }

	case sMacroItem::CS: {
		if (item.lock.flags * 0x01) {
			n = sprintf (desc, "<<< 독립실행구간으로 들어감");
		}
		else {
			n = sprintf (desc, ">>> 독립실행구간으로부터 나옴");
		}
		break; }
	}

	return desc;
}

