//WinAPI
#include <windows.h>

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow) {
	//��� ���� ������ API ������� � ������� ��������.
	//hInstance - ��� ��������� ������������ *.exe-�����.
	//lpCmdLine - ��������� ������,� ������� ��� ������� *.exe-����.
	//            � ��� ���������� ������ ����� ���������� ���������.
	//nCmdShow  - ����� ����������� ����:
	//          - �������� �� ������ �����
	//          - �������� � ����
	//          - ���������� �� ���� �����

	MessageBox(NULL, "Hello World!\n���� ����� ������ �� ��� ������ :-)", "��� ���� ���������", MB_YESNOCANCEL | MB_ICONINFORMATION);
	//const char* - ANSI/ASCII string
	//LPCWSTR - LongPointer to Wide String
	//Wide string - ��� ������ � ��������� Unicode (wchar_t)
	return 0;
}