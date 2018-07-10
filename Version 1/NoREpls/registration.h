#pragma once
#ifndef __REGISTRATION_H__
#define __REGISTRATION_H__

//#define VERSION L"Version 1.3"
//#define TITLEA "NoREpls - Version 1.3"
//#define TITLE L"NoREpls - Version 1.3"
//#define TITLE_DEMO L"NoREpls - Version 1.3 (demo version)"
//#define LICENSE_FILE L"License.lic"
// "NOREPLS-0892-H43O-19JG-3UAN"
// "NOREPLS-MAST-ERKE-Y000-0000"
// #define SERIAL "deab67dde2001dc1a078456015765ed6"

extern LPWSTR lpVersionW;
extern LPWSTR lpTitleW;
extern LPWSTR lpTitleW;
extern LPWSTR lpLicenseW;
extern LPWSTR lpRegRequiredW;
extern LPWSTR lpUnRegW;
extern LPWSTR lpRegSuccessW;
extern LPWSTR lpRegThankW;
extern LPWSTR lpRegFailW;
extern LPWSTR lpInvalidSerialW;
extern LPWSTR lpNoSerialW;
extern LPWSTR lpEnterSerialW;
extern LPWSTR lpNoNameW;
extern LPWSTR lpEnterNameW;
extern LPWSTR lpSerialEqW;
extern LPWSTR lpNameEqW;
extern LPWSTR lpTitleFmtW;
extern LPWSTR lpSerialFmtW;
extern LPWSTR lpLicenseFmtW;
extern LPWSTR lpContUnregW;
extern LPWSTR lpWaitFmtW;

extern bool g_bRegistered;
extern WCHAR g_szLicenseName[MAX_PATH];

void RegisterProgram(HWND hDlg);
//bool CheckSerial(LPSTR lpName, LPSTR lpSerial);
bool CheckLicenseRegistry();
INT_PTR CALLBACK RegisterDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif // !__REGISTRATION_H__
