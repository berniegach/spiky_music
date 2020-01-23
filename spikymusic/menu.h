#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include "resource.h"
#include <sstream>
#include <strsafe.h>
#include <ObjIdl.h>
#include <gdiplus.h>
#include "file_explorer.h"
#include <SDL.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#pragma comment(lib,"SDL2.lib")

class Menu
{
public:
	Menu();
	Menu(HWND* parent, HINSTANCE* hinstance, int parent_width, int parent_height);
	void Init(HWND* parent, HINSTANCE* hinstance, int parent_width, int parent_height);
	void createMainButtons();
	void drawButtons(LPDRAWITEMSTRUCT pdis);
	void windowSizeChanged(HWND* hwnd);
	void mainButtonClicked(int id,HWND h_clicked);
	void paint(HDC* hdc,HWND* hwnd);
	void displayLastErrorDebug(LPTSTR lpSzFunction);
	~Menu();
private:
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	//windows handles
	HWND* h_parent;
	HINSTANCE* hinst;
	int i_parent_width;
	int i_parent_height;
	int i_previous_btn_pressed = 0;
	bool b_previous_btn_checked = false;
	int i_which_main_btn_pressed = 0;
	HWND* h_previous_window_clicked;
	int i_repeat_btn_status = 0;
	int i_shuffle_btn_status = 0;
	double d_play_running_time = 1;
	double d_play_remaining_time = 2;
	double d_play_total_time = 3;
	//leftmost buttons 0-7
	HWND h_comments_btn{};
	HWND h_upload_btn{};
	HWND h_playlist_btn{};
	HWND h_favoriteslist_btn{};
	HWND h_favorites_add_btn{};
	HWND h_fullscreen_btn{};
	HWND h_repeat_btn{};
	HWND h_shuffle_btn{};
	//center buttons 8-10
	HWND h_previous_btn{};
	HWND h_play_btn{};
	HWND h_next_btn{};
	//rightmost buttons 11-12
	HWND h_sort_by_similarity_btn{};
	HWND h_sort_by_ratings_btn{};
	HWND h_search_similar_songs_btn{};
	HWND h_play_progress_bar[3];
	HWND h_play_time_txt[2];
	//main window buttons
	HWND h_favorites_add_large_btn{};
	HWND h_sdl_window{};
	//windows ids
	const int i_comments_btn_id{ 1 };
	const int i_upload_btn_id{2};
	const int i_playlist_btn_id{3};
	const int i_favoriteslist_btn_id{4};
	const int i_favorites_add_btn_id{5};
	const int i_fullscreen_btn_id{6};
	const int i_repeat_btn_id{7};
	const int i_shuffle_btn_id{8};
	const int i_previous_btn_id{ 9 };
	const int i_play_btn_id{ 10 };
	const int i_next_btn_id{ 11 };
	const int i_sort_by_similarity_btn{ 12 };
	const int i_sort_by_ratings_btn{ 13 };
	const int i_search_similar_songs_btn{ 14 };
	const int i_play_progress_bar_id[3]{ 15,16,17 };
	const int i_plat_time_txt_id[2]{ 18,19 };
	const int i_favorites_add_large_btn_id{ 20 };
	const int i_sdl_window_id = 21;
};

