#include "StdAfx.h"
#include "common.h"
#include "macro_def.h"
#include "virtual_key.h"


const char *strKeyboardOption (int flags)
{
	switch(flags) {
	case 0: return "";
	case 1: return "������";
	case 2: return "����";
	case 3: return "";
	default: return "(����)";
	}
}

const char *strMouseOption (int flags)
{
	switch(flags) {
	case 0: return "";
	case 1: return "������";
	case 2: return "����";
	case 3: return "Ŭ��";
	default: return "(����)";
	}
}

const char *GetItemDescription (const sMacroItem &item)
{
	int n = 0;
	static char desc[256];
	desc[0] = '\0';

	switch (item.type) {
	case sMacroItem::NONE: 
		n = sprintf (desc, "(����)");
		break;

	case sMacroItem::KEY: {
		int index = getVkIndex(item.key.vk);
		n = sprintf (desc, "Ű���� %s ", g_vk_list[index].desc);
		if (item.key.flags) { 
			n += sprintf (desc + n, "%s", strKeyboardOption(item.key.flags));
		}
		break; }

	case sMacroItem::MOUSE: {
		n = sprintf (desc, "���콺 ");
		if (item.mouse.flags & MOUSEEVENTF_MOVE) {
			if (item.mouse.flags & MOUSEEVENTF_ABSOLUTE) {
				n += sprintf (desc + n, "������ġ(%d, %d) %s", 
					item.mouse.x, item.mouse.y, 
					(item.mouse.flags & MOUSEEVENTF_SCREEN_INDEPENDENT_POS) ? "*SI" : "");
			}
			else {
				n += sprintf (desc + n, "�����ġ(%d, %d) %s", 
					item.mouse.x, item.mouse.y, 
					(item.mouse.flags & MOUSEEVENTF_SCREEN_INDEPENDENT_POS) ? "*SI" : "");
			}
		}

		if (item.mouse.flags & MOUSEEVENTF_WHEEL) {
			n += sprintf (desc + n, "��(%d) ", (int)(short)HIWORD(item.mouse.flags));
		}

		int flags = (item.mouse.flags>>1) & 0x03;
		if (flags) {
			n += sprintf (desc + n, "�޹�ư_%s ", strMouseOption(flags));
		}
		flags = (item.mouse.flags>>3) & 0x03;
		if (flags) {
			n += sprintf (desc + n, "������ư_%s ", strMouseOption(flags));
		}
		flags = (item.mouse.flags>>5) & 0x03;
		if (flags) {
			n += sprintf (desc + n, "�߾ӹ�ư_%s ", strMouseOption(flags));
		}
		flags = (item.mouse.flags>>7) & 0x03;
		if (flags) {
			if (HIWORD(item.mouse.flags) == XBUTTON1) {
				n += sprintf (desc + n, "X1��ư_%s ", strMouseOption(flags));
			}
			if (HIWORD(item.mouse.flags) == XBUTTON2) {
				n += sprintf (desc + n, "X2��ư_%s ", strMouseOption(flags));
			}
		}
		break; }

	case sMacroItem::TIME:
		if (!(item.time.flags&0x01)) {
			n = sprintf (desc, "�ð����� %0.3f ��", (double)item.time.delay/1000.);
			
			int random_delay = item.time.flags >> 16;
			if (random_delay > 0) {
				random_delay = BOUND (random_delay, 0, 10);
				n += sprintf (desc + n, ", ���� ���� +- %d%% �߰�", random_delay*10);
			}
		}
		else {
			int flag = (item.time.flags>>1) & 0x03;

			if (flag == 3) {
				n = sprintf (desc, "�����ð� %d�� %d�� %d�� ����", 
					(item.time.delay/(60*60*1000))%24, 
					(item.time.delay/(60*1000))%60, 
					(item.time.delay/(1000))%60);
			}
			else if (flag == 2) { 
				n = sprintf (desc, "�����ð� --�� %d�� %d�� ����", 
					(item.time.delay/(60*1000))%60, 
					(item.time.delay/(1000))%60);
			}
			else if (flag == 1) {
				n = sprintf (desc, "�����ð� --�� --�� %d�� ����", 
					(item.time.delay/(1000))%60);
			}
			else if (flag == 0) {
				n = sprintf (desc, "�����ð� --�� --�� --�� ����");
			}
		}
		break;

	case sMacroItem::STRING: {
		char str[64+1];

		strncpy (str, item.str.text, 64);
		str[64] = '\0';

		if (item.str.flags & 0x01) {
			n = sprintf (desc, "���ڿ�(����) '%s'", str);
		}
		else {
			n = sprintf (desc, "���ڿ� '%s'", str);
		}
		break; }

	case sMacroItem::MACRO: {
		char str[64+1];

		strncpy (str, item.mcall.name, 64);
		str[64] = '\0';

		n = sprintf (desc, "��ũ�� ȣ�� '%s'", str);
		break; }

	case sMacroItem::CS: {
		if (item.lock.flags * 0x01) {
			n = sprintf (desc, "<<< �������౸������ ��");
		}
		else {
			n = sprintf (desc, ">>> �������౸�����κ��� ����");
		}
		break; }
	}

	return desc;
}

