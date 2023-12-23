#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include "resource.h"
#include <dwmapi.h>
//#include "pch.h"

#include <winrt/Windows.UI.ViewManagement.h>

using namespace Windows::UI::ViewManagement;

inline bool IsColorLight(Windows::UI::Color& clr)
{
	return (((5 * clr.G) + (2 * clr.R) + clr.B) > (8 * 128));
}

HFONT hFont;

CONST CHAR g_sz_CLASSNAME[] = "Calc";
CONST INT g_i_START_X = 10;
CONST INT g_i_START_Y = 10;
CONST INT g_i_INTERVAL = 1;
CONST INT g_i_BUTTON_SIZE = 50;
CONST INT g_i_BUTTON_DOUBLE_SIZE = g_i_BUTTON_SIZE * 2 + g_i_INTERVAL;
CONST INT g_i_DISPLAY_WIDTH = (g_i_BUTTON_SIZE + g_i_INTERVAL) * 5 - g_i_INTERVAL;
CONST INT g_i_DISPLAY_HEIGHT = 32;
CONST INT g_i_BUTTON_START_X = g_i_START_X;
CONST INT g_i_BUTTON_START_Y = g_i_START_Y + g_i_DISPLAY_HEIGHT + g_i_INTERVAL;
CONST INT g_i_WINDOW_WIDTH = g_i_DISPLAY_WIDTH + g_i_START_X * 2 + 16;
CONST INT g_i_WINDOW_HEIGHT = g_i_DISPLAY_HEIGHT + g_i_START_Y * 2 + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 5 + 42;

CONST CHAR* g_sz_arr_OPERATIONS[] = { "+", "-", "*", "/" };


INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	//1) Регистрация класса окна:
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

	if (RegisterClassEx(&wc) == NULL)
	{
		MessageBox(NULL, "Class registration failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	//2) Создание окна:
	HWND hwnd = CreateWindowEx
	(
		NULL,	//ExStyles
		g_sz_CLASSNAME,		//Class name
		g_sz_CLASSNAME,		//Window name
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,//Window styles ^ - XOR
		CW_USEDEFAULT, CW_USEDEFAULT,//Position
		g_i_WINDOW_WIDTH, g_i_WINDOW_HEIGHT,//Size
		NULL,	//Parent window
		NULL,	//Menu
		hInstance,
		NULL
	);
	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window creation failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	//3) Запуск цикла сообщений:
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
	CONST INT i_DISPLAY_BUFFER_SIZE = 256;
	static CHAR sz_display[i_DISPLAY_BUFFER_SIZE]{};

	static DOUBLE	a = 0;	//operands
	static DOUBLE	b = 0;	//operands
	static INT		operation;
	static BOOL		input = FALSE;
	static BOOL		operation_input = FALSE;
	static BOOL		in_default_state = TRUE;

	switch (uMsg)
	{
	case WM_CREATE:
	{
		HWND hEdit = CreateWindowEx
		(
			NULL, "Edit", "",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
			g_i_START_X, g_i_START_Y,
			g_i_DISPLAY_WIDTH, g_i_DISPLAY_HEIGHT,
			hwnd,
			(HMENU)IDC_EDIT,
			GetModuleHandle(NULL),
			NULL
		);

		LOGFONT LF = { -(g_i_BUTTON_SIZE/3), 0, 0, 0, FW_HEAVY, 0, 0, 0, RUSSIAN_CHARSET,
		   OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, 0, "Cascadia Code" };
		hFont = CreateFontIndirect(&LF);
		SendDlgItemMessage(hwnd, IDC_EDIT, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

		//		Digits:
		for (int i = 6; i >= 0; i -= 3)
		{
			for (int j = 0; j < 3; j++)
			{
				CHAR sz_digit[2] = "";
				sz_digit[0] = i + j + 49;
				HWND hButton = CreateWindowEx
				(
					NULL, "Button", sz_digit,
					WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
					g_i_BUTTON_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * j,	//X-position
					g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * (3 - i / 3 - 1),//Y-position
					g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
					hwnd,
					(HMENU)(IDC_BUTTON_1 + i + j),
					GetModuleHandle(NULL),
					NULL
				);
				HBITMAP hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BUTTON_1+i+j), IMAGE_BITMAP, 0, 0, 0);
				//HBITMAP hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BUTTON_1+i+j));
				SendMessage(hButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
			}
		}
		//
		HWND hButton0 = CreateWindowEx
		(
			NULL, "Button", "0",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_BUTTON_START_X,
			g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 3,
			g_i_BUTTON_DOUBLE_SIZE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_0,
			GetModuleHandle(NULL),
			NULL
		);
		HBITMAP hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BUTTON_0), IMAGE_BITMAP, 0, 0, 0);
		SendMessage(hButton0, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
		//
		HWND hButtonPoint = CreateWindowEx
		(
			NULL, "Button", ".",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_BUTTON_START_X + g_i_BUTTON_DOUBLE_SIZE + g_i_INTERVAL,
			g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 3,
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_POINT,
			GetModuleHandle(NULL),
			NULL
		);
		hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BUTTON_POINT), IMAGE_BITMAP, 0, 0, 0);
		SendMessage(hButtonPoint, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
		////////////			Operations			/////////////////
		for (int i = 0; i < 4; i++)
		{
			HWND hButton = CreateWindowEx
			(
				NULL,
				"Button",
				g_sz_arr_OPERATIONS[3 - i],
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
				g_i_BUTTON_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 3,
				g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * i,
				g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
				hwnd,
				(HMENU)(IDC_BUTTON_SLASH - i),
				GetModuleHandle(NULL),
				NULL
			);
			HBITMAP hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BUTTON_SLASH - i), IMAGE_BITMAP, 0, 0, 0);
			SendMessage(hButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
		}

		HWND hButtonBSP = CreateWindowEx
		(
			NULL,
			"Button",
			"<-",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_BUTTON_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 4,
			g_i_BUTTON_START_Y,
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_BSP,
			GetModuleHandle(NULL),
			NULL
		);
		hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BUTTON_BSP), IMAGE_BITMAP, 0, 0, 0);
		SendMessage(hButtonBSP, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);

		HWND hButtonClear = CreateWindowEx
		(
			NULL,
			"Button",
			"C",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_BUTTON_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 4,
			g_i_BUTTON_START_Y + g_i_BUTTON_SIZE + g_i_INTERVAL,
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_CLEAR,
			GetModuleHandle(NULL),
			NULL
		);
		hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BUTTON_CLEAR), IMAGE_BITMAP, 0, 0, 0);
		SendMessage(hButtonClear, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);

		HWND hButtonEqual = CreateWindowEx
		(
			NULL,
			"Button",
			"=",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_BUTTON_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 4,
			g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 2,
			g_i_BUTTON_SIZE, g_i_BUTTON_DOUBLE_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_EQUAL,
			GetModuleHandle(NULL),
			NULL
		);
		hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BUTTON_EQUAL), IMAGE_BITMAP, 0, 0, 0);
		SendMessage(hButtonEqual, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);

		HWND hButtonChangeTheme = CreateWindowEx
		(
			NULL,
			"Button",
			"Brush",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_BUTTON_START_X,
			g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 4,
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_EQUAL,
			GetModuleHandle(NULL),
			NULL
		);
		hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BUTTON_BRUSH), IMAGE_BITMAP, 0, 0, 0);
		SendMessage(hButtonChangeTheme, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);

		RECT window_rect, client_rect;
		GetWindowRect(hwnd, &window_rect);
		GetClientRect(hwnd, &client_rect);
		using namespace std;
		cout << "Window rect:\t" << window_rect.left << window_rect.top << window_rect.right << window_rect.bottom << endl;
		cout << "Client rect:\t" << client_rect.left << client_rect.top << client_rect.right << client_rect.bottom << endl;
	}
	break;
	case WM_COMMAND:
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
		SendMessage(hEdit, WM_GETTEXT, i_DISPLAY_BUFFER_SIZE, (LPARAM)sz_display);
		CHAR sz_symbol[2]{};
		if (LOWORD(wParam) >= IDC_BUTTON_0 && LOWORD(wParam) <= IDC_BUTTON_POINT) {
			if (input == FALSE)SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)"");
			sz_symbol[0] = LOWORD(wParam) - IDC_BUTTON_0 + 48;
			if (LOWORD(wParam) == IDC_BUTTON_POINT) {
				if (strchr(sz_display, '.'))break;
				sz_symbol[0] = '.';
			}
			strcat(sz_display, sz_symbol);
			SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_display);
			input = TRUE;
		}
		if (LOWORD(wParam) >= IDC_BUTTON_PLUS && LOWORD(wParam) <= IDC_BUTTON_SLASH) {
			SendMessage(hEdit, WM_GETTEXT, i_DISPLAY_BUFFER_SIZE, (LPARAM)sz_display);
			if (input)b = atof(sz_display);

			if (in_default_state)a = b;
			in_default_state = FALSE;

			if (input && operation_input)SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_EQUAL), 0);
			input = FALSE;
			operation = LOWORD(wParam);
			operation_input = TRUE;
		}
		if (LOWORD(wParam) == IDC_BUTTON_EQUAL) {
			SendMessage(hEdit, WM_GETTEXT, i_DISPLAY_BUFFER_SIZE, (LPARAM)sz_display);
			if (input)b = atof(sz_display);
			input = FALSE;
			switch (operation)
			{
			case IDC_BUTTON_PLUS:	a += b;	break;
			case IDC_BUTTON_MINUS:	a -= b; break;
			case IDC_BUTTON_ASTER:	a *= b;	break;
			case IDC_BUTTON_SLASH:	a /= b;	break;
			}
			operation_input = FALSE;
			sprintf(sz_display, "%g", a);
			SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_display);
		}
		if (LOWORD(wParam) == IDC_BUTTON_BSP) {
			CHAR sz_buffer[MAX_PATH]{};
			SendMessage(hEdit, WM_GETTEXT, MAX_PATH, (LPARAM)sz_buffer);
			if (sz_buffer[0])sz_buffer[strlen(sz_buffer) - 1] = 0;
			SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_buffer);
		}
		if (LOWORD(wParam) == IDC_BUTTON_CLEAR) {
			a = b = 0;
			operation = 0;
			input = FALSE;
			operation_input = FALSE;
			in_default_state = TRUE;
			sz_display[0] = 0;
			SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_display);
		}
		if (LOWORD(wParam) == IDC_BUTTON_BRUSH) {
			init_apartment();

			auto settings = UISettings();
			auto foreground = settings.GetColorValue(UIColorType::Foreground);

			bool isDarkMode = static_cast<bool>(IsColorLight(foreground));

			auto revoker = settings.ColorValuesChanged([settings](auto&&...)
				{
					auto foregroundRevoker = settings.GetColorValue(UIColorType::Foreground);
					bool isDarkModeRevoker = static_cast<bool>(IsColorLight(foregroundRevoker));
				});

			static bool s_go = true;
			while (s_go)
			{
				Sleep(50);
			}
	}
	break;
	case WM_DESTROY:PostQuitMessage(0); break;
	case WM_CLOSE:
	{
	DeleteObject(hFont);
	DestroyWindow(hwnd);
	}break;
	default:		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return FALSE;
}