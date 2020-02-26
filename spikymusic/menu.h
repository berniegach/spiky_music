#ifndef MENU_H
#define MENU_H
#include <Windows.h>
#include <CommCtrl.h>
#include "resource.h"
#include <sstream>
#include <strsafe.h>
#include <ObjIdl.h>
#include <gdiplus.h>
#include <SDL.h>
#include "ffplay_variant.h"
#include <thread>
#include <future>
#include <exception>
#include "logger.h"
#include <Shlobj.h>
#include <ShlObj_core.h>
#include "Shlwapi.h"
#include <fstream>
#include <boost/algorithm/string/replace.hpp>
#include <algorithm>
#include "playlist_view.h"

#pragma comment (lib,"Gdiplus.lib")
#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"Shlwapi.lib")

using namespace Gdiplus;
using std::array;
using std::vector;
using std::wstring;
using std::fstream;
using std::ofstream;
using std::ifstream;
using std::ios;

class Menu
{
public:
	Menu();
	Menu(HWND parent, HINSTANCE hinstance, int parent_width, int parent_height);
	void Init(HWND parent, HINSTANCE hinstance, int parent_width, int parent_height);
	void check_config_files(HWND parent);
	void createMainButtons();
	void drawButtons(LPDRAWITEMSTRUCT pdis);
	void windowSizeChanged(HWND* hwnd);
	void mainButtonClicked(int id,HWND h_clicked);
	void paint(HDC* hdc,HWND* hwnd);
	LRESULT playlistview_notify(HWND hwnd, LPARAM lparam);
	typedef enum class SdlMusicOptions { SDL_SONG_QUIT, SDL_SONG_PLAY, SDL_SONG_PAUSE, SDL_SONG_SEEK } sdl_music_options;
	bool insert_listview_items(HWND h_listview, int c_items);
	HWND get_h_playlist_view();
	int send_sdl_music_event(SdlMusicOptions options, void* seek_fraction);
	void play_song(wstring song_path);
	void play_song_task(wstring song_path);
	void exit_playback();
	void update_stop_button(bool status);
	void exit();
	void set_song_duration_task();
	void update_song_time_elapsed_task();
	void displayLastErrorDebug(LPTSTR lpSzFunction);
	void close_song_gui();
	void progress_bar_clicked(HWND h_clicked);
	void write_to_prefs_file();
	void read_from_prefs_file();
	void update_header_menu_items();
	void menu_header_clicked(int id, HMENU parent_menu);
	void shuffle_songs();
	~Menu();
private:
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Uint32 sdl_event ;
	PlaylistView playlistView;

	struct SongStatus
	{
		enum class SongPlaying { SONG_PLAY_EMPTY, SONG_PLAY_PLAYING, SONG_PLAY_PAUSED };
		SongPlaying song_playing{ SongPlaying::SONG_PLAY_EMPTY };
		int song_number = 0;
	} song_status;
	vector<wstring>songs_to_play;
	vector<wstring>songs_to_play_temp;
	wchar_t w_config_file_path[MAX_PATH];
		
	//windows handles
	HWND h_parent;
	HINSTANCE hinst;
	int i_parent_width;
	int i_parent_height;
	int i_previous_btn_pressed = 0;
	bool b_previous_btn_checked = false;
	int i_which_main_btn_pressed = 0;
	HWND* h_previous_window_clicked;
	//no repeat = 0, repeat all = 1 and repeat one = 2
	int i_repeat_btn_status = 0;
	int i_shuffle_btn_status = 0;
	double d_play_running_time = 0;
	double d_play_remaining_time = 0;
	double d_play_total_time = 0;
	int i_progress_bar_y_pos = 0;
	//leftmost buttons 0-7
	HWND h_comments_btn{};
	HWND h_upload_btn{};
	HWND h_playlist_btn{};
	HWND h_favoriteslist_btn{};
	HWND h_favorites_add_btn{};
	HWND h_fullscreen_btn{};
	HWND h_repeat_btn{};
	HWND h_shuffle_btn{};
	HWND h_stop_btn{};
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
	//HWND h_favorites_add_large_btn{};
	HWND h_sdl_window{};
	HWND h_playlist_listview{};
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
	//const int i_favorites_add_large_btn_id{ 20 };
	const int i_sdl_window_id = 21;
	const int i_stop_btn_id = 22;
	const int i_playlist_listview_id = 23;

	std::thread thread_song;
	std::future<void> ft_play_song;
	std::future<void> ft_set_song_duration;
	std::future<void> ft_set_song_time_elapsed;

	struct ConfigFile
	{
		bool changed{ false };
		enum class PlayBackShowMode{ SHOW_MODE_NONE = -1, SHOW_MODE_VIDEO = 0, SHOW_MODE_WAVES, SHOW_MODE_RDFT, SHOW_MODE_NB } playback_show_mode;
		int i_play_repeat = 0;
		int i_play_shuffle = 0;
		enum class SomeValue { ONE, TWO, THREE } some_value;
	};
	ConfigFile s_config_file;
	int i_checked_graphics_menu = 0;
};
#endif

