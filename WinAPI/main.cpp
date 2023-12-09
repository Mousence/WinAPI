//WinAPI
#include <windows.h>

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow) {
	//Все типы данных API пишутся в верхнем регистре.
	//hInstance - это экземпляр рагруженного *.exe-файла.
	//lpCmdLine - командная строка,с которой был запущен *.exe-файл.
	//            В эту коммандную строку можно передавать параметры.
	//nCmdShow  - режим отображения окна:
	//          - свернуто на панель задач
	//          - свернуто в окно
	//          - развернуто на весь экран

	MessageBox(NULL, "Hello World!\nСюда можно писать всё что угодно :-)", "Это окно сообщения", MB_YESNOCANCEL | MB_ICONINFORMATION);
	//const char* - ANSI/ASCII string
	//LPCWSTR - LongPointer to Wide String
	//Wide string - это строка в кодировке Unicode (wchar_t)
	return 0;
}