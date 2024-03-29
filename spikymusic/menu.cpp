#include "file_explorer.h"
#include "menu.h"



Menu::Menu()
{
	//default constructor
	//initialize GDI+
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	s_config_file.playback_show_mode = ConfigFile::PlayBackShowMode::SHOW_MODE_WAVES;
	s_config_file.some_value = ConfigFile::SomeValue::THREE;
	s_config_file.i_play_repeat = 0;
	s_config_file.i_play_shuffle = 0;
}
Menu::Menu(HWND parent, HINSTANCE hinstance,int parent_width,int parent_height)
	:h_parent(parent),	hinst(hinstance),i_parent_width(parent_width),i_parent_height(parent_height)
{
	//initialize GDI+
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	//sdl_event= SDL_RegisterEvents(1);
	s_config_file.playback_show_mode = ConfigFile::PlayBackShowMode::SHOW_MODE_WAVES;
	s_config_file.some_value = ConfigFile::SomeValue::THREE;
	s_config_file.i_play_repeat = 0;
	s_config_file.i_play_shuffle = 0;
	
	createMainButtons();
}
void Menu::Init(HWND parent, HINSTANCE hinstance, int parent_width, int parent_height)
{
	h_parent = parent;
	hinst = hinstance;
	i_parent_width = parent_width;
	i_parent_height = parent_height;
	
	check_config_files(parent);
	read_from_prefs_file();
	update_header_menu_items();
	//create the buttons
	createMainButtons();
}
/*
check if configuration and preferences file are available
if not create them
*/
void Menu::check_config_files(HWND parent)
{
	bool directory_exists{ false }, file_exists{ false };
	HANDLE h_file_prefs{};

	if (SHGetSpecialFolderPathW(parent, w_config_file_path, CSIDL_APPDATA, false))
	{
		PathAppend(w_config_file_path, TEXT("stellerwave"));
		if (PathFileExistsW(w_config_file_path))
		{
			directory_exists = true;
		}
		else
		{
			//If lpSecurityAttributes is NULL, the directory gets a default security descriptor. 
			//The ACLs in the default security descriptor for a directory are inherited from its parent directory
			if (CreateDirectory(w_config_file_path, NULL))
			{
				directory_exists = true;
			}
			else
				OutputDebugStringW((LPCWSTR)GetLastError());
		}
		if (directory_exists)
		{
			PathAppend(w_config_file_path, TEXT("prefs.dat"));
			if (PathFileExistsW(w_config_file_path))
			{
				file_exists = true;
			}
			else
			{
				h_file_prefs = CreateFile(w_config_file_path,                // name of the write
					GENERIC_READ | GENERIC_WRITE,          // open for writing
					0,                      // do not share
					NULL,                   // default security
					CREATE_NEW,             // create new file only
					FILE_ATTRIBUTE_HIDDEN ,  // hidden file
					NULL);
				if (h_file_prefs == INVALID_HANDLE_VALUE)
					OutputDebugStringW((LPCWSTR)GetLastError());
				else
				{
					CloseHandle(h_file_prefs);
					file_exists = true;
					//since weve just created the file, it is empty with no defaults so we need to add them 
					s_config_file.changed = true;
				}
			}
		}
		if (file_exists)
		{
			
			if (s_config_file.changed)
				write_to_prefs_file();
		}
			
	}
}
/*
this function draws the main menu buttons at the bottom of the screen. We draw the buttons starting with the bottom left heading rightwards
*/
void Menu::createMainButtons()
{
	int i_menu_bar_height = GetSystemMetrics(SM_CYMENU);
	int i_title_bar_height= GetSystemMetrics(SM_CXSCREEN);
	RECT rect{};
	GetClientRect(h_parent, &rect);	

	int i_btn_width=30, i_btn_height = 30;
	int i_distance_between = 10;
	int i_total_left_btns_width = (8 * i_distance_between) + (8 * i_btn_width);
	int i_total_center_btns_width= (3 * i_distance_between) + (3 * i_btn_width);
	int i_total_right_btns_width= (3 * i_distance_between) + (3 * i_btn_width);

	HWND* hwnds[]{ &h_comments_btn, &h_upload_btn, &h_playlist_btn, &h_favoriteslist_btn, &h_favorites_add_btn, &h_fullscreen_btn, &h_repeat_btn, &h_shuffle_btn, &h_previous_btn, &h_play_btn, &h_next_btn,
	&h_sort_by_similarity_btn, &h_sort_by_ratings_btn, &h_search_similar_songs_btn };
	int ids[]{ i_comments_btn_id, i_upload_btn_id, i_playlist_btn_id, i_favoriteslist_btn_id, i_favorites_add_btn_id, i_fullscreen_btn_id, i_repeat_btn_id ,i_shuffle_btn_id, i_previous_btn_id, i_play_btn_id, i_next_btn_id,
	i_sort_by_similarity_btn, i_sort_by_ratings_btn, i_search_similar_songs_btn };
	//first draw the left most buttons
	int i_x = 5;
	int i_y = rect.bottom-35;
	//create the leftmost buttons
	for (int c = 0; c <= 7; c++)
	{
		*hwnds[c] = CreateWindow(WC_BUTTON, TEXT(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, i_x, i_y, i_btn_width, i_btn_height, h_parent, (HMENU)ids[c], hinst, NULL);
		i_x += i_btn_width + i_distance_between;
	}
	//create the center buttons
	long l_x_center = rect.right / 2;
	i_x = l_x_center - (i_btn_width + i_distance_between + (i_btn_width / 2));
	//lets take advantage that we have the center to create the stop button which is left of the center buttons
	//the stop button starts as disbaled since we dont have a song playing
	long i_x_stop = i_x - (i_btn_width + i_distance_between );
	h_stop_btn = CreateWindow(WC_BUTTON, TEXT(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, i_x_stop, i_y, i_btn_width, i_btn_height, h_parent, (HMENU)i_stop_btn_id, hinst, NULL);
	EnableWindow(h_stop_btn, false);
	//create the three center buttons
	//previous, play, and next
	for (int c = 8; c <= 10; c++)
	{
		*hwnds[c] = CreateWindow(WC_BUTTON, TEXT(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, i_x, i_y, i_btn_width, i_btn_height, h_parent, (HMENU)ids[c], hinst, NULL);
		i_x += i_btn_width + i_distance_between;
	}
	//create the rightmost buttons
	i_x = rect.right - i_btn_width - 5;
	for (int c = 13; c >= 11; c--)
	{
		*hwnds[c] = CreateWindow(WC_BUTTON, TEXT(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, i_x, i_y, i_btn_width, i_btn_height, h_parent, (HMENU)ids[c], hinst, NULL);
		i_x -= i_btn_width + i_distance_between;
	}
	//create the progress bar
	//the progress bar is made up of three buttons
	//the dark colored that shows ellapsed time, light colored that shows remaining time and middle circle button that shows the progress thumb
	i_x = 10;
	i_y -= 6;
	int i_progress_left_w = (rect.right - 20) / 2-4;
	int i_progress_right_w= (rect.right - 20) / 2;
	int i_progress_middle_w = 8;
	int i_progress_h = 4;
	h_play_progress_bar[0]= CreateWindow(WC_BUTTON, TEXT(""), WS_CHILD  | BS_OWNERDRAW, i_x, i_y, i_progress_left_w, i_progress_h, h_parent, (HMENU)i_play_progress_bar_id[0], hinst, NULL);
	i_x += i_progress_left_w;
	h_play_progress_bar[1] = CreateWindow(WC_BUTTON, TEXT(""), WS_CHILD | BS_OWNERDRAW, i_x+8, i_y, i_progress_right_w, i_progress_h, h_parent, (HMENU)i_play_progress_bar_id[1], hinst, NULL);
	i_y -= 2;
	h_play_progress_bar[2] = CreateWindow(WC_BUTTON, TEXT(""), WS_CHILD | BS_OWNERDRAW, i_x, i_y, i_progress_middle_w, i_progress_middle_w, h_parent, (HMENU)i_play_progress_bar_id[2], hinst, NULL);
	//create the time labels
	i_x = 10;
	i_y -= 10;
	int i_time_w = 50;
	int i_time_h = 12;
	h_play_time_txt[0]= CreateWindow(WC_STATIC, TEXT("00:00:00"), WS_CHILD | SS_LEFT , i_x, i_y, i_time_w, i_time_h, h_parent, (HMENU)i_plat_time_txt_id[0], hinst, NULL);
	i_x = rect.right - 10 - 50;
	h_play_time_txt[1] = CreateWindow(WC_STATIC, TEXT("00:00:00"), WS_CHILD | SS_RIGHT, i_x, i_y, i_time_w, i_time_h, h_parent, (HMENU)i_plat_time_txt_id[1], hinst, NULL);
	//create the window where sdl window will be attached to
	//make it invisible
	h_sdl_window = CreateWindow(WC_STATIC, TEXT(""), WS_CHILD | WS_VISIBLE, 0, 0, rect.right, i_y, h_parent, (HMENU)i_sdl_window_id, hinst, NULL);
	ShowWindow(h_sdl_window, false);
	//create the playlist listview
	//make it invisible
	//h_playlist_listview = CreateWindow(WC_LISTVIEW, TEXT(""), WS_CHILD , 0, 0, rect.right, i_y, h_parent, (HMENU)i_playlist_listview_id, hinst, NULL);
	h_playlist_listview = playlistView.CreateListView(hinst, h_parent);
	//initialize the TreeView control
	playlistView.InitListView(h_playlist_listview);
}
void Menu::drawButtons(LPDRAWITEMSTRUCT pdis)
{
	int cx, cy;
	HDC  hdcMem;
	BITMAP bitmap;
	HBITMAP hBitmap;
	HBRUSH h_brush;
	//these are the two state buttons which change according to each other status
	switch (pdis->CtlID)
	{
		case 1:
			hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_COMMENT_NORMAL));
			break;
		case 2:
			hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_UPLOAD_NORMAL));
			break;
		case 3:
			hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_PLAYLIST_NORMAL));
			break;
		case 4:
			hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_FAVORITE_LIST_NORMAL));
			break;
		case 5:
			hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_FAVORITE_ADD_NORMAL));
			break;
		case 6:
			hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_FULLSCREEN_NORMAL));
			break;
		case 7:
			if (s_config_file.i_play_repeat == 0)
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_REPEAT_ALL_NORMAL));
			else if (s_config_file.i_play_repeat == 1)
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_REPEAT_ALL_PRESSED));
			else 
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_REPEAT_ONE_PRESSED));
			break;
		case 8:
			if(s_config_file.i_play_shuffle==0)
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_SHUFFLE_NORMAL));
			else
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_SHUFFLE_PRESSED));
			break;
		case 9:
			hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_PREVIOUS_NORMAL));
			break;
		case 10:

			song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_PLAYING ?
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_PAUSE_W_NORMAL)) :
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_PLAY_NORMAL));
			break;
		case 11:
			hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_NEXT_NORMAL));
			break;
		case 12:
			hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_SORT_BY_SIMILARITY_NORMAL));
			break;
		case 13:
			hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_SORT_BY_RATINGS_NORMAL));
			break;
		case 14:
			hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_SCAN_NORMAL));
			break;
		case 15:
			h_brush = CreateSolidBrush(RGB(150, 150, 150));
			SelectObject(pdis->hDC, h_brush);
			FillRect(pdis->hDC, &pdis->rcItem,h_brush);
			hBitmap = nullptr;
			DeleteObject(h_brush);
			break;
		case 16:
			h_brush = CreateSolidBrush(RGB(220, 220, 220));
			SelectObject(pdis->hDC, h_brush);
			FillRect(pdis->hDC, &pdis->rcItem, h_brush);
			hBitmap = nullptr;
			DeleteObject(h_brush);
			break;
		case 17:
			hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_PROGRESS_BAR_CIRCLE_NORMAL));			
			break;
		case 20:
			hBitmap= LoadBitmap(hinst, MAKEINTRESOURCE(IDB_FAVORITE_ADD_NORMAL_LARGE));
			break;
		case 22:
			song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_EMPTY ?
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_STOP_DISABLED)) :
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_STOP_NORMAL));
			break;
		default:
			hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_COMMENT_NORMAL));

	}
	if (pdis->CtlID >= 15 && pdis->CtlID <= 16)
		return;
	GetObject(hBitmap, sizeof(BITMAP), &bitmap);
	hdcMem = CreateCompatibleDC(pdis->hDC);
	SelectObject(hdcMem, hBitmap);
	//stretch the resulting bitmap to fit the corresponding size
	if(pdis->CtlID==17)
		StretchBlt(pdis->hDC, pdis->rcItem.left, pdis->rcItem.top, pdis->rcItem.right - pdis->rcItem.left, pdis->rcItem.bottom - pdis->rcItem.top, hdcMem, 0, 0, 8, 8, SRCCOPY);
	else if(pdis->CtlID == 20)
		StretchBlt(pdis->hDC, pdis->rcItem.left, pdis->rcItem.top, pdis->rcItem.right - pdis->rcItem.left, pdis->rcItem.bottom - pdis->rcItem.top, hdcMem, 0, 0, 100, 100, SRCCOPY);
	else
		StretchBlt(pdis->hDC, pdis->rcItem.left, pdis->rcItem.top, pdis->rcItem.right - pdis->rcItem.left, pdis->rcItem.bottom - pdis->rcItem.top, hdcMem, 0, 0, 30, 30, SRCCOPY);
	// if the button is selected
	if (pdis->itemState & ODS_SELECTED)
	{
		switch (pdis->CtlID)
		{
			case 1:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_COMMENT_PRESSED));
				break;
			case 2:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_UPLOAD_PRESSED));
				break;	
			case 3:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_PLAYLIST_PRESSED));
				break;
			case 4:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_FAVORITE_LIST_PRESSED));
				break;
			case 5:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_FAVORITE_ADD_PRESSED));
				break;
			case 6:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_FULLSCREEN_PRESSED));
				break;
			case 7:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_REPEAT_ALL_PRESSED));
				break;
			case 8:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_SHUFFLE_PRESSED));
				break;
			case 9:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_PREVIOUS_PRESSED));
				break;
			case 10:
				song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_PLAYING ?
					hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_PAUSE_W_PRESSED)) :
					hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_PLAY_PRESSED));
				break;
			case 11:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_NEXT_PRESSED));
				break;
			case 12:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_SORT_BY_SIMILARITY_PRESSED));
				break;
			case 13:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_SORT_BY_RATINGS_PRESSED));
				break;
			case 14:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_SCAN_PRESSED));
				break;
			case 20:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_FAVORITE_ADD_PRESSED_LARGE));
				break;
			case 22:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_STOP_PRESSED));
				break;
			default:
				hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_COMMENT_PRESSED));

		}
		GetObject(hBitmap, sizeof(BITMAP), &bitmap);
		hdcMem = CreateCompatibleDC(pdis->hDC);
		SelectObject(hdcMem, hBitmap);
		if (pdis->CtlID == 20)
			StretchBlt(pdis->hDC, pdis->rcItem.left, pdis->rcItem.top, pdis->rcItem.right - pdis->rcItem.left, pdis->rcItem.bottom - pdis->rcItem.top, hdcMem, 0, 0, 100, 100, SRCCOPY);
		else
			StretchBlt(pdis->hDC, pdis->rcItem.left, pdis->rcItem.top, pdis->rcItem.right - pdis->rcItem.left, pdis->rcItem.bottom - pdis->rcItem.top, hdcMem, 0, 0, 30, 30, SRCCOPY);
	}
	// Draw a focus rectangle if the button has the focus
    if (pdis->itemState & ODS_FOCUS)
	{
		// Draw inward and outward black triangles
		cx = pdis->rcItem.right - pdis->rcItem.left;
		cy = pdis->rcItem.bottom - pdis->rcItem.top;
		pdis->rcItem.left += cx / 16;
		pdis->rcItem.top += cy / 16;
		pdis->rcItem.right -= cx / 16;
		pdis->rcItem.bottom -= cy / 16;
		DrawFocusRect(pdis->hDC, &pdis->rcItem);
	}

	ReleaseDC(pdis->hwndItem, hdcMem);
}
void Menu::windowSizeChanged(HWND* hwnd)
{
	
	//we need to check the new dimensions against the total size of the buttons
	//there are three primary button groups, leftmost, center and rightmost
	//if the new size can fit all three we show them, if smaller than that we show leftmost and center, if smaller we show center which is the smallest size of the window
	HWND* hwnds[]{ &h_comments_btn, &h_upload_btn, &h_playlist_btn, &h_favoriteslist_btn, &h_favorites_add_btn, &h_fullscreen_btn, &h_repeat_btn, &h_shuffle_btn, &h_previous_btn, &h_play_btn, &h_next_btn,
	&h_sort_by_similarity_btn, &h_sort_by_ratings_btn, &h_search_similar_songs_btn };
	int ids[]{ i_comments_btn_id, i_upload_btn_id, i_playlist_btn_id, i_favoriteslist_btn_id, i_favorites_add_btn_id, i_fullscreen_btn_id, i_repeat_btn_id ,i_shuffle_btn_id, i_previous_btn_id, i_play_btn_id, i_next_btn_id,
	i_sort_by_similarity_btn, i_sort_by_ratings_btn, i_search_similar_songs_btn };

	RECT rect{};
	if(!GetClientRect(*hwnd, &rect))
		displayLastErrorDebug((LPTSTR)L"MoveWindow");
	const RECT c_rect = rect;
	int i_btn_width = 30, i_btn_height = 30;
	int i_distance_between = 10;
	int i_total_left_btns_width = (8 * i_distance_between) + (8 * i_btn_width);
	int i_total_center_btns_width = (4 * i_distance_between) + (4 * i_btn_width);
	int i_total_right_btns_width = (3 * i_distance_between) + (3 * i_btn_width);
	int i_x = 0, i_y = rect.bottom - 35;
	//check if we can fit all of them
	//if not hide the leftmost buttons
	if (rect.right >= (i_total_left_btns_width + i_total_center_btns_width + i_total_right_btns_width) || rect.right >= (i_total_left_btns_width + i_total_center_btns_width))
	{
		//move the leftmost buttons
		int i_x = 5;
		for (int c = 0; c <= 7; c++)
		{
			if (!IsWindowVisible(*hwnds[c]))
				ShowWindow(*hwnds[c], true);
			MoveWindow(*hwnds[c], i_x, i_y, i_btn_width, i_btn_height, true);
			i_x += i_btn_width + i_distance_between;
		}
	}
	else
		for (int c = 0; c <= 7; c++)
			ShowWindow(*hwnds[c], false);
	if (rect.right >=  i_total_center_btns_width )
	{
		//move the center buttons
		//check if we are showing the leftmost and center buttons
		if (rect.right >= (i_total_left_btns_width + i_total_center_btns_width ) && rect.right < (i_total_left_btns_width + i_total_center_btns_width + i_total_right_btns_width ))
		{
			i_x = rect.right - i_btn_width - 5;
			for (int c = 10; c >= 8; c--)
			{
				if (!IsWindowVisible(*hwnds[c]))
					ShowWindow(*hwnds[c], true);
				MoveWindow(*hwnds[c], i_x, i_y, i_btn_width, i_btn_height, true);
				i_x -= i_btn_width + i_distance_between;
			}
			//move the stop button
			if (!IsWindowVisible(h_stop_btn))
				ShowWindow(h_stop_btn, true);
			MoveWindow(h_stop_btn, i_x, i_y, i_btn_width, i_btn_height, true);
		}
		else
		{
			long l_x_center = rect.right / 2;
			i_x = l_x_center - (i_btn_width + i_distance_between + (i_btn_width / 2));
			long i_x_stop = i_x - (i_btn_width + i_distance_between);
			//move the stop button
			MoveWindow(h_stop_btn, i_x_stop, i_y, i_btn_width, i_btn_height, true);
			for (int c = 8; c <= 10; c++)
			{
				MoveWindow(*hwnds[c], i_x, i_y, i_btn_width, i_btn_height, true);
				i_x += i_btn_width + i_distance_between;
			}
		}
		
	}
	else
	{
		//we cant fit the center buttons so we hide them
		for (int c = 8; c <= 10; c++)
			ShowWindow(*hwnds[c], false);
		ShowWindow(h_stop_btn, false);
	}
	if (rect.right >= (i_total_left_btns_width + i_total_center_btns_width + i_total_right_btns_width))
	{
		//move the rightmost buttons
		i_x = rect.right - i_btn_width - 5;
		for (int c = 13; c >= 11; c--)
		{
			if (!IsWindowVisible(*hwnds[c]))
				ShowWindow(*hwnds[c], true);
			MoveWindow(*hwnds[c], i_x, i_y, i_btn_width, i_btn_height, true);
			i_x -= i_btn_width + i_distance_between;
		}
	}
	else
		for (int c = 13; c >= 11; c--)
			ShowWindow(*hwnds[c], false);
	//move the play progress bar
	i_x = 10;
	i_y -= 6;
	i_progress_bar_y_pos = i_y;
	int i_progress_left_w = (rect.right - 20) *d_play_running_time/d_play_total_time-4;
	int i_progress_right_w = (rect.right - 20) *d_play_remaining_time/d_play_total_time;
	int i_progress_middle_w = 8;
	int i_progress_h = 4;
	MoveWindow(h_play_progress_bar[0], i_x, i_y, i_progress_left_w, i_progress_h, true);
	i_x += i_progress_left_w;
	MoveWindow(h_play_progress_bar[1], i_x+8, i_y, i_progress_right_w, i_progress_h, true);
	i_y -= 2;
	MoveWindow(h_play_progress_bar[2], i_x, i_y, i_progress_middle_w, i_progress_middle_w, true);
	//move the time labels
	i_x = 10;
	i_y -= 10;
	int i_time_w = 50;
	int i_time_h = 12;
	MoveWindow(h_play_time_txt[0], i_x, i_y, i_time_w, i_time_h, true);
	i_x= rect.right - 10 - 50;
	MoveWindow(h_play_time_txt[1], i_x, i_y, i_time_w, i_time_h, true);
	//move the sdl background window
	//if (IsWindowVisible(h_sdl_window))
		MoveWindow(h_sdl_window, 0, 0, rect.right, i_y, true);
	//move the playlist view window
		int i_list_y = rect.bottom - 60;
	playlistView.ResizeListView(h_playlist_listview, 200, 0, rect.right, i_list_y);
	ListView_SetColumnWidth(h_playlist_listview, 2, LVSCW_AUTOSIZE_USEHEADER);
	
}
void Menu::mainButtonClicked(int id,HWND h_clicked)
{
	
	using std::future;
	using std::async;
	using std::launch;
	i_which_main_btn_pressed = id;
	
	if (id == i_repeat_btn_id)
	{
		//there are three states in the repeat button
		//no repeat, repeat one and repeat all
		s_config_file.i_play_repeat = (s_config_file.i_play_repeat + 1) % 3;
		s_config_file.i_play_repeat = s_config_file.i_play_repeat;
		s_config_file.changed = true;
		InvalidateRect(h_clicked, NULL, true);
		UpdateWindow(h_clicked);
	}
	else if (id == i_shuffle_btn_id)
	{
		//there are two states in the shuffle button
		//shuffle and dont shuffle
		s_config_file.i_play_shuffle = (s_config_file.i_play_shuffle + 1) % 2;
		shuffle_songs();
		InvalidateRect(h_clicked, NULL, true);
		UpdateWindow(h_clicked);
	}
	else if (id == i_play_btn_id)
	{
		//if there are no songs in the que find the song to play
		if (song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_EMPTY)
		{
			FileExplorer file_explorer;
			//make sure file_explorer is not cancelled
			if ((songs_to_play = file_explorer.find_songs_to_play(GetParent(h_clicked))).empty())
				return;
			//update the playlist view with the songs
			playlistView.InsertListViewItems(h_playlist_listview, songs_to_play.size() == 1 ? 1 : songs_to_play.size() - 1);
			//check if we are allowed to shuffle the songs
			shuffle_songs();
			//if we are at playlist view hide it
			//ShowWindow(h_playlist_listview, IsWindowVisible(h_playlist_listview) ? false : true);

			if (songs_to_play.size() == 1)
			{
				play_song(songs_to_play.at(0));
			}
			else
			{
				wstring directory = songs_to_play.at(0);
				wstring song_path = directory + L"\\" + songs_to_play.at(++song_status.song_number);
				play_song(song_path);
			}
		}
		else
		{
			//first we check if we are playing the first song
			//or we are proceeding to play a song
			if (song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_PLAYING)
			{
				if (send_sdl_music_event(SdlMusicOptions::SDL_SONG_PAUSE, 0))
					song_status.song_playing = SongStatus::SongPlaying::SONG_PLAY_PAUSED;
			}
			else if (song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_PAUSED)
			{
				if (send_sdl_music_event(SdlMusicOptions::SDL_SONG_PLAY, 0))
					song_status.song_playing = SongStatus::SongPlaying::SONG_PLAY_PLAYING;
			}
			//we refresh the playing button so that when we click the button a second time it reflects the pause intent
			InvalidateRect(h_clicked, NULL, true);
			UpdateWindow(h_clicked);

		}

	}
	else if (id == i_next_btn_id)
	{
		if (songs_to_play.size() > song_status.song_number + 1)
			;
		else if (songs_to_play.size() == song_status.song_number + 1 && s_config_file.i_play_repeat == 1 && songs_to_play.size() != 1)
			song_status.song_number = 0;
		else if (songs_to_play.size() == 1)
			return;
		else
			return;
		//first lets quit the current song
		//we first call the song time tasks before ending the song to make sure the main thread doesn't hang
		song_status.song_playing = SongStatus::SongPlaying::SONG_PLAY_EMPTY;
		ft_set_song_time_elapsed.get();
		ft_set_song_duration.get();
		send_sdl_music_event(SdlMusicOptions::SDL_SONG_QUIT, 0);
		ft_play_song.get();

		//now lets play the next song
		wstring directory = songs_to_play.at(0);
		wstring song_path = directory + L"\\" + songs_to_play.at(++song_status.song_number);
		play_song(song_path);
	}
	else if (id == i_previous_btn_id)
	{
		if (song_status.song_number - 1 > 0)
		{
			//first lets quit the current song
			//we first call the song time tasks before ending the song to make sure the main thread doesn't hang
			song_status.song_playing = SongStatus::SongPlaying::SONG_PLAY_EMPTY;
			ft_set_song_time_elapsed.get();
			ft_set_song_duration.get();
			send_sdl_music_event(SdlMusicOptions::SDL_SONG_QUIT, 0);
			ft_play_song.get();
			//now lets play the next song
			wstring directory = songs_to_play.at(0);
			wstring song_path = directory + L"\\" + songs_to_play.at(--song_status.song_number);
			play_song(song_path);
		}
	}
	else if (id == i_stop_btn_id)
	{
		exit_playback();
	}
	else if (id == i_play_progress_bar_id[0] || id == i_play_progress_bar_id[1] || id == i_play_progress_bar_id[2])
	{
		progress_bar_clicked(h_clicked);
	}
	else if (id == i_playlist_btn_id)
	{
		/*if (!IsWindowVisible(h_playlist_listview))
			AnimateWindow(h_playlist_listview, 2000, AW_CENTER);
		else
			AnimateWindow(h_playlist_listview, 2000, AW_CENTER | AW_HIDE);*/
			if (song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_EMPTY)
			{
				ShowWindow(h_playlist_listview, IsWindowVisible(h_playlist_listview) ? false : true);
			}
			else
			{
				ShowWindow(h_playlist_listview, IsWindowVisible(h_playlist_listview) ? false : true);
				ShowWindow(h_sdl_window, IsWindowVisible(h_sdl_window) ? false : true);
			}
	}
}

void Menu::paint(HDC* hdc, HWND* hwnd)
{
	RECT rect;
	GetClientRect(*hwnd, &rect); 
	Graphics graphics(*hdc);
	//draw the bottom menu bar bounding box
	Pen pen(Color(220, 220, 220));
	int i_x = 1;
	int i_y = rect.bottom - 60;
	int i_w = rect.right - 3;
	int i_h = rect.bottom - i_y - 2;
	graphics.DrawRectangle(&pen,i_x,i_y,i_w,i_h);
	//draw the play background
	i_x = 1;
	i_h = i_y-1;
	i_y = 1;
	i_w = rect.right-1;
	SolidBrush solid_brush(Color(230, 23, 230));
	//graphics.FillRectangle(&solid_brush, i_x, i_y, i_w, i_h);
	int i_lancher_image_w = 100;
	i_x = rect.right / 2 - i_lancher_image_w / 2;
	i_y = rect.bottom / 2 - i_lancher_image_w / 2;
	HBITMAP bitmap= (HBITMAP)LoadImage(hinst, MAKEINTRESOURCE(IDB_LAUNCHER_100), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	HDC hMemDC = CreateCompatibleDC(*hdc);
	SelectObject(hMemDC, bitmap);
	BitBlt(*hdc, i_x, i_y, 100, 100, hMemDC, 0, 0, SRCCOPY);
	DeleteDC(hMemDC);
}

LRESULT Menu::playlistview_notify(HWND hwnd, LPARAM lparam)
{
	return playlistView.ListViewNotify(hwnd, lparam, songs_to_play);
}
// InsertListViewItems: Inserts items into a list view. 
// hWndListView:        Handle to the list-view control.
// cItems:              Number of items to insert.
// Returns TRUE if successful, and FALSE otherwise.
bool Menu::insert_listview_items(HWND h_listview, int c_items)
{
	LVITEM lvI;

	// Initialize LVITEM members that are common to all items.
	lvI.pszText = LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
	lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvI.stateMask = 0;
	lvI.iSubItem = 0;
	lvI.state = 0;

	// Initialize LVITEM members that are different for each item.
	for (int index = 0; index < c_items; index++)
	{
		lvI.iItem = index;
		lvI.iImage = index;

		// Insert items into the list.
		if (ListView_InsertItem(h_listview, &lvI) == -1)
			return false;
	}

	return true;
}
HWND Menu::get_h_playlist_view()
{
	return h_playlist_listview;
}
int Menu::send_sdl_music_event(SdlMusicOptions options, void* seek_fraction)
{
	SDL_Event sdlevent;
	int i_return = 0;
	switch (options)
	{
	case SdlMusicOptions::SDL_SONG_QUIT:
		sdlevent.type = SDL_KEYDOWN;
		sdlevent.key.keysym.sym = SDLK_q;
		i_return = SDL_PushEvent(&sdlevent);
		break;
	case SdlMusicOptions::SDL_SONG_PAUSE:
		sdlevent.type = SDL_KEYDOWN;
		sdlevent.key.keysym.sym = SDLK_SPACE;
		i_return = SDL_PushEvent(&sdlevent);
		break;
	case SdlMusicOptions::SDL_SONG_PLAY:
		sdlevent.type = SDL_KEYDOWN;
		sdlevent.key.keysym.sym = SDLK_SPACE;
		i_return = SDL_PushEvent(&sdlevent);
		break;
	case SdlMusicOptions::SDL_SONG_SEEK:
	{
		//Uint32 sdl_event= sdl_event = SDL_RegisterEvents(1);
		if (Ffplay::sdl_my_event != ((Uint32)-1))
		{
			SDL_Event event;
			SDL_memset(&event, 0, sizeof(event)); /* or SDL_zero(event) */
			//since SDL_USEREVENT is registered as 0x8000 which is 32768 we use it instead creating our own user event
			event.type = SDL_USEREVENT;// Ffplay::sdl_my_event;
			event.user.code = 1;
			event.user.data1 = seek_fraction;
			i_return = SDL_PushEvent(&event);
		}
		
	}
	break;
	}
	return i_return;
}
void Menu::play_song(wstring song_path)
{
	using std::future;
	using std::async;
	using std::launch;

	for (int c = 0; c <= 2; c++)
		ShowWindow(h_play_progress_bar[c], true);
	for (int c = 0; c <= 1; c++)
		ShowWindow(h_play_time_txt[c], true);
	
	wchar_t w_str[256];
	wsprintf(w_str, L"%s", song_path.c_str());
	
	wchar_t* buffer;
	wchar_t* token = wcstok_s(w_str, L"\\", &buffer);
	std::vector<wchar_t*>v_str;
	while (token) {
		token = wcstok_s(NULL, L"\\", &buffer);
		v_str.push_back(token);
	}
	//this method needs to be delivered synchronously 
	//That's because the window manager needs to be able to manage the lifetime of the text data
	SetWindowText(h_parent, v_str.at(v_str.size() - 2));
	ft_play_song = std::async(launch::async, &Menu::play_song_task, this, song_path);
	if (i_which_main_btn_pressed != i_previous_btn_id && i_which_main_btn_pressed != i_next_btn_id)
		update_stop_button(true);

	ft_set_song_duration = std::async(launch::async, &Menu::set_song_duration_task, this);
	ft_set_song_time_elapsed = std::async(launch::async, &Menu::update_song_time_elapsed_task, this);
}
void Menu::play_song_task(wstring song_path)
{
	using std::future;
	using std::async;
	using std::launch;

	Ffplay ffplay;
	string input{ song_path.begin(),song_path.end() };
	
	//lets plays the song
	song_status.song_playing = SongStatus::SongPlaying::SONG_PLAY_PLAYING;	
	ffplay.play_song(input, h_sdl_window, (VideoState::ShowMode) s_config_file.playback_show_mode);
	//the song is not playing anymore
	song_status.song_playing = SongStatus::SongPlaying::SONG_PLAY_EMPTY;
}
void Menu::exit_playback()
{
	send_sdl_music_event(SdlMusicOptions::SDL_SONG_QUIT, 0);
	ft_play_song.get();
	update_stop_button(false);
	songs_to_play.clear();
	song_status.song_number = 0;
	close_song_gui();
	SetWindowTextW(h_parent, L"StellerWave");
	playlistView.InsertListViewItems(h_playlist_listview, songs_to_play.size() == 1 ? 1 : songs_to_play.size() - 1);
}
/*
update the stop button to reflect the status of the song
NB: ALL GUI updating MUST be done in the main thread. if we do it in the async the app will hang*/
void Menu::update_stop_button(bool enable)
{
	
	EnableWindow(h_stop_btn, enable);
	SetFocus(NULL);
	InvalidateRect(h_stop_btn, NULL, true);
	UpdateWindow(h_stop_btn);
	ShowWindow(h_sdl_window, false);
}
/*
we place exit functions here because when we close the main window the destructor is not called properly
so we instead call this function to close
*/
void Menu::exit()
{
	//first we check if there is a song playing we shut it down
	if (song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_PLAYING || song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_PAUSED)
	{
		send_sdl_music_event(SdlMusicOptions::SDL_SONG_QUIT, 0);
	}
}
void Menu::set_song_duration_task()
{
	int hours, mins, secs, us;
	int64_t duration, time;
	wchar_t total_time[64];
	wchar_t start_time[64];
	for (;;)
	{
		if (song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_EMPTY)
			break;
		if (Ffplay::is_song_duration_set())
		{
			duration = Ffplay::get_song_duration();
			time = duration + (duration <= INT64_MAX - 5000 ? 5000 : 0);
			secs = time / AV_TIME_BASE;
			us = time % AV_TIME_BASE;
			mins = secs / 60;
			secs %= 60;
			hours = mins / 60;
			mins %= 60;
			if (hours > 0)
			{
				wsprintf(total_time, L"%d:%d:%d", hours, mins, secs);
				wsprintf(start_time, L"00:00:00");
			}
			else if (mins > 0)
			{
				wsprintf(total_time, L"%d:%d", mins, secs);
				wsprintf(start_time, L"00:00");
			}
			else
			{
				wsprintf(total_time, L"%d", secs);
				wsprintf(start_time, L"00");
			}
			SetWindowText(h_play_time_txt[0], start_time);
			SetWindowText(h_play_time_txt[1], total_time);
			break;
		}
	}
}
/*
this functions get the time the song has been playing and updates it on the GUI
*/
void Menu::update_song_time_elapsed_task()
{
	int64_t duration, time;
	double total_secs = 0;
	wchar_t total_time[64];
	bool song_finished{ false };
	for(;;)
		if (Ffplay::is_song_duration_set())
		{
			duration = Ffplay::get_song_duration();
			time = duration + (duration <= INT64_MAX - 5000 ? 5000 : 0);
			total_secs = time / AV_TIME_BASE;
			break;
		}
	d_play_total_time = total_secs;
	for (;;)
	{
		if (song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_EMPTY)
			break;

		double secs_elapsed = Ffplay::get_time_played_in_secs();
		double* frac = (double*)malloc(sizeof(double));
		*frac = 0.0;
		if (secs_elapsed >= d_play_total_time)
		{
			if (s_config_file.i_play_repeat == 2)
			{
				//we use the lambda expression
				//the capture list uses & to saywe want to use all local variables by reference
				std::future<void> result(std::async([&]()
					{
						send_sdl_music_event(SdlMusicOptions::SDL_SONG_SEEK, frac);
					}));
				continue;
			}
			//we post the message so that we can proceed with this thread without waiting
			else if (songs_to_play.size() >= song_status.song_number + 1 && songs_to_play.size() != 1)
				PostMessage(h_next_btn, BM_CLICK, 0, 0);
			else if (songs_to_play.size() == 1 && s_config_file.i_play_repeat == 1)
			{
				std::future<void> result(std::async([&]()
					{
						send_sdl_music_event(SdlMusicOptions::SDL_SONG_SEEK, frac);
					}));
				continue;
			}
			else
				 exit_playback();
			break;
		}
		

		//set the elapsed time text label
		int secs = secs_elapsed;
		int mins = secs / 60;
		secs %= 60;
		int hours = mins / 60;
		mins %= 60;
		if (hours > 0)
			wsprintf(total_time, L"%d:%d:%d", hours, mins, secs);
		else if (mins > 0)
			wsprintf(total_time, L"%d:%d", mins, secs);
		else
			wsprintf(total_time, L"%d", secs);
		SetWindowText(h_play_time_txt[0], total_time);

		//set the progress bar
		d_play_running_time = secs_elapsed;
		//RECT rect{};
		//GetWindowRect(h_play_progress_bar[0], &rect);
		RECT rect{};
		
		GetClientRect(GetParent(h_play_progress_bar[0]), &rect);
		int i_x = 10;
		int i_y =i_progress_bar_y_pos;
		int i_progress_left_w = (rect.right - 20) * d_play_running_time / d_play_total_time - 4;
		int i_progress_right_w = (rect.right - 20) * (d_play_total_time - d_play_running_time) / d_play_total_time;
		int i_progress_middle_w = 8;
		int i_progress_h = 4;
		MoveWindow(h_play_progress_bar[0], i_x, i_y, i_progress_left_w, i_progress_h, true);
		i_x += i_progress_left_w;
		MoveWindow(h_play_progress_bar[1], i_x + 8, i_y, i_progress_right_w, i_progress_h, true);
		i_y -= 2;
		MoveWindow(h_play_progress_bar[2], i_x, i_y, i_progress_middle_w, i_progress_middle_w, true);
		Sleep(500);
	}
	
}
void Menu::displayLastErrorDebug(LPTSTR lpSzFunction)
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)& lpMsgBuf, 0, NULL);

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpSzFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("%s failed with error %d: %s"), lpSzFunction, dw, lpMsgBuf);
	OutputDebugStringW((LPCWSTR)lpDisplayBuf);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}
void Menu::close_song_gui()
{
	//set the start and end time to defaults
	SetWindowText(h_play_time_txt[1], L"00:00:00");
	//ft_set_song_time_elapsed.get();
	SetWindowText(h_play_time_txt[0], L"00:00:00");
	for (int c = 0; c <= 2; c++)
		ShowWindow(h_play_progress_bar[c], false);
	for (int c = 0; c <= 1; c++)
		ShowWindow(h_play_time_txt[c], false);
	InvalidateRect(h_play_btn, NULL, true);
	UpdateWindow(h_play_btn);

}
void Menu::progress_bar_clicked(HWND h_clicked)
{
	if (h_clicked == h_play_progress_bar[2])
		return;
	POINT point;
	RECT rect_left, rect_middle, rect_right, rect_parent;
	int total_width = 0;
	double* frac = (double*)malloc(sizeof(double));
	

	GetCursorPos(&point);
	ScreenToClient(GetParent(h_clicked), &point);
	
	GetClientRect(h_play_progress_bar[0], &rect_left);
	GetClientRect(h_play_progress_bar[1], &rect_middle);
	GetClientRect(h_play_progress_bar[2], &rect_right);
	GetClientRect(GetParent(h_play_progress_bar[0]), &rect_parent);

	//total_width = rect_left.right + rect_right.right;

	int i_x = 10;
	int i_y = i_progress_bar_y_pos;
	double i_progress_left_w = point.x - 10;
	double i_progress_right_w = rect_parent.right - 20 - i_progress_left_w - 8;
	int i_progress_middle_w = 8;
	int i_progress_h = 4;
	MoveWindow(h_play_progress_bar[0], i_x, i_y, i_progress_left_w, i_progress_h, true);
	i_x += i_progress_left_w;
	MoveWindow(h_play_progress_bar[1], i_x + 8, i_y, i_progress_right_w, i_progress_h, true);
	i_y -= 2;
	MoveWindow(h_play_progress_bar[2], i_x, i_y, i_progress_middle_w, i_progress_middle_w, true);

	*frac = i_progress_left_w / (i_progress_left_w + i_progress_right_w + 8);
	*frac = *frac >= 100 ? 100 : *frac;
	*frac = *frac >= 0 ? *frac : 0;

	send_sdl_music_event(SdlMusicOptions::SDL_SONG_SEEK, frac);
}
void Menu::write_to_prefs_file()
{
	HANDLE h_file_prefs{};
	h_file_prefs = CreateFile(w_config_file_path,                // name of the write
		GENERIC_WRITE,          // open for writing
		FILE_SHARE_WRITE ,                      // do not share
		NULL,                   // default security
		OPEN_EXISTING,             // create new file only
		FILE_ATTRIBUTE_HIDDEN,  // hidden file
		NULL);

	if (h_file_prefs == INVALID_HANDLE_VALUE || !s_config_file.changed)
		return;

	char data_buf[256]; 
	sprintf_s(data_buf, "%s\t%d\n%s\t%d\n%s\t%d", 
		"PlayBackShowMode", s_config_file.playback_show_mode, 
		"repeat", s_config_file.i_play_repeat,
		"shuffle", s_config_file.i_play_shuffle);
	
	DWORD dw_bytes_to_write = (DWORD)strlen(data_buf);
	DWORD dw_bytes_written = 0;
	bool b_error_flag = false;

	b_error_flag = WriteFile(
		h_file_prefs,           // open file handle
		data_buf,      // start of data to write
		dw_bytes_to_write,  // number of bytes to write
		&dw_bytes_written, // number of bytes that were written
		NULL);
	if (!b_error_flag)
		displayLastErrorDebug((LPTSTR)L"file write");
	CloseHandle(h_file_prefs);	
}
void Menu::read_from_prefs_file()
{
	const int buffer_size = 256;
	HANDLE h_file_prefs{};
	h_file_prefs = CreateFile(w_config_file_path,                // name of the write
		GENERIC_READ ,          // open for writing
		FILE_SHARE_READ,                      // do not share
		NULL,                   // default security
		OPEN_EXISTING,             // create new file only
		FILE_ATTRIBUTE_HIDDEN,  // hidden file
		NULL);

	if (h_file_prefs == INVALID_HANDLE_VALUE)
		return;
	char data_buf[buffer_size];
	DWORD dw_bytes_read = 0;
	bool b_error_flag = false;
	//the max bytes read need to be less 1 from buffer size to preserve room for the null character
	b_error_flag = ReadFile(h_file_prefs, data_buf, buffer_size - 1, &dw_bytes_read, NULL);
	if (dw_bytes_read > 0 && dw_bytes_read <= buffer_size - 1)
	{
		data_buf[dw_bytes_read] = '\0'; // NULL character
	}
	if (b_error_flag)
	{
		char* token,* next_token, * inner_token, * inner_next_token;
		int count = 0;
		token=strtok_s(data_buf, "\n", &next_token);
		while (token) 
		{			
			int  inner_count = 0;
			inner_token = strtok_s(token, "\t", &inner_next_token);
			while (inner_token)
			{
				if (inner_count == 1)
				{
					//convert the value straight to int
					char val = atoi(inner_token);
					if (count == 0)
						s_config_file.playback_show_mode = (ConfigFile::PlayBackShowMode) val;
					else if (count == 1)
						s_config_file.i_play_repeat = val;
					else if (count == 2)
						s_config_file.i_play_shuffle = val;
				}
				inner_token = strtok_s(NULL, "\t", &inner_next_token);
				inner_count += 1;
			}
			token = strtok_s(NULL, "\n", &next_token);
			count += 1;
		}
	}
	CloseHandle(h_file_prefs);
}
void Menu::update_header_menu_items()
{
	HMENU main_menu = GetMenu(h_parent); 
	switch ( s_config_file.playback_show_mode)
	{
		//graphics menu
	case ConfigFile::PlayBackShowMode::SHOW_MODE_WAVES:
		CheckMenuItem(main_menu, ID_GRAPHICS_SOUNDWAVE, MF_CHECKED);
		i_checked_graphics_menu = ID_GRAPHICS_SOUNDWAVE;
		break;
	case ConfigFile::PlayBackShowMode::SHOW_MODE_VIDEO:
		CheckMenuItem(main_menu, ID_GRAPHICS_MEDIAART, MF_CHECKED);
		i_checked_graphics_menu = ID_GRAPHICS_MEDIAART;
		break;
	case ConfigFile::PlayBackShowMode::SHOW_MODE_RDFT:
		CheckMenuItem(main_menu, ID_GRAPHICS_SPECTRUM, MF_CHECKED);
		i_checked_graphics_menu = ID_GRAPHICS_SPECTRUM;
		break;
	case ConfigFile::PlayBackShowMode::SHOW_MODE_NONE:
		CheckMenuItem(main_menu, ID_GRAPHICS_BLANK, MF_CHECKED);
		i_checked_graphics_menu = ID_GRAPHICS_BLANK;
		break;
	default:
		CheckMenuItem(main_menu, ID_GRAPHICS_MEDIAART, MF_CHECKED);
		i_checked_graphics_menu = ID_GRAPHICS_MEDIAART;
		break;
	}
}
void Menu::menu_header_clicked(int id, HMENU parent_menu)
{
	
	bool graphics = false;
	switch (id)
	{
		//graphics menu
	case ID_GRAPHICS_BLANK:
		s_config_file.playback_show_mode = ConfigFile::PlayBackShowMode::SHOW_MODE_NONE;
		graphics = true;
		break;
	case ID_GRAPHICS_ILLUSTRATIONS:
		//s_config_file.playback_show_mode = ConfigFile::PlayBackShowMode::SHOW_MODE_NONE;
		//graphics = true;
		break;
	case ID_GRAPHICS_LOGO:
		s_config_file.playback_show_mode = ConfigFile::PlayBackShowMode::SHOW_MODE_NONE;
		graphics = true;
		break;
	case ID_GRAPHICS_MEDIAART:
		s_config_file.playback_show_mode = ConfigFile::PlayBackShowMode::SHOW_MODE_VIDEO;
		graphics = true;
		break;
	case ID_GRAPHICS_SOUNDWAVE:
		s_config_file.playback_show_mode = ConfigFile::PlayBackShowMode::SHOW_MODE_WAVES;
		graphics = true;
		break;
	case ID_GRAPHICS_TASTEBUBBLE:
		//s_config_file.playback_show_mode = ConfigFile::PlayBackShowMode::SHOW_MODE_NONE;
		//graphics = true;
		break;
	case ID_GRAPHICS_SPECTRUM:
		s_config_file.playback_show_mode = ConfigFile::PlayBackShowMode::SHOW_MODE_RDFT;
		graphics = true;
		break;
	}
	if (graphics)
	{
		CheckMenuItem(parent_menu, i_checked_graphics_menu, MF_UNCHECKED);
		CheckMenuItem(parent_menu, id, MF_CHECKED);
		i_checked_graphics_menu = id;
		s_config_file.changed = true;
	}
}
void Menu::shuffle_songs()
{
	if (s_config_file.i_play_shuffle == 1 && songs_to_play.size() > 1)
	{
		songs_to_play_temp = songs_to_play;
		std::random_shuffle(songs_to_play.begin() + 1, songs_to_play.end());
	}
	else if (s_config_file.i_play_shuffle == 0 && songs_to_play.size() > 1 && !songs_to_play_temp.empty())
	{
		songs_to_play = songs_to_play_temp;
	}
	s_config_file.i_play_shuffle = s_config_file.i_play_shuffle;
	s_config_file.changed = true;
}
Menu::~Menu()
{
	GdiplusShutdown(gdiplusToken);
	write_to_prefs_file();
}
