#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include"favorites.h"
#pragma comment(lib, "comdlg32.lib")


class FileExplorer
{
public:
	FileExplorer();
	void openDialogWindow(int);
	~FileExplorer();
private:
	bool init_succeed{ false };
};

