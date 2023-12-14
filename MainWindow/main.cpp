#include <Windows.h>
#include <string>
#include "resource.h"

CONST CHAR g_sz_WINDOW_CLASS[] = "My first Window";

INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
	wc.hIcon = (HICON)LoadImage(hInstance, "icon_py_trash.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
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
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int windowWidth = screenWidth * 0.75;
	int windowHeight = screenHeight * 0.75;

	int windowX = (screenWidth - windowWidth) / 2;
	int windowY = (screenHeight - windowHeight) / 2;

	HWND hwnd = CreateWindowEx
	(
		NULL,
		g_sz_WINDOW_CLASS,
		g_sz_WINDOW_CLASS,
		WS_OVERLAPPEDWINDOW,
		windowX, windowY,
		windowWidth, windowHeight,
		NULL,
		NULL,
		hInstance,
		NULL
	);
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
	static RECT windowRect;

	switch (uMsg)
	{
	case WM_MOVE:
		GetWindowRect(hwnd, &windowRect);
		break;
	case WM_SIZE:
		GetWindowRect(hwnd, &windowRect);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// Display size and position in the title bar
		std::string title = std::to_string(windowRect.right - windowRect.left) +
			"x" + std::to_string(windowRect.bottom - windowRect.top) +
			"\t" + std::to_string(windowRect.left) +
			"x" + std::to_string(windowRect.top);

		SetWindowText(hwnd, title.c_str());

		EndPaint(hwnd, &ps);
	}
	case WM_CREATE:
		break;
	case WM_COMMAND:
		break;
	case WM_DESTROY: PostQuitMessage(0); break;
	case WM_CLOSE: DestroyWindow(hwnd); break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return FALSE;
}
