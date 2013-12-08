#include "stdafx.h"
#include "etc.h"

void SendRestartMessage (char *WindowClass, char *Title)
{
	HWND hPrevWnd = FindWindow(WindowClass, Title);
	if (!hPrevWnd) return;

	// Command line argument를 hPrevWnd 윈도우로 전송한다.
	char *cmd = GetCommandLine();
	int n_cmd = strlen(cmd)+1;
	char *buff = (char *)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, n_cmd);

	strcpy (buff, cmd);

	COPYDATASTRUCT tip;
	tip.dwData = 0;
	tip.cbData = n_cmd;
	tip.lpData = buff;
	
	SendMessage (hPrevWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&tip);
	
	::HeapFree(::GetProcessHeap(), 0, buff);

	PostMessage (hPrevWnd, WM_RESTART, 0, 0);
	FlashWindow (hPrevWnd, TRUE);
}

bool ApplicationAlreadyExist (char *WindowClass, char *Title)
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, Title);   // Create mutex
	switch(GetLastError()) {
	case ERROR_SUCCESS:
		break;
	case ERROR_ALREADY_EXISTS:
		SendRestartMessage (WindowClass, Title);
		return true;
	default:
		return false;
	}
	return false;
}

PCHAR* CommandLineToArgvA(PCHAR CmdLine, int* _argc)
{
	PCHAR* argv;
	PCHAR  _argv;
	ULONG  len;
	ULONG  argc;
	CHAR   a;
	ULONG  i, j;

	BOOLEAN  in_QM;
	BOOLEAN  in_TEXT;
	BOOLEAN  in_SPACE;

	len = strlen(CmdLine);
	i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

	argv = (PCHAR*)GlobalAlloc(GMEM_FIXED, i + (len+2)*sizeof(CHAR));
	_argv = (PCHAR)(((PUCHAR)argv)+i);

	argc = 0;
	argv[argc] = _argv;
	in_QM = FALSE;
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	i = 0;
	j = 0;

	while( a = CmdLine[i] ) {
		if(in_QM) {
			if(a == '\"') {
				in_QM = FALSE;
			} else {
				_argv[j] = a;
				j++;
			}
		} 
		else {
			switch(a) {
			case '\"':
				in_QM = TRUE;
				in_TEXT = TRUE;
				if(in_SPACE) {
					argv[argc] = _argv+j;
					argc++;
				}
				in_SPACE = FALSE;
				break;
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				if(in_TEXT) {
					_argv[j] = '\0';
					j++;
				}
				in_TEXT = FALSE;
				in_SPACE = TRUE;
				break;
			default:
				in_TEXT = TRUE;
				if(in_SPACE) {
					argv[argc] = _argv+j;
					argc++;
				}
				_argv[j] = a;
				j++;
				in_SPACE = FALSE;
				break;
			}
		}
		i++;
	}
	_argv[j] = '\0';
	argv[argc] = NULL;

	(*_argc) = argc;
	return argv;
}

