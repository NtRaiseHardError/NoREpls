/*
 *
 *	Software cracking challenge
 *	
 *	Version			Protection/Licensing
 *	--------        ----------------------
 *	1.0				Static, plaintext serial string
 *					Nags: Main dialog
 *	1.1				Static, (base64) encoded serial string
 *					Registered persisence: Global registered check variable
 *					Nags: Save function, about dialog, register menu item
 *	1.2				License file "License.lic", static obfuscated serial string; successful serial check routine writes license file
 *					Registered persisence: Checks license file content to verify and allow features
 *					Nags: Start and close dialog, save function, about dialog, register menu item
 *	1.3				License file "License.lic", combine user name and license
 *					Registered persistence: 
 *					Nags: Unchanged
 *	--------------------------------------
 *	2.0				License file, "License.lic", combine user name and license, successful serial check writes license file
 *					Anti: IsDebuggerPresent and LoadString + String table resource
 *					Registered persistence: 
 *					Nags: Unchanged
 *	2.1				Registry keys (String value serial key), set global registered check variable in check routine
 *					Anti: Inlined IsDebuggerPresent (PEB IsDebugged), check integrity of string table with md5 and load strings as required
 *					Registered persistence: Checks registry values and then sets global registered check variable
 *					Nags: Unchanged
 *	2.2				
 *					Anti: Enumerate window names, antidisassembly
 *					Registered persistence: 
 *					Nags: Unchanged
 *	--------------------------------------
 */

#include <Windows.h>
#include <Richedit.h>
#include <CommCtrl.h>

#include "aboutdlg.h"
#include "anti.h"
#include "helper.h"
#include "nagdlg.h"
#include "registration.h"
#include "resource.h"

#pragma comment(lib, "ComCtl32.lib")
#pragma comment(lib, "Crypt32.lib")

bool g_bRegistered = false;
WCHAR g_szLicenseName[MAX_PATH];
HINSTANCE g_hInstance = NULL;

LPWSTR lpVersionW = NULL;
LPWSTR lpTitleW = NULL;
LPWSTR lpLicenseW = NULL;
LPWSTR lpRegRequiredW = NULL;
LPWSTR lpUnRegW = NULL;
LPWSTR lpRegSuccessW = NULL;
LPWSTR lpRegThankW = NULL;
LPWSTR lpRegFailW = NULL;
LPWSTR lpInvalidSerialW = NULL;
LPWSTR lpNoSerialW = NULL;
LPWSTR lpEnterSerialW = NULL;
LPWSTR lpNoNameW = NULL;
LPWSTR lpEnterNameW = NULL;
LPWSTR lpSerialEqW = NULL;
LPWSTR lpNameEqW = NULL;
LPWSTR lpTitleFmtW = NULL;
LPWSTR lpSerialFmtW = NULL;
LPWSTR lpLicenseFmtW = NULL;
LPWSTR lpContUnregW = NULL;
LPWSTR lpWaitFmtW = NULL;

_anti a;

_init i;

void SaveText(HWND hDlg) {
	LPOPENFILENAME lpOfn = new OPENFILENAME;
	WCHAR szFileName[MAX_PATH] = L"";

	::ZeroMemory(lpOfn, sizeof(OPENFILENAME));

	lpOfn->lStructSize = sizeof(OPENFILENAME);
	lpOfn->hwndOwner = hDlg;
	lpOfn->lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	lpOfn->lpstrFile = szFileName;
	lpOfn->nMaxFile = MAX_PATH;
	lpOfn->Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_CREATEPROMPT;
	lpOfn->lpstrDefExt = L"txt";

	if (::GetSaveFileName(lpOfn)) {
		HANDLE hFile = ::CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile) {
			// get edit box text length
			DWORD dwTextLen = ::GetWindowTextLength(::GetDlgItem(hDlg, IDC_EDIT));
			// get text
			LPSTR lpText = new CHAR[dwTextLen + 1];
			::GetDlgItemTextA(hDlg, IDC_EDIT, lpText, dwTextLen + 1);

			// write to file
			DWORD dwWrite = 0;
			::WriteFile(hFile, lpText, dwTextLen + 1, &dwWrite, NULL);

			// delete text
			delete[] lpText;
		}
		::CloseHandle(hFile);
	}

	delete lpOfn;
}

void OpenText(HWND hDlg) {
	LPOPENFILENAME lpOfn = new OPENFILENAME;
	WCHAR szFileName[MAX_PATH] = L"";

	ZeroMemory(lpOfn, sizeof(OPENFILENAME));

	lpOfn->lStructSize = sizeof(OPENFILENAME);
	lpOfn->hwndOwner = hDlg;
	lpOfn->lpstrFile = szFileName;
	lpOfn->lpstrFile[0] = '\0';
	lpOfn->nMaxFile = MAX_PATH;
	lpOfn->lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	lpOfn->nFilterIndex = 1;
	lpOfn->lpstrFileTitle = NULL;
	lpOfn->nMaxFileTitle = 0;
	lpOfn->lpstrInitialDir = NULL;
	lpOfn->Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (::GetOpenFileName(lpOfn)) {
		HANDLE hFile = ::CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile) {
			// get file content length
			DWORD dwTextLen = ::GetFileSize(hFile, NULL);
			// get text
			LPSTR lpText = new CHAR[dwTextLen];
			ZeroMemory(lpText, dwTextLen);
			DWORD dwRead = 0;
			::ReadFile(hFile, lpText, dwTextLen, &dwRead, NULL);

			// write to edit box
			::SetDlgItemTextA(hDlg, IDC_EDIT, lpText);

			// clean up
			delete[] lpText;
			::CloseHandle(hFile);
		}
	}

	delete lpOfn;
}

__declspec(noinline) void DragFile(HWND hDlg, HDROP hDrop) {
	WCHAR szFileName[MAX_PATH];
	if (::DragQueryFile(hDrop, 0, szFileName, MAX_PATH)) {
		HANDLE hFile = ::CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile) {
			// get file content length
			DWORD dwTextLen = ::GetFileSize(hFile, NULL);
			// get text
			LPSTR lpText = new CHAR[dwTextLen];
			ZeroMemory(lpText, dwTextLen);
			DWORD dwRead = 0;
			::ReadFile(hFile, lpText, dwTextLen, &dwRead, NULL);

			// write to edit box
			::SetDlgItemTextA(hDlg, IDC_EDIT, lpText);

			// clean up
			delete[] lpText;
			::CloseHandle(hFile);
		}
		::DragFinish(hDrop);
	}
}

__declspec(noinline) void ChangeFont(HWND hDlg) {
	CHOOSEFONT cf;            // common dialog box structure
	static LOGFONT lf;        // logical font structure
	static DWORD rgbCurrent;  // current text color

	// Initialize CHOOSEFONT
	ZeroMemory(&cf, sizeof(cf));
	cf.lStructSize = sizeof(cf);
	cf.hwndOwner = hDlg;
	cf.lpLogFont = &lf;
	cf.rgbColors = rgbCurrent;
	cf.Flags = CF_SCREENFONTS | CF_EFFECTS;

	if (ChooseFont(&cf)) {
		HFONT hFont = CreateFontIndirect(cf.lpLogFont);
		::SendMessage(::GetDlgItem(hDlg, IDC_EDIT), WM_SETFONT, (WPARAM)hFont, TRUE);
	}
}

__declspec(noinline) void AlwaysOnTop(HWND hDlg) {
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };

	mii.fMask = MIIM_STATE;
	::GetMenuItemInfo(::GetMenu(hDlg), ID_OPTIONS_ALWAYSONTOP, FALSE, &mii);

	mii.fState ^= MFS_CHECKED;
	if (mii.fState == MFS_CHECKED)
		SetWindowPos(hDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	else
		SetWindowPos(hDlg, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	::SetMenuItemInfo(::GetMenu(hDlg), ID_OPTIONS_ALWAYSONTOP, FALSE, &mii);
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	DSTRYFUNCBOUND
	switch (uMsg) {
		case WM_INITDIALOG:
			MyLoadString(IDS_STRING_TITLEDEMOW, lpTitleW);
			::SetWindowText(hDlg, lpTitleW);
			MyUnloadString(lpTitleW);
			ANTIDIS_LINEAR
			if (!CheckLicenseRegistry()) {
				::DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG4), 0, NagDialogProc);
			} else {
				RegisterProgram(hDlg);
				::EnableMenuItem(::GetMenu(hDlg), ID_ABOUT_REGISTER, MF_GRAYED);
			}
			if (g_bRegistered) {
				WCHAR szTitle[MAX_PATH];
				MyLoadString(IDS_STRING_TITLEW, lpTitleW);
				MyLoadString(IDS_STRING_TITLEFMTW, lpTitleFmtW);
				wsprintf(szTitle, lpTitleFmtW, lpTitleW, g_szLicenseName);
				MyUnloadString(lpTitleFmtW);
				MyUnloadString(lpTitleW);
				::SetWindowText(hDlg, szTitle);
			}
			::ShowWindow(hDlg, SW_SHOW);
			return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_FILE_NEW:
					if (::GetWindowTextLengthA(::GetDlgItem(hDlg, IDC_EDIT)) > 0)
						if (::MessageBox(hDlg, L"You will lose all progress. Are you sure you want to continue?", L"Progress Detected", MB_YESNO | MB_ICONWARNING) == IDYES)
							::SetDlgItemTextA(hDlg, IDC_EDIT, "");
					return TRUE;
				case ID_FILE_SAVE:
					if (g_bRegistered)
						SaveText(hDlg);
					else {
						MyLoadString(IDS_STRING_REGREQW, lpRegRequiredW);
						MyLoadString(IDS_STRING_UNREGW, lpUnRegW);
						::MessageBox(hDlg, lpRegRequiredW, lpUnRegW, MB_OK | MB_ICONINFORMATION);
						MyUnloadString(lpUnRegW);
						MyUnloadString(lpRegRequiredW);
					}
					return TRUE;
				case ID_FILE_OPEN:
					OpenText(hDlg);
					return TRUE;
				case ID_FILE_EXIT:
					::SendMessage(hDlg, WM_CLOSE, 0, 0);
					return TRUE;
				case ID_OPTIONS_ALWAYSONTOP:
					AlwaysOnTop(hDlg);
					return TRUE;
				case ID_OPTIONS_FONT:
					if (g_bRegistered)
						ChangeFont(hDlg); 
					else {
						MyLoadString(IDS_STRING_REGREQW, lpRegRequiredW);
						MyLoadString(IDS_STRING_UNREGW, lpUnRegW);
						::MessageBox(hDlg, lpRegRequiredW, lpUnRegW, MB_OK | MB_ICONINFORMATION);
						MyUnloadString(lpUnRegW);
						MyUnloadString(lpRegRequiredW);
					}
					return TRUE;
				case ID_ABOUT_REGISTER:
					if (::DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG2), hDlg, RegisterDialogProc)) {
						MyLoadString(IDS_STRING_REGSUCCESSW, lpRegSuccessW);
						MyLoadString(IDS_STRING_REGTHANKW, lpRegThankW);
						::MessageBox(hDlg, lpRegThankW, lpRegSuccessW, MB_OK | MB_ICONINFORMATION);
						MyUnloadString(lpRegThankW);
						MyUnloadString(lpRegSuccessW);
					}
					return TRUE;
				case ID_ABOUT_ABOUT:
					::DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG3), hDlg, AboutDialogProc);
					return TRUE;
			}
			break;
		case WM_DROPFILES:
			DragFile(hDlg, (HDROP)wParam);
			return TRUE;
		case WM_SIZE: {
			RECT rc;
			::GetClientRect(hDlg, &rc);
			::MoveWindow(::GetDlgItem(hDlg, IDC_EDIT), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
			return TRUE;
		}
		case WM_CLOSE:
			if (::GetWindowTextLengthA(::GetDlgItem(hDlg, IDC_EDIT)) > 0)
				if (::MessageBox(hDlg, L"You will lose all progress. Are you sure you want to continue?", L"Progress Detected", MB_YESNO | MB_ICONWARNING) == IDNO)
					return TRUE;
			::DestroyWindow(hDlg);
			return TRUE;
		case WM_DESTROY:
			if (!g_bRegistered)
				::DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG4), 0, NagDialogProc);
			::PostQuitMessage(0);
			return TRUE;
	}

	return FALSE;
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	HWND hDlg = ::CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc, 0);

	MSG msg;
	BOOL ret;
	while ((ret = ::GetMessage(&msg, 0, 0, 0)) != 0) {
		if (ret == -1)
			return -1;

		if (!IsDialogMessage(hDlg, &msg)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	return 0;
}
