#include "stdafx.h"
#include "macro_def.h"
#include "resource.h"
#include "DialogKeyboard.h"
#include "DialogMouse.h"
#include "DialogMacroRec.h"
#include "DialogMacroEdit.h"
#include "key_macroDlg.h"

extern CDialogKeyboard  *g_keyDlg;
extern CDialogMouse     *g_mouseDlg;
extern CDialogMacroRec  *g_recDlg;
extern CDialogMacroEdit *g_editDlg;
extern Ckey_macroDlg    *g_macroDlg;

bool g_keyboardMouseHookDisable = false;

static HHOOK hKeybdHook = NULL; 
static HHOOK hMouseHook = NULL;

#include "mmtimer.h"

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (0 <= nCode && !g_keyboardMouseHookDisable) {
		KBDLLHOOKSTRUCT *stat = (KBDLLHOOKSTRUCT *)lParam;
		// TRACE ("K %d %X : %02X %02X %02X %d\n", nCode, wParam, (int)stat->flags, stat->scanCode, stat->vkCode, stat->time);

		int vk = 0;
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) { vk = stat->vkCode;              }
		if (wParam == WM_KEYUP   || wParam == WM_SYSKEYUP)   { vk = stat->vkCode | VK_KEY_UP;  }

		if (g_keyDlg)        g_keyDlg   ->OnKeyEvent (vk);
		else if (g_editDlg)  g_editDlg  ->OnKeyMouseEvent (vk, true);
		else if (g_recDlg)   g_recDlg   ->OnKeyboardEvent (vk, wParam, stat);
		else if (g_macroDlg) g_macroDlg ->OnKeyMouseEvent (vk, true);
	}
	return CallNextHookEx(hKeybdHook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (0 <= nCode && !g_keyboardMouseHookDisable) {
		MSLLHOOKSTRUCT *stat = (MSLLHOOKSTRUCT *)lParam;
		// TRACE ("M %d %X : %X (%d, %d) %X %d\n", nCode, wParam, (int)stat->flags, stat->pt.x, stat->pt.y, stat->mouseData, stat->time);

		int vk = 0;
		if      (wParam == WM_LBUTTONDOWN) { vk = VK_LBUTTON;				}
		else if (wParam == WM_LBUTTONUP)   { vk = VK_LBUTTON | VK_KEY_UP;	}
		else if (wParam == WM_RBUTTONDOWN) { vk = VK_RBUTTON;				}
		else if (wParam == WM_RBUTTONUP)   { vk = VK_RBUTTON | VK_KEY_UP;	}
		else if (wParam == WM_MBUTTONDOWN) { vk = VK_MBUTTON;				}
		else if (wParam == WM_MBUTTONUP)   { vk = VK_MBUTTON | VK_KEY_UP;	}
		else if (wParam == WM_MOUSEWHEEL)  { vk = ((short)HIWORD(stat->mouseData) > 0) ? 0x0A : 0x0B; }
		else if (wParam == WM_XBUTTONDOWN) {
			if (HIWORD(stat->mouseData) & XBUTTON1) { vk = VK_XBUTTON1;  }
			if (HIWORD(stat->mouseData) & XBUTTON2) { vk = VK_XBUTTON2;  }
		}
		else if (wParam == WM_XBUTTONUP) {
			if (HIWORD(stat->mouseData) & XBUTTON1) { vk = VK_XBUTTON1 | VK_KEY_UP;  }
			if (HIWORD(stat->mouseData) & XBUTTON2) { vk = VK_XBUTTON2 | VK_KEY_UP;  }
		}

		if (g_mouseDlg)      g_mouseDlg ->OnMouseEvent    (stat->pt.x, stat->pt.y);
		else if (g_editDlg)  g_editDlg  ->OnKeyMouseEvent (vk, false);
		else if (g_recDlg)   g_recDlg   ->OnMouseEvent    (vk, wParam, stat);
		else if (g_macroDlg) g_macroDlg ->OnKeyMouseEvent (vk, false);
	}

	return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

void InstallHook ()
{
	HINSTANCE hInst = AfxGetInstanceHandle ();

	hKeybdHook = SetWindowsHookEx (WH_KEYBOARD_LL, LowLevelKeyboardProc, hInst, 0); 
	hMouseHook = SetWindowsHookEx (WH_MOUSE_LL,    LowLevelMouseProc,    hInst, 0);
}

void UninstallHook ()
{
	UnhookWindowsHookEx(hKeybdHook);
	UnhookWindowsHookEx(hMouseHook);

	hKeybdHook = NULL;
	hMouseHook = NULL;
}
