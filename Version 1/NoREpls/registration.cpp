#include <Windows.h>

#include "anti.h"
#include "helper.h"
#include "md5.h"
#include "registration.h"
#include "resource.h"
#include "str_obfuscator.hpp"

//__declspec(noinline) bool Base64Encode(LPSTR& lpOutput, LPSTR lpInput) {
//	DWORD nDestinationSize;
//	if (CryptBinaryToStringA(reinterpret_cast<const BYTE*> (lpInput), wcslen(lpInput), CRYPT_STRING_BASE64, NULL, &nDestinationSize)) {
//		LPSTR pszDestination = new CHAR[nDestinationSize];
//		if (pszDestination) {
//			if (CryptBinaryToStringA(reinterpret_cast<const BYTE*> (lpInput), wcslen(lpInput), CRYPT_STRING_BASE64, pszDestination, &nDestinationSize)) {
//				lpOutput = pszDestination;
//				return true;
//			}
//		}
//	}
//
//	return false;
//}

__declspec(noinline) void RegisterProgram(HWND hDlg) {
	DSTRYFUNCBOUND
	ANTIDIS_LINEAR
	ISDBGPRSNT
	g_bRegistered = true;
	::EnableWindow(::GetDlgItem(hDlg, IDC_STATIC_DEMO), FALSE);
	MyLoadString(IDS_STRING_TITLEW, lpTitleW);
	::SetWindowText(::GetParent(hDlg), lpTitleW);
	MyUnloadString(lpTitleW);
	::EnableMenuItem(::GetMenu(::GetParent(hDlg)), ID_ABOUT_REGISTER, MF_GRAYED);
}

bool SerialAlgorithm(LPWSTR lpName, LPWSTR lpSerial) {
	DSTRYFUNCBOUND
	int iRes = 0;
	if (wcslen(lpName) > wcslen(lpSerial)) {
		for (int i = 0; i < wcslen(lpName); i+=2)
			iRes += ((((69 * iRes) + __LINE__) * i) >> __COUNTER__) ^ lpName[i % wcslen(lpName)];
	} else {
		for (int i = 0; i < wcslen(lpSerial); i+=2)
			iRes += ((((69 * iRes) + __LINE__) * i) >> __COUNTER__) ^ lpName[i % wcslen(lpName)];
	}
	ANTIDIS_LINEAR
#ifndef _DEBUG
	return (iRes % 100 > 0x69);
#else
	return true;
#endif // !_DEBUG

}

__declspec(noinline) static bool CheckSerial(LPWSTR lpName, LPWSTR lpSerial) {
	BOOL bRet = false;
	int nPart[4];

	ANTIDIS_LINEAR
	MyLoadString(IDS_STRING_SERIALFMTW, lpSerialFmtW);
	if (swscanf(lpSerial, lpSerialFmtW, &nPart[0], &nPart[1], &nPart[2], &nPart[3]) == 4)
		if (SerialAlgorithm(lpName, lpSerial)) {
			wsprintf(g_szLicenseName, L"%s", lpName);
			bRet = true;
		}
	MyUnloadString(lpSerialFmtW);

	ANTIDIS_LINEAR
	return bRet;
}

__declspec(noinline) bool GetLicense(HWND hDlg) {
	bool bRet = false;
	WCHAR szName[MAX_PATH];
	WCHAR szSerial[MAX_PATH];
	::GetDlgItemText(hDlg, IDC_USERNAME, szName, MAX_PATH);
	::GetDlgItemText(hDlg, IDC_SERIAL, szSerial, MAX_PATH);

	if (wcslen(szSerial) == 0) {
		MyLoadString(IDS_STRING_NOSERW, lpNoSerialW);
		MyLoadString(IDS_STRING_ENTERSERW, lpEnterSerialW);
		::MessageBox(hDlg, lpEnterSerialW, lpNoSerialW, MB_OK | MB_ICONINFORMATION);
		MyUnloadString(lpEnterSerialW);
		MyUnloadString(lpNoSerialW);
		return bRet;
	} else if (wcslen(szName) == 0) {
		MyLoadString(IDS_STRING_NONAMEW, lpNoNameW);
		MyLoadString(IDS_STRING_ENTERSERW, lpEnterSerialW);
		::MessageBox(hDlg, lpEnterNameW, lpNoNameW, MB_OK | MB_ICONINFORMATION);
		MyUnloadString(lpEnterSerialW);
		MyUnloadString(lpNoSerialW);
		return bRet;
	}

	ANTIDIS_LINEAR
	bRet = CheckSerial(szName, szSerial);

	// write to registry
	if (bRet) {
		// create key
		HKEY hKey = NULL;
		if (::RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\NoREpls", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
			// set name value
			if (::RegSetValueEx(hKey, L"Name", 0, REG_SZ, (LPBYTE)szName, wcslen(szName) * sizeof(WCHAR)) == ERROR_SUCCESS)
				// set name value
				if (::RegSetValueEx(hKey, L"SN", 0, REG_SZ, (LPBYTE)szSerial, wcslen(szSerial) * sizeof(WCHAR)) == ERROR_SUCCESS)
			::RegCloseKey(hKey);
		}
	}

	ANTIDIS_LINEAR
	return bRet;
}

__declspec(noinline) bool CheckLicenseRegistry() {
	DSTRYFUNCBOUND
	bool bRet = false;
	// get handle to registry key
	HKEY hKey = NULL;
	if (::RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\NoREpls", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_QUERY_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
		WCHAR szName[50];
		WCHAR szSerial[50];
		DWORD dwBufSize = sizeof(szName);

		wmemset(szName, '\0', sizeof(szName) / sizeof(WCHAR));
		wmemset(szSerial, '\0', sizeof(szSerial) / sizeof(WCHAR));

		// get name value
		if (::RegQueryValueExW(hKey, L"Name", 0, NULL, (LPBYTE)szName, &dwBufSize) == ERROR_SUCCESS) {
			// get name value
			DWORD dwBufSize = sizeof(szSerial);
			if (::RegQueryValueEx(hKey, L"SN", 0, NULL, (LPBYTE)szSerial, &dwBufSize) == ERROR_SUCCESS)
				bRet = CheckSerial(szName, szSerial);
		}
		::RegCloseKey(hKey);
	}

	// compare
	ANTIDIS_LINEAR
	return bRet;
}

INT_PTR CALLBACK RegisterDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	_anti a;

	switch (uMsg) {
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_REGISTER:
					ANTIDIS_LINEAR
					if (GetLicense(hDlg)) {
						ISDBGPRSNT
						RegisterProgram(hDlg);
						WCHAR szTitle[MAX_PATH];
						MyLoadString(IDS_STRING_TITLEW, lpTitleW);
						MyLoadString(IDS_STRING_TITLEFMTW, lpTitleFmtW);
						wsprintf(szTitle, lpTitleFmtW, lpTitleW, g_szLicenseName);
						MyUnloadString(lpTitleFmtW);
						MyUnloadString(lpTitleW);
						::SetWindowText(::GetParent(hDlg), szTitle);
						::EndDialog(hDlg, TRUE);
					} else {
						MyLoadString(IDS_STRING_INVALIDSERW, lpInvalidSerialW);
						MyLoadString(IDS_STRING_REGFAILW, lpRegFailW);
						//ANTIDIS_LINEAR
						//CheckFuncInt3(MessageBox);
						::MessageBox(hDlg, lpInvalidSerialW, lpRegFailW, MB_OK | MB_ICONERROR);
						MyUnloadString(lpRegFailW);
						MyUnloadString(lpInvalidSerialW);
					}
					return TRUE;
			}
			break;

		case WM_CLOSE:
			::EndDialog(hDlg, FALSE);
			return TRUE;

	}

	return FALSE;
}