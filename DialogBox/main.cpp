#include <Windows.h>
#include "resource.h"

CONST CHAR* g_sz_LOGIN_INVITE = "������� �����";

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//HWND - ���� (Handler to Window - ���������� ����)
	//uMsg - ���������, ������� ���������� ����
	//wParam, lParam - ��������� ���������
	switch (uMsg)
	{
	case WM_INITDIALOG: {
		//����� ����� ��������� �������� ����������.
		//��� ������ ������������ ���� ��� ��� ������� ����.
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIcon);

		SendMessage(GetDlgItem(hwnd, IDC_EDIT_LOGIN), WM_SETTEXT, 0, (LPARAM)g_sz_LOGIN_INVITE);
	}
					  break;
	case WM_COMMAND:
		//����� �������������� ������� ������� ������, ��������� ������ � �.�.
		switch (LOWORD(wParam))
		{
		case IDC_EDIT_LOGIN: {
			HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
			CHAR sz_buffer[MAX_PATH] = {};
			SendMessage(hEditLogin, WM_GETTEXT, MAX_PATH, (LPARAM)sz_buffer);

			//EN - Edit Notification
			if (HIWORD(wParam) == EN_SETFOCUS && strcmp(sz_buffer, g_sz_LOGIN_INVITE) == 0)
				SendMessage(hEditLogin, WM_SETTEXT, 0, (LPARAM)"");
			if (HIWORD(wParam) == EN_KILLFOCUS && strlen(sz_buffer) == 0)
					SendMessage(hEditLogin, WM_SETTEXT, 0, (LPARAM)g_sz_LOGIN_INVITE);
		}
		break;
		case IDOK:
		MessageBox(hwnd, "���� ������ ������ ��", "Info", MB_OK | MB_ICONINFORMATION);
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
		//������������ ��� ������� ������ "�������" (X)
		EndDialog(hwnd, 0);
		break;
	}
	return FALSE;
}
