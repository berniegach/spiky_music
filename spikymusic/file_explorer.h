#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include"favorites.h"

#pragma comment(lib, "comdlg32.lib")
#define INITIAL_LOAD_FAVORITES 1

using std::wstring;
using std::vector;
class FileExplorer
{
public:
	FileExplorer();
	vector<wstring> find_songs_to_play(HWND parent);
	~FileExplorer();
private:
	bool init_succeed{ false };
};

