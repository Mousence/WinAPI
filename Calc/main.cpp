#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <string>
#include <CommCtrl.h>
#include <mmiscapi.h>
#include "resource.h"

CONST CHAR g_sz_CLASSNAME[] = "My Calc";
CONST INT g_i_START_X = 10;
CONST INT g_i_START_Y = 10;
CONST INT g_i_INTERVAL = 5;
CONST INT g_I_BUTTON_SIZE = 50;
CONST INT g_I_BUTTON_DOUBLE_SIZE = g_I_BUTTON_SIZE * 2 + g_i_INTERVAL;
CONST INT g_i_DISPLAY_WIDTH = (g_I_BUTTON_DOUBLE_SIZE + g_i_INTERVAL) * 5;
CONST INT g_i_DISPLAY_HEIGHT = 32;
CONST INT g_i_BUTTON_START_X = g_i_START_X;
CONST INT g_i_BUTTON_START_Y = g_i_START_Y + g_i_DISPLAY_HEIGHT + g_i_INTERVAL;

INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdShow, INT nCmdShow) {
	//1) Регистрация класса окна
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.style = 0;

	wc.hIcon = (HICON)LoadImage(hInstance, "calc.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	wc.hIconSm = (HICON)LoadImage(hInstance, "calc.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	wc.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	wc.hInstance = hInstance;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_sz_CLASSNAME;
	wc.lpfnWndProc = (WNDPROC)WndProc;

	if (RegisterClassEx(&wc) == NULL) {
		MessageBox(NULL, "Class registration failed", "Error", MB_OK | MB_ICONERROR);
		return NULL;
	}
	//2) Создание окна
	HWND hwnd = CreateWindowEx
	(
		NULL,
		g_sz_CLASSNAME,
		g_sz_CLASSNAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (hwnd == NULL) {
		MessageBox(NULL, "Window creation failed", "Error", MB_OK | MB_ICONERROR);
		return NULL;
	}
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	//3) Запуск цикла сообщений
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE:
	{
		HWND hEfit = CreateWindowEx(
			NULL,
			"Edit", "",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, //ES_RIGHT выравнивание по правому
			g_i_START_X, g_i_START_Y,
			g_i_DISPLAY_WIDTH, g_i_DISPLAY_HEIGHT,
			hwnd,
			(HMENU)IDC_EDIT,
			GetModuleHandle(NULL),
			NULL
		);
		for (int i = 6; i >= 0; i-=3) {
			for (int j = 0; j < 3; j++) {
				CHAR sz_digit[2] = "";
				sz_digit[0] = i + j + 49;
				CreateWindowEx
				(
					NULL, "Button", sz_digit,
					WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					g_i_BUTTON_START_X + (g_I_BUTTON_SIZE + g_i_INTERVAL) * j, //x
					g_i_BUTTON_START_Y + (g_I_BUTTON_SIZE + g_i_INTERVAL) * (3 - i/3 - 1), //y
					g_I_BUTTON_SIZE, g_I_BUTTON_SIZE,
					hwnd,
					(HMENU)(IDC_BUTTON_0 + i + j),
					GetModuleHandle(NULL),
					NULL
				);
			}
		}
		CreateWindowEx(NULL, "Button", "0", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			g_i_BUTTON_START_X,
			g_i_BUTTON_START_Y + (g_I_BUTTON_SIZE + g_i_INTERVAL)*3,
			g_I_BUTTON_DOUBLE_SIZE, g_I_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_0,
			GetModuleHandle(NULL),
			NULL);
		CreateWindowEx(NULL, "Button", ".", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			g_i_BUTTON_START_X + g_I_BUTTON_DOUBLE_SIZE + g_i_INTERVAL,
			g_i_BUTTON_START_Y + (g_I_BUTTON_SIZE + g_i_INTERVAL) * 3,
			g_I_BUTTON_SIZE, g_I_BUTTON_SIZE, 
			hwnd,
			(HMENU)IDC_BUTTON_POINT,
			GetModuleHandle(NULL),
			NULL);
	}
		break;
	case WM_COMMAND:
		break;
	case WM_DESTROY: PostQuitMessage(0); break;
	case WM_CLOSE: {
		CHAR sz_message[MAX_PATH];
		sprintf(sz_message, "Вы действительно хотите выйти?");
		int result = MessageBox(hwnd, sz_message, "Вопрос", MB_YESNO | MB_ICONQUESTION);

		if (result == IDYES) {
			DestroyWindow(hwnd);
		}
	}
				 break;
	default:         return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return FALSE;
}