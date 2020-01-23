#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include"favorites.h"
#pragma comment(lib, "comdlg32.lib")
#define INITIAL_LOAD_FAVORITES 1


class FileExplorer
{
public:
	FileExplorer();
	void openDialogWindow(int, HWND);
	~FileExplorer();
private:
	bool init_succeed{ false };
};

