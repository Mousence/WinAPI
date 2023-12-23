#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<Windowsx.h>
#include<cstdio>
#include<CommCtrl.h>
#include"resource.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

CONST CHAR g_sz_WINDOW_CLASS[] = "My first Window";

TOOLINFO g_toolItem;
HWND g_hwndTrackingTT;
BOOL g_trackingMouse = FALSE;

INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND CreateTrackingToolTip(INT	toolID, HWND hwnd, LPSTR lp_text);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	//1) Регистрация класса окна:
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	//cb - Count Bytes
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.style = 0;

	//wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	//wc.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
	wc.hIcon = (HICON)LoadImage(hInstance, "pycharm.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	wc.hIconSm = (HICON)LoadImage(hInstance, "pycharm.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	wc.hCursor = (HCURSOR)LoadImage(hInstance, "eyeball2.ani", IMAGE_CURSOR, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	//wc.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_ARROW));
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	wc.hInstance = hInstance;
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.lpszClassName = g_sz_WINDOW_CLASS;
	if (RegisterClassEx(&wc) == NULL)
	{
		MessageBox(NULL, "Class registration failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	//2) Создание окна:
	//SM_ - System Metric;
	INT screen_width = GetSystemMetrics(SM_CXSCREEN);
	INT screen_height = GetSystemMetrics(SM_CYSCREEN);
	INT window_width = screen_width * 3 / 4;
	INT window_height = screen_height * 3 / 4;
	INT window_start_x = screen_width / 8;
	INT window_start_y = screen_height / 8;
	HWND hwnd = CreateWindowEx
	(
		NULL,	//ExStyles
		g_sz_WINDOW_CLASS,	//Class name
		g_sz_WINDOW_CLASS,	//Заголовок окна
		WS_OVERLAPPEDWINDOW,//Такой стиль задается главному окну. Этот стиль определяет ,
		//что у окна будет строка заголовка, кнопки управления окном, 
		//и маштабируемая граница.
		window_start_x, window_start_y,	//Положение окна на экране
		window_width, window_height,	//Положение окна на экране
		//CW_USEDEFAULT, CW_USEDEFAULT,	//Размер окна
		//CW_USEDEFAULT, CW_USEDEFAULT,	//Размер окна
		NULL,	//Родительское окно
		NULL,	//Для главного окна этот параметр задает меню,
		hInstance,
		NULL
	);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	//3) Запуск цикла сообщений:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		IsDialogMessage(hwnd, &msg);
	}
	return msg.wParam;
}

INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BOOL bShowCoords = FALSE;
	switch (uMsg)
	{
	case WM_CREATE: {
		InitCommonControls();
		g_hwndTrackingTT = CreateTrackingToolTip(IDC_TOOLTIP, hwnd, (LPSTR)"");
		CreateWindowEx(
			NULL, "Button", "Показать координаты мыши",
			BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE,
			10, 10, 250, 20,
			hwnd,
			(HMENU)IDC_CHECKBOX_MOUSE_COORDS,
			GetModuleHandle(NULL),
			NULL
		);
		CreateWindowEx(
			NULL, STATUSCLASSNAME, "Status bar",
			WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			hwnd,
			(HMENU)IDM_STATUS,
			GetModuleHandle(NULL),
			NULL
		);
		INT parts[2] = { 64, -1 };
		HWND hStatus = CreateWindowEx(
			NULL,
			STATUSCLASSNAME,
			"status bar",
			WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			hwnd,
			(HMENU)IDM_STATUS,
			GetModuleHandle(NULL),
			NULL
		);
	}
		return TRUE;
	case WM_MOUSELEAVE:
		SendMessage(g_hwndTrackingTT, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&g_toolItem);
		g_trackingMouse = FALSE;
		return FALSE;
	case WM_MOUSEMOVE:
	{
		static INT oldX, oldY;
		INT newX, newY;
		if (!g_trackingMouse)
		{
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
			tme.hwndTrack = hwnd;
			tme.dwFlags = TME_LEAVE;

			TrackMouseEvent(&tme);

			if(bShowCoords)SendMessage(g_hwndTrackingTT, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&g_toolItem);
			g_trackingMouse = TRUE;
		}
		GetClientRect(hwnd, &g_toolItem.rect);
		newX = LOWORD(lParam);
		newY = HIWORD(lParam);

		if (newX != oldX || newY != oldY)
		{
			oldX = newX;
			oldY = newY;

			CHAR coords[12]{};
			sprintf(coords, "%ix%i", oldX, oldY);
			g_toolItem.lpszText = coords;
			SendMessage(g_hwndTrackingTT, TTM_SETTOOLINFO, 0, (LPARAM)&g_toolItem);

			POINT pt = { newX, newY };
			ClientToScreen(hwnd, &pt);
			SendMessage(g_hwndTrackingTT, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(pt.x + 10, pt.y - 20));
		}
		CHAR sz_status[256]{};
		sprintf(sz_status, "Mouse position: %ix%i", oldX, oldY);
		SendMessage(GetDlgItem(hwnd, IDM_STATUS), SB_SETPARTS, 1, (LPARAM)sz_status);
	}
	return FALSE;
	case WM_SIZE:
	case WM_MOVE:
	{
		CONST INT SIZE = 256;
		CHAR sz_title[SIZE]{};
		RECT rect;
		GetWindowRect(hwnd, &rect);
		sprintf(sz_title, "%s Position:%ix%i, Size: %ix%i", g_sz_WINDOW_CLASS,
			rect.left, rect.top,
			rect.right - rect.left, rect.bottom - rect.top);
		SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)sz_title);
		SetWindowPos(GetDlgItem(hwnd, IDM_STATUS), NULL, 0, 0, 0, 0, SWP_NOMOVE);
		UpdateWindow(hwnd);

	}
	break;
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case IDC_CHECKBOX_MOUSE_COORDS:
			if (SendMessage(GetDlgItem(hwnd, IDC_CHECKBOX_MOUSE_COORDS), BM_GETCHECK, 0, 0) == BST_CHECKED)
				bShowCoords = TRUE;
			else bShowCoords = FALSE;
			break;
		}
	}
		break;
	case WM_DESTROY:PostQuitMessage(0); break;
	case WM_CLOSE:
		switch (MessageBoxA(hwnd, "Вы действительно хотите выйти?", "Вопрос", MB_YESNO | MB_ICONQUESTION)) {
		case IDYES: DestroyWindow(hwnd);
		case IDNO: break;
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

	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&g_toolItem);
	return hwndTT;
}