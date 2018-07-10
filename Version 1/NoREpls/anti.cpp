#include <Shlwapi.h>

#include "anti.h"
#include "helper.h"
#include "md5.h"

#pragma comment(lib, "Shlwapi.lib")

#define MD5_LEN 32

LPWSTR lpDbgWindow = NULL;

LPWSTR DebuggerNames[] = {
	L"odbg",
	L"ollydbg",
	L"ida",
	L"immunity",
	L"softice",
	L"radare",
	L"gdb",
	L"x32dbg"
	/* needs more names */
};

LPWSTR MonitoringToolNames[] = {
	L"procmon",
	L"processhacker",
	L"procexp",
	L"wireshark"
	/* needs more names */
};

BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCWSTR lpszType, LPCWSTR lpszName, LONG_PTR lParam) {
	DSTRYFUNCBOUND;
	HRSRC hRsrc = ::FindResource(hModule, lpszName, lpszType);
	if (hRsrc) {
		DWORD dwSize = ::SizeofResource(hModule, hRsrc);
		HGLOBAL hResData = ::LoadResource(hModule, hRsrc);
		if (hResData) {
			LPVOID lpResource = ::LockResource(hResData);
			if (lpResource) {
				std::string digest = md5(lpResource, dwSize);
				if (strncmp(digest.c_str(), "f9e5a571aedde949df612d885d7d2472", MD5_LEN) && strncmp(digest.c_str(), "d11a7dd370cb977ff0a9756b34dda9af", MD5_LEN))
					DIE;
			}
		}
	}

	return TRUE;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	DSTRYFUNCBOUND;
	int nLength = ::GetWindowTextLength(hwnd);
	LPWSTR lpWindowText = new WCHAR[nLength + 1];

	ANTIDIS_LINEAR;
	if (nLength > 0 && IsWindowVisible(hwnd)) {
		::GetWindowText(hwnd, lpWindowText, nLength + 1);
		for (int i = 0; i < 8; i++)
			if (::StrStrI(lpWindowText, DebuggerNames[i]) != NULL) {
				DIE;
			}
		for (int i = 0; i < 4; i++)
			if (::StrStrI(lpWindowText, MonitoringToolNames[i]) != NULL) {
				DIE;
			}
	}

	delete[] lpWindowText;

	return TRUE;
}

void CheckDebuggerWindows() {
	ANTIDIS_LINEAR;
	::EnumWindows(EnumWindowsProc, 0);
}