#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include "resource.h"

CONST CHAR* g_sz_VALUES[] = { "This", "is", "my", "first", "Combo", "Box" };

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ChildDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT cCmdShow) {
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static HWND hAddDialog;

	switch (uMsg) {
	case WM_INITDIALOG: {
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_PLANE));
		SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIcon);

		HWND hCombo = GetDlgItem(hwnd, IDC_COMBO1);
		for (int i = 0; i < sizeof(g_sz_VALUES) / sizeof(*g_sz_VALUES); i++) {
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)g_sz_VALUES[i]);
		}
		SendMessage(hCombo, CB_SETCURSEL, 0, 0);
	}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_ADD:
		{
			HWND hAddDialog = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2), hwnd, (DLGPROC)ChildDlgProc);
			ShowWindow(hAddDialog, SW_SHOW);
		}
		break;
		case IDC_BUTTON_DELETE: {
			HWND hCombo = GetDlgItem(hwnd, IDC_COMBO1);
			CHAR sz_buffer[MAX_PATH]{};
			CHAR sz_message[MAX_PATH]{};
			int i = SendMessage(hCombo, CB_GETCURSEL, 0, 0);

			if (i != CB_ERR) {
				SendMessage(hCombo, CB_GETLBTEXT, i, (LPARAM)sz_buffer);
				sprintf(sz_message, "Вы действительно хотите удалить пункт %i со значением \"%s\"?", i, sz_buffer);
				int result = MessageBox(hwnd, sz_message, "Info", MB_YESNO | MB_ICONQUESTION);

				if (result = IDYES) {
					SendMessage(hCombo, CB_DELETESTRING, i, 0);
				}
			}
			else {
				sprintf(sz_message, "Нет удаляемого значения ;)");
				MessageBox(hwnd, sz_message, "Info", MB_OK | MB_ICONERROR);
			}
			SendMessage(hCombo, CB_SETCURSEL, 0, 0);
		}
			break;
		case IDOK: {
			HWND hCombo = GetDlgItem(hwnd, IDC_COMBO1);
			CHAR sz_buffer[MAX_PATH]{};
			CHAR sz_message[MAX_PATH]{};
			int i = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
			SendMessage(hCombo, CB_GETLBTEXT, i, (LPARAM)sz_buffer);
		    sprintf(sz_message, "Вы выбрали пункт %i со значением \"%s\".", i, sz_buffer);
			MessageBox(hwnd, sz_message, "Info", MB_OK | MB_ICONINFORMATION);
		}
			break;
		case IDCANCEL: EndDialog(hwnd, 0); 
			break;
		}
		break;
	case WM_CLOSE: EndDialog(hwnd, 0); break;
	}
	return FALSE;
}

BOOL CALLBACK ChildDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) {
	case WM_INITDIALOG: {
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_PLANE));
		SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIcon);
	}
					  break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			HWND hText = GetDlgItem(hwnd, IDC_EDIT1);
			HWND hCombo = GetDlgItem(hwnd, IDC_COMBO1);
			CHAR sz_buffer[MAX_PATH] = {};
			GetWindowText(hText, sz_buffer, MAX_PATH);
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)sz_buffer);

			EndDialog(hwnd, 0);
		}
				 break;
		case IDCANCEL: EndDialog(hwnd, 0);
			break;
		}
		break;
	case WM_CLOSE: EndDialog(hwnd, 0); break;
	}
	return FALSE;
}