#include <Windows.h>

#include "helper.h"
#include "nagdlg.h"
#include "registration.h"
#include "resource.h"

#define IDT_TIMER 1

INT_PTR CALLBACK NagDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static int i = 4;

	switch (uMsg) {
		case WM_INITDIALOG:
			i = 4;
			::SetTimer(hDlg, IDT_TIMER, 1000, NULL);
			return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
					::EndDialog(hDlg, TRUE);
					return TRUE;
			}
			break;
		case WM_TIMER:
			switch (wParam) {
				case IDT_TIMER:
					if (i == 0) {
						MyLoadString(IDS_STRING_CONTUNREGW, lpContUnregW);
						::SetDlgItemText(hDlg, IDOK, lpContUnregW);
						MyUnloadString(lpContUnregW);
						::EnableWindow(::GetDlgItem(hDlg, IDOK), TRUE);
						::KillTimer(hDlg, IDT_TIMER);
					} else {
						WCHAR szWaitText[26];
						MyLoadString(IDS_STRING_WAITFMTW, lpWaitFmtW);
						wsprintf(szWaitText, lpWaitFmtW, i);
						MyUnloadString(lpWaitFmtW);
						::SetDlgItemText(hDlg, IDOK, szWaitText);
						i--;
					}
			}
		case WM_CLOSE:
			return TRUE;
	}

	return FALSE;
}