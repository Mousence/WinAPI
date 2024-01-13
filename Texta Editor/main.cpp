#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <Richedit.h> 
#include "resource.h"

CONST CHAR g_sz_CLASS_NAME[] = "My text editor";

INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT CALLBACK DlgProcAbout(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

CHAR* DropFile(HDROP hDrop);
BOOL LoadTextFileToEdit(HWND hEdit, LPSTR lpszFileName);
BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR lpszFileName);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	//1
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.style = 0;

	wc.hIcon = (HICON)LoadImage(hInstance, "ICO\\TextEdit.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	wc.hIconSm = (HICON)LoadImage(hInstance, "ICO\\TextEditor.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	wc.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	wc.hInstance = hInstance;
	wc.lpszClassName = g_sz_CLASS_NAME;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MAIN_MENU);

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Class registration failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	//2

	HWND hwnd = CreateWindowEx
	(
		NULL,
		g_sz_CLASS_NAME,
		g_sz_CLASS_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL,
		hInstance,
		NULL
	);
	if (hwnd == 0) {
		MessageBox(NULL, "Window creation failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	DragAcceptFiles(hwnd, TRUE);
	//3
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CHAR szFileName[MAX_PATH]{};
	static CHAR* szFileText = NULL;
	static BOOL onSave = TRUE;
	switch (uMsg)
	{
	case WM_CREATE:
	{
		LoadLibrary("riched20.dll");
		////////////////////////////
		RECT rect;
		GetWindowRect(hwnd, &rect);

		HWND hEdit = CreateWindowEx
		(
			NULL, RICHEDIT_CLASS, "",
			WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
			0, 0,
			rect.right, rect.bottom,
			hwnd,
			(HMENU)IDC_EDIT,
			GetModuleHandle(NULL),
			NULL
		);
		SetFocus(hEdit);
	} break;
	case WM_SIZE: {
		RECT rect;
		GetClientRect(hwnd, &rect);
		SetWindowPos(GetDlgItem(hwnd, IDC_EDIT), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_NOZORDER);
	} break;
	case WM_DROPFILES: {
		CHAR* szFileName = DropFile((HDROP)wParam);
		if (SendMessage(GetDlgItem(hwnd, IDC_EDIT), EM_GETMODIFY, 0, 0)) {
			switch (MessageBox(hwnd, "Сохранить изменения в файле?", "info", MB_YESNOCANCEL | MB_ICONQUESTION)) {
			case IDYES: SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVE, 0); break;
			case IDNO:  SendMessage(GetDlgItem(hwnd, IDC_EDIT), EM_SETMODIFY, FALSE, 0);
			case IDCANCEL:
				break;
			}
		}
		if (!(SendMessage(GetDlgItem(hwnd, IDC_EDIT), EM_GETMODIFY, 0, 0))) {
				HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
				onSave = LoadTextFileToEdit(hEdit, szFileName);
		}
	} break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_FILE_OPEN:
		{
			//CHAR szFileName[MAX_PATH]{};
			if (SendMessage(GetDlgItem(hwnd, IDC_EDIT), EM_GETMODIFY, 0, 0)) {
				switch (MessageBox(hwnd, "Сохранить изменения в файле?", "info", MB_YESNOCANCEL | MB_ICONQUESTION)) {
				case IDYES: SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVE, 0); break;
				case IDNO:  SendMessage(GetDlgItem(hwnd, IDC_EDIT), EM_SETMODIFY, FALSE, 0);
				case IDCANCEL:
					break;
				}
			}
			if (!(SendMessage(GetDlgItem(hwnd, IDC_EDIT), EM_GETMODIFY, 0, 0))) {
				OPENFILENAME ofn;
				ZeroMemory(&ofn, sizeof(ofn));

				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFilter = "Text files: (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
				ofn.lpstrFile = szFileName;
				ofn.nMaxFile = MAX_PATH;
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
				ofn.lpstrDefExt = "txt";

				if (GetOpenFileName(&ofn))
				{
					HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
					onSave = LoadTextFileToEdit(hEdit, szFileName);
				}
			}
		}
		break;
		case ID_FILE_SAVE:
			if (strlen(szFileName)) onSave = SaveTextFileFromEdit(GetDlgItem(hwnd, IDC_EDIT), szFileName);
			else SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVEAS, 0);
			break;
		case ID_FILE_SAVEAS:
		{

			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFilter = "Text files: (*.txt)\0*.txt\0All files (*.*)\0:*.*\0";
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
			ofn.lpstrDefExt = "txt";

			if (GetSaveFileName(&ofn))SaveTextFileFromEdit(GetDlgItem(hwnd, IDC_EDIT), szFileName);
		}
		break;
		case ID_HELP_ABOUT:
			DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hwnd, (DLGPROC)DlgProcAbout, 0);
			break;
		}
	} break;
	case WM_DESTROY: PostQuitMessage(0); break;
	case WM_CLOSE: {
		HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
		if (SendMessage(GetDlgItem(hwnd, IDC_EDIT), EM_GETMODIFY, 0, 0)) {
			onSave = FALSE;
			switch (MessageBox(hwnd, "Сохранить изменения в файле?", "info", MB_YESNOCANCEL | MB_ICONQUESTION)) {
			case IDYES:
				SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVE, 0);
				if (onSave) DestroyWindow(hwnd); break;
			case IDNO: DefWindowProc(hwnd, uMsg, wParam, lParam);
			case IDCANCEL:
				break;
			}
		}
		else DestroyWindow(hwnd);
	} break;
	default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
INT CALLBACK DlgProcAbout(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: EndDialog(hwnd, 0); break;
		}
	case WM_CLOSE: EndDialog(hwnd, 0); break;
	}
	return FALSE;
}
CHAR* DropFile(HDROP hDrop) {
	CHAR szFileName[MAX_PATH]{};

	UINT numFiles = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
	if (numFiles > 0) DragQueryFile(hDrop, 0, szFileName, MAX_PATH);
	

	DragFinish(hDrop);

	CHAR* result = new CHAR[strlen(szFileName) + 1];
	strcpy(result, szFileName);

	return result;
}

BOOL LoadTextFileToEdit(HWND hEdit, LPSTR lpszFileName)
{
	BOOL bSuccess = FALSE;
	HANDLE hFile = CreateFile(lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile != INVALID_HANDLE_VALUE) {
		DWORD dwFileSize = GetFileSize(hFile, NULL);

		if (dwFileSize != UINT_MAX) {
			LPSTR lpszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);

			if (lpszFileText) {
				DWORD dwRead;

				if (ReadFile(hFile, lpszFileText, dwFileSize, &dwRead, NULL)) {
					lpszFileText[dwFileSize] = 0;

					// Load the text and clear the undo buffer
					if (SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)lpszFileText)) {
						SendMessage(hEdit, EM_EMPTYUNDOBUFFER, 0, 0);
						SendMessage(hEdit, EM_SETUNDOLIMIT, -1, 0);
						bSuccess = TRUE;
					}
				}

				GlobalFree(lpszFileText);
			}

			CloseHandle(hFile);
		}
	}

	return bSuccess;
}

BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR lpszFileName)
{
	BOOL bSuccess = FALSE;
	HANDLE hFile = CreateFile(lpszFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);

	if (hFile != INVALID_HANDLE_VALUE) {
		INT nTextLength = GetWindowTextLength(hEdit);
		LPSTR lpszFileText = (LPSTR)GlobalAlloc(GPTR, nTextLength + 1);

		if (lpszFileText) {
			GetWindowText(hEdit, lpszFileText, nTextLength + 1);
			DWORD dwWritten;

			if (WriteFile(hFile, lpszFileText, nTextLength, &dwWritten, NULL)) {
				bSuccess = TRUE;
			}

			GlobalFree(lpszFileText);
		}

		CloseHandle(hFile);
	}

	if (bSuccess) {
		SendMessage(hEdit, EM_SETMODIFY, FALSE, 0);
	}

	return bSuccess;
}