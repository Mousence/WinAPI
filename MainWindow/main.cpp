#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <string>
#include <CommCtrl.h>
#include <mmiscapi.h>
#include "resource.h"

CONST CHAR g_sz_WINDOW_CLASS[] = "My first Window";
CONST INT g_i_START_X = 10;
CONST INT g_i_START_Y = 10;
CONST INT g_I_BUTTON_SIZE = 200;

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HWND g_hCheckBox;
TOOLINFO g_toolItem;
HWND g_hwndTrackingTT;
BOOL g_trackingMouse = FALSE;

INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND CreateTrackingToolTip(INT toolID, HWND hwnd, LPSTR lpText);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow) {
	// 1) Регистрация класса окна
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	//cb - Count Bytes
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.style = 0;

	//wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	//wc.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
	wc.hIcon = (HICON)LoadImage(hInstance, "icon_py_is_trash.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	wc.hIconSm = (HICON)LoadImage(hInstance, "pycharm.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	//wc.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
	wc.hCursor = (HICON)LoadImage(hInstance, "eyeball2.ani", IMAGE_CURSOR, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	wc.hInstance = hInstance;
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.lpszClassName = g_sz_WINDOW_CLASS;

	if (RegisterClassEx(&wc) == NULL) {
		MessageBox(NULL, "Class registration failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	// 2) Создание окна
	// SM - System Metrics
	INT screenWidth = GetSystemMetrics(SM_CXSCREEN);
	INT screenHeight = GetSystemMetrics(SM_CYSCREEN);

	INT windowWidth = screenWidth * 0.75;
	INT windowHeight = screenHeight * 0.75;

	INT windowX = (screenWidth - windowWidth) / 2;
	INT windowY = (screenHeight - windowHeight) / 2;

	HWND hwnd = CreateWindowEx
	(
		WS_EX_TOPMOST,
		g_sz_WINDOW_CLASS,
		g_sz_WINDOW_CLASS,
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
		windowX, windowY,
		windowWidth, windowHeight,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	// 3) Запуск цикла сообщений
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{

	case WM_CREATE: {
		g_hCheckBox = CreateWindowEx
		(
			NULL, "Button", "Показать координаты мыши",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			g_i_START_X * 5, //x
			g_i_START_Y * 10, //y
			g_I_BUTTON_SIZE, g_I_BUTTON_SIZE /3,
			hwnd,
			(HMENU)(IDC_BUTTON),
			GetModuleHandle(NULL),
			NULL
		);
		InitCommonControls();
		g_hwndTrackingTT = CreateTrackingToolTip(IDC_TOOLTIP, hwnd, (LPSTR)"");
		return TRUE;
	}
	case WM_MOUSELEAVE:
		if (!g_trackingMouse) {
			SendMessage(g_hwndTrackingTT, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&g_toolItem);
		}
		return FALSE;
	case WM_MOUSEMOVE: {
		static INT oldX, oldY;
		INT newX, newY;

		if (!g_trackingMouse)
		{
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
			tme.hwndTrack = hwnd;
			tme.dwFlags = TME_LEAVE;

			TrackMouseEvent(&tme);

			SendMessage(g_hwndTrackingTT, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&g_toolItem);
			g_trackingMouse = TRUE;
		}

		newX = LOWORD(lParam);
		newY = HIWORD(lParam);

		if (newX != oldX || newY != oldY)
		{
			CONST INT SIZE = 256;
			CHAR sz_title[SIZE]{};

			RECT rect;
			GetWindowRect(hwnd, &rect);

			oldX = newX;
			oldY = newY;

			sprintf(sz_title, "%s Position:%ix%i, Size: %ix%i, Mouse Position: %ix%i", g_sz_WINDOW_CLASS,
				rect.left, rect.top,
				rect.right - rect.left, rect.bottom - rect.top,
				oldX, oldY);
			SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)sz_title);

			CHAR coords[12]{};
			sprintf(coords, "%ix%i", oldX, oldY);
			g_toolItem.lpszText = coords;
			SendMessage(g_hwndTrackingTT, TTM_SETTOOLINFO, 0, (LPARAM)&g_toolItem);

			POINT pt = { oldX, oldY };
			ClientToScreen(hwnd, &pt);
			SendMessage(g_hwndTrackingTT, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(pt.x + 10, pt.y - 20));
		}
	}
		return FALSE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_BUTTON) {
			if (SendMessage(g_hCheckBox, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				SendMessage(g_hwndTrackingTT, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&g_toolItem);
				g_trackingMouse = TRUE;
			}
			else
			{
				SendMessage(g_hwndTrackingTT, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&g_toolItem);
				g_trackingMouse = FALSE;
			}
		}
		break;
	case WM_SIZE:
	case WM_MOVING:
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);

		CONST INT SIZE = 256;
		CHAR sz_title[SIZE]{};
		RECT rect;
		GetWindowRect(hwnd, &rect);
		sprintf(sz_title, "%s Position:%ix%i, Size: %ix%i, Mouse Position: %ix%i", g_sz_WINDOW_CLASS,
			rect.left, rect.top,
			rect.right - rect.left, rect.bottom - rect.top, cursorPos.x, cursorPos.y);
		SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)sz_title);
	}
	break;
	case WM_DESTROY:PostQuitMessage(0); break;
	case WM_CLOSE: {
		CHAR sz_message[MAX_PATH];
		sprintf(sz_message, "Вы действительно хотите выйти?");
		int result = MessageBox(hwnd, sz_message, "Вопрос", MB_YESNO | MB_ICONQUESTION);

		if (result == IDYES) {
			DestroyWindow(hwnd);
			break;
		}
	}
				 break;
	default:		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}
HWND CreateTrackingToolTip(INT	toolID, HWND hwnd, LPSTR lpszText)
{
	HWND hwndTT = CreateWindowEx(
		WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hwnd,
		NULL,
		GetModuleHandle(NULL),
		NULL
	);
	if (hwndTT == NULL)
	{
		MessageBox(hwnd, "Tooltip creation failed", "Error", MB_OK | MB_ICONERROR);
		return NULL;
	}

	g_toolItem.cbSize = sizeof(TOOLINFO);
	g_toolItem.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
	g_toolItem.hwnd = hwnd;
	g_toolItem.hinst = GetModuleHandle(NULL);
	g_toolItem.lpszText = lpszText;
	g_toolItem.uId = (UINT_PTR)hwnd;
	GetClientRect(hwnd, &g_toolItem.rect);

	//Associate the tooltip with the tool window.
	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&g_toolItem);
	return hwndTT;
}