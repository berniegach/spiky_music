#pragma once
#include <Windows.h>
#include "resource.h"
#include <CommCtrl.h>
#include <tchar.h>
#include <string>
#include <vector>

#pragma comment(lib,"Comctl32.lib")
using std::wstring;
using std::vector;

class PlaylistView
{
public:
	HWND CreateListView(HINSTANCE hInstance, HWND hwndParent);
	void ResizeListView(HWND hwndListView, int x, int y, int w, int h);
	void PositionHeader(HWND hwndListView);
	BOOL InitListView(HWND hwndListView);
	BOOL InsertListViewItems(HWND hwndListView, int size);
	LRESULT ListViewNotify(HWND hWnd, LPARAM lParam, vector<wstring> songs);
	void ErrorHandlerEx(WORD wLine, LPSTR lpszFile);
	void SwitchView(HWND hwndListView, DWORD dwView);
};

