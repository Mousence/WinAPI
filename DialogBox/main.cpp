#include <Windows.h>
#include "resource.h"

CONST CHAR* LOGIN_TEXT = "Введите логин";

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//HWND - окно (Handler to Window - обработчик окна)
	//uMsg - сообщение, которое передается окну
	//wParam, lParam - параметры сообщения
	switch (uMsg)
	{
	case WM_INITDIALOG: {
		//Здесь можно создавать элементы интерфейса.
		//Эта секция отрабатывает один раз при запуске окна.
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIcon);

		HWND hLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
		SendMessage(hLogin, WM_SETTEXT, 0, (LPARAM)LOGIN_TEXT);
	}
					  break;
	case WM_COMMAND:
		//Здесь обрабатывается конамды нажатия кнопок, сочетания клавиш и т.д.
		switch (LOWORD(wParam))
		{
		case IDC_EDIT_LOGIN:
		{
			HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
			CHAR sz_buffer[MAX_PATH] = {};
			SendMessage(hEditLogin, WM_GETTEXT, MAX_PATH, (LPARAM)sz_buffer);


			if (HIWORD(wParam) == EN_SETFOCUS) {
				if (strcmp(sz_buffer, LOGIN_TEXT) == 0)
				SendMessage(hEditLogin, WM_SETTEXT, 0, (LPARAM)"");
			}
			if (HIWORD(wParam) == EN_KILLFOCUS){
				if(strlen(sz_buffer) == 0)
					SendMessage(hEditLogin, WM_SETTEXT, 0, (LPARAM)LOGIN_TEXT);
			}
		}
		break;
		case IDOK:
		MessageBox(hwnd, "Была нажата кнопка ОК", "Info", MB_OK | MB_ICONINFORMATION);
			break;
		case IDC_BUTTON_COPY: {
			CONST INT SIZE = 256;
			CHAR sz_buffer[SIZE] = {};
			HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
			HWND hEditPassword = GetDlgItem(hwnd, IDC_EDIT_PASSWORD);
			SendMessage(hEditLogin, WM_GETTEXT, SIZE, (LPARAM)sz_buffer);
			SendMessage(hEditPassword, WM_SETTEXT, 0, (LPARAM)sz_buffer);
		}
							break;
		case IDCANCEL: EndDialog(hwnd, 0); break;
		}
		break;
	case WM_CLOSE:
		//Отрабатывает при нажатии кнопки "Закрыть" (X)
		EndDialog(hwnd, 0);
		break;
	}
	return FALSE;
}
