#include <Windows.h>

#include "helper.h"

BOOL FileExists(LPCWSTR szPath) {
	DWORD dwAttrib = ::GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void MyWideCharToMultiByte(LPSTR& lpOutput, LPWSTR lpwInput, SIZE_T nInLen) {
	int nSize = WideCharToMultiByte(CP_UTF8, 0, lpwInput, nInLen, NULL, 0, NULL, NULL);
	lpOutput = new CHAR[nSize];
	WideCharToMultiByte(CP_UTF8, 0, lpwInput, nInLen, lpOutput, nSize, NULL, NULL);
}

void MyMultiByteToWideChar(LPWSTR& lpwOutput, LPSTR lpInput, SIZE_T nInLen) {
	int nSize = MultiByteToWideChar(CP_UTF8, 0, lpInput, nInLen, NULL, 0);
	lpwOutput = new WCHAR[nSize];
	MultiByteToWideChar(CP_UTF8, 0, lpInput, nInLen, lpwOutput, nSize);
}

void MyLoadString(UINT uID, LPWSTR& lpBuffer) {
	::LoadString(::GetModuleHandle(NULL), uID, (LPWSTR)&lpBuffer, 0);
}

void MyUnloadString(LPWSTR& lpBuffer) {
	lpBuffer = NULL;
}