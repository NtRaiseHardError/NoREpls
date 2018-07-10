#include <Windows.h>

#include "aboutdlg.h"
#include "helper.h"
#include "registration.h"
#include "resource.h"

INT_PTR CALLBACK AboutDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_INITDIALOG:
			MyLoadString(IDS_STRING_VERSIONW, lpVersionW);
			::SetDlgItemText(hDlg, IDC_STATIC_VERSION, lpVersionW);
			if (g_bRegistered) {
				WCHAR szTitle[MAX_PATH];
				MyLoadString(IDS_STRING_LICENSEFMTW, lpLicenseFmtW);
				wsprintf(szTitle, lpLicenseFmtW, g_szLicenseName);
				MyUnloadString(lpLicenseFmtW);
				::SetDlgItemText(hDlg, IDC_STATIC_DEMO, szTitle);
			}
			MyUnloadString(lpVersionW);
			return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
					::EndDialog(hDlg, TRUE);
					return TRUE;
			}
			break;

		case WM_CLOSE:
			::EndDialog(hDlg, TRUE);
			return TRUE;
	}

	return FALSE;
}