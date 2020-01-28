#pragma once
#pragma once
#include<vector>
#include<string>
#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <algorithm>
#include "ffplay_variant.h"

#define BUFSIZE 4096 

class Favorites
{
public:
	Favorites(HWND);
	void initialAddFavorites(std::vector<std::wstring>&);
	~Favorites();
private:
	PROCESS_INFORMATION piProcInfo;
	HWND h_parent{};
};

