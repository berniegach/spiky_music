#include "file_explorer.h"
#include "menu.h"



Menu::Menu()
{
	//default constructor
}
Menu::Menu(HWND* parent, HINSTANCE* hinstance,int parent_width,int parent_height)
	:h_parent(parent),	hinst(hinstance),i_parent_width(parent_width),i_parent_height(parent_height)
{
	//initialize GDI+
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	createMainButtons();
}
void Menu::Init(HWND* parent, HINSTANCE* hinstance, int parent_width, int parent_height)
{
	//initialize GDI+
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	h_parent = parent;
	hinst = hinstance;
	i_parent_width = parent_width;
	i_parent_height = parent_height;
	//create the buttons
	createMainButtons();
}
/*
this function draws the main menu buttons at the bottom of the screen. We draw the buttons starting with the bottom left heading rightwards
*/
void Menu::createMainButtons()
{
	int i_menu_bar_height = GetSystemMetrics(SM_CYMENU);
	int i_title_bar_height= GetSystemMetrics(SM_CXSCREEN);
	RECT rect{};
	GetClientRect(*h_parent, &rect);	

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
		*hwnds[c] = CreateWindow(WC_BUTTON, TEXT(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, i_x, i_y, i_btn_width, i_btn_height, *h_parent, (HMENU)ids[c], *hinst, NULL);
		i_x += i_btn_width + i_distance_between;
	}
	//create the center buttons
	long l_x_center = rect.right / 2;
	i_x = l_x_center - (i_btn_width + i_distance_between + (i_btn_width / 2));
	//lets take advantage that we have the center to create the stop button which is left of the center buttons
	//the stop button starts as disbaled since we dont have a song playing
	long i_x_stop = i_x - (i_btn_width + i_distance_between );
	h_stop_btn = CreateWindow(WC_BUTTON, TEXT(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, i_x_stop, i_y, i_btn_width, i_btn_height, *h_parent, (HMENU)i_stop_btn_id, *hinst, NULL);
	EnableWindow(h_stop_btn, false);
	//create the three center buttons
	//previous, play, and next
	for (int c = 8; c <= 10; c++)
	{
		*hwnds[c] = CreateWindow(WC_BUTTON, TEXT(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, i_x, i_y, i_btn_width, i_btn_height, *h_parent, (HMENU)ids[c], *hinst, NULL);
		i_x += i_btn_width + i_distance_between;
	}
	//create the rightmost buttons
	i_x = rect.right - i_btn_width - 5;
	for (int c = 13; c >= 11; c--)
	{
		*hwnds[c] = CreateWindow(WC_BUTTON, TEXT(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, i_x, i_y, i_btn_width, i_btn_height, *h_parent, (HMENU)ids[c], *hinst, NULL);
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
	h_play_progress_bar[0]= CreateWindow(WC_BUTTON, TEXT(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, i_x, i_y, i_progress_left_w, i_progress_h, *h_parent, (HMENU)i_play_progress_bar_id[0], *hinst, NULL);
	i_x += i_progress_left_w;
	h_play_progress_bar[1] = CreateWindow(WC_BUTTON, TEXT(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, i_x+8, i_y, i_progress_right_w, i_progress_h, *h_parent, (HMENU)i_play_progress_bar_id[1], *hinst, NULL);
	i_y -= 2;
	h_play_progress_bar[2] = CreateWindow(WC_BUTTON, TEXT(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, i_x, i_y, i_progress_middle_w, i_progress_middle_w, *h_parent, (HMENU)i_play_progress_bar_id[2], *hinst, NULL);
	//create the time labels
	i_x = 10;
	i_y -= 10;
	int i_time_w = 50;
	int i_time_h = 12;
	h_play_time_txt[0]= CreateWindow(WC_STATIC, TEXT("00:00:00"), WS_CHILD | WS_VISIBLE | SS_LEFT , i_x, i_y, i_time_w, i_time_h, *h_parent, (HMENU)i_plat_time_txt_id[0], *hinst, NULL);
	i_x = rect.right - 10 - 50;
	h_play_time_txt[1] = CreateWindow(WC_STATIC, TEXT("00:00:00"), WS_CHILD | WS_VISIBLE | SS_RIGHT, i_x, i_y, i_time_w, i_time_h, *h_parent, (HMENU)i_plat_time_txt_id[1], *hinst, NULL);
	//create the window where sdl window will be attached to
	//make it invisible
	h_sdl_window = CreateWindow(WC_STATIC, TEXT(""), WS_CHILD | WS_VISIBLE, 0, 0, rect.right, i_y, *h_parent, (HMENU)i_sdl_window_id, *hinst, NULL);
	ShowWindow(h_sdl_window, false);
	//ffplay.init(h_sdl_window);
	//check if the user has favorites added. if not show this button
	int i_big_fav_add_w = 100;
	i_x = rect.right / 2 - i_big_fav_add_w / 2;
	i_y = rect.bottom / 2 - i_big_fav_add_w / 2;
	bool has_favorites = false;
	if(!has_favorites)
		h_favorites_add_large_btn= CreateWindow(WC_BUTTON, TEXT(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, i_x, i_y, i_big_fav_add_w, i_big_fav_add_w, *h_parent, (HMENU)i_favorites_add_large_btn_id, *hinst, NULL);
	
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
			hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_COMMENT_NORMAL));
			break;
		case 2:
			hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_UPLOAD_NORMAL));
			break;
		case 3:
			hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_PLAYLIST_NORMAL));
			break;
		case 4:
			hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_FAVORITE_LIST_NORMAL));
			break;
		case 5:
			hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_FAVORITE_ADD_NORMAL));
			break;
		case 6:
			hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_FULLSCREEN_NORMAL));
			break;
		case 7:
			if (i_repeat_btn_status == 0)
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_REPEAT_ALL_NORMAL));
			else if (i_repeat_btn_status == 1)
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_REPEAT_ALL_PRESSED));
			else 
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_REPEAT_ONE_PRESSED));
			break;
		case 8:
			if(i_shuffle_btn_status==0)
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_SHUFFLE_NORMAL));
			else
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_SHUFFLE_PRESSED));
			break;
		case 9:
			hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_PREVIOUS_NORMAL));
			break;
		case 10:
			song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_EMPTY || song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_PLAYING ?
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_PLAY_NORMAL)) :
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_PAUSE_W_NORMAL));
			break;
		case 11:
			hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_NEXT_NORMAL));
			break;
		case 12:
			hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_SORT_BY_SIMILARITY_NORMAL));
			break;
		case 13:
			hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_SORT_BY_RATINGS_NORMAL));
			break;
		case 14:
			hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_SCAN_NORMAL));
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
			hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_PROGRESS_BAR_CIRCLE_NORMAL));			
			break;
		case 20:
			hBitmap= LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_FAVORITE_ADD_NORMAL_LARGE));
			break;
		case 22:
			song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_EMPTY ?
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_STOP_DISABLED)) :
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_STOP_NORMAL));
			break;
		default:
			hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_COMMENT_NORMAL));

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
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_COMMENT_PRESSED));
				break;
			case 2:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_UPLOAD_PRESSED));
				break;	
			case 3:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_PLAYLIST_PRESSED));
				break;
			case 4:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_FAVORITE_LIST_PRESSED));
				break;
			case 5:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_FAVORITE_ADD_PRESSED));
				break;
			case 6:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_FULLSCREEN_PRESSED));
				break;
			case 7:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_REPEAT_ALL_PRESSED));
				break;
			case 8:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_SHUFFLE_PRESSED));
				break;
			case 9:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_PREVIOUS_PRESSED));
				break;
			case 10:
				song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_EMPTY || song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_PLAYING ?
					hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_PLAY_PRESSED)) :
					hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_PAUSE_W_PRESSED));
				break;
			case 11:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_NEXT_PRESSED));
				break;
			case 12:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_SORT_BY_SIMILARITY_PRESSED));
				break;
			case 13:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_SORT_BY_RATINGS_PRESSED));
				break;
			case 14:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_SCAN_PRESSED));
				break;
			case 20:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_FAVORITE_ADD_PRESSED_LARGE));
				break;
			case 22:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_STOP_PRESSED));
				break;
			default:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_COMMENT_PRESSED));

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
	if (IsWindowVisible(h_sdl_window))
		MoveWindow(h_sdl_window, 0, 0, rect.right, i_y, true);
	//move the big favorite add button to the middle
	if (IsWindowVisible(h_favorites_add_large_btn))
	{
		int i_big_fav_add_w = 100;
		i_x = rect.right / 2 - i_big_fav_add_w / 2;
		i_y = rect.bottom / 2 - i_big_fav_add_w / 2;
		MoveWindow(h_favorites_add_large_btn, i_x, i_y, i_big_fav_add_w, i_big_fav_add_w, true);
	}
	
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
		i_repeat_btn_status = (i_repeat_btn_status + 1) % 3;		
		InvalidateRect(h_clicked, NULL, true);
		UpdateWindow(h_clicked);
	}
	else if (id == i_shuffle_btn_id)
	{
		//there are two states in the shuffle button
		//shuffle and dont shuffle
		i_shuffle_btn_status = (i_shuffle_btn_status + 1) % 2;
		InvalidateRect(h_clicked, NULL, true);
		UpdateWindow(h_clicked);
	}
	else if (id == i_favorites_add_large_btn_id)
	{
		//the large button for adding favorites has been clicked
		//open file explorer and get the song paths
		//we pass one to tell the class that we are adding the initial favorites
		//FileExplorer file_explorer;
		//file_explorer.openDialogWindow(INITIAL_LOAD_FAVORITES, h_sdl_window);
	}
	else if (id == i_play_btn_id)
	{
		//if there are no songs in the que find the song to play
		if ( song_status.song_playing==SongStatus::SongPlaying::SONG_PLAY_EMPTY)
		{
			FileExplorer file_explorer;
			//make sure file_explorer is not cancelled
			if ((songs_to_play = file_explorer.find_songs_to_play(GetParent(h_clicked))).empty())
				return;

			if (songs_to_play.size() == 1)
			{
				ft_play_song=std::async(launch::async, &Menu::play_song, this, songs_to_play.at(0));
				update_stop_button(true);
				ft_set_song_duration = std::async(launch::async, &Menu::set_song_duration, this);
			}
			else
			{
				wstring directory = songs_to_play.at(0);
				wstring song_path=directory + L"\\" + songs_to_play.at(++song_status.song_number);
				ft_play_song = std::async(launch::async, &Menu::play_song, this, song_path);
				update_stop_button(true);
				//EnableWindow(h_stop_btn, true);

			}
		}
		else
		{
			//first we check if we are playing the first song
			//or we are proceeding to play a song
			if (song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_PLAYING)
			{
				if (send_sdl_music_event(SdlMusicOptions::SDL_SONG_PAUSE))
					song_status.song_playing = SongStatus::SongPlaying::SONG_PLAY_PAUSED;
			}		
			else if (song_status.song_playing == SongStatus::SongPlaying::SONG_PLAY_PAUSED)
			{
				if(send_sdl_music_event(SdlMusicOptions::SDL_SONG_PLAY))
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
		{
			//first lets quit the current song
			send_sdl_music_event(SdlMusicOptions::SDL_SONG_QUIT);
			ft_play_song.get();	
			//now lets play the next song
			wstring directory = songs_to_play.at(0);
			wstring song_path = directory + L"\\" + songs_to_play.at(++song_status.song_number);
			ft_play_song = std::async(launch::async, &Menu::play_song, this, song_path);
		}
	}
	else if (id == i_previous_btn_id)
	{
		if (song_status.song_number - 1 > 0)
		{
			//first lets quit the current song
			send_sdl_music_event(SdlMusicOptions::SDL_SONG_QUIT);
			ft_play_song.get();
			//now lets play the next song
			wstring directory = songs_to_play.at(0);
			wstring song_path = directory + L"\\" + songs_to_play.at(--song_status.song_number);
			ft_play_song = std::async(launch::async, &Menu::play_song, this, song_path);
		}
	}
	else if (id == i_stop_btn_id)
	{
		send_sdl_music_event(SdlMusicOptions::SDL_SONG_QUIT);
		ft_play_song.get();
		update_stop_button(false);
		songs_to_play.clear();
		song_status.song_number = 0;
	}
}

void Menu::paint(HDC* hdc, HWND* hwnd)
{
	RECT rect;
	GetClientRect(*hwnd, &rect);
	Graphics graphics(*hdc);
	//draw the bottom bounding box
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
	SolidBrush solid_brush(Color(230, 230, 230));
	graphics.FillRectangle(&solid_brush, i_x, i_y, i_w, i_h);
}
int Menu::send_sdl_music_event(SdlMusicOptions options)
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
	}
	return i_return;
}
void Menu::play_song(wstring song_path)
{
	using std::future;
	using std::async;
	using std::launch;

	Ffplay ffplay;
	string input{ song_path.begin(),song_path.end() };
	//lets plays the song
	song_status.song_playing = SongStatus::SongPlaying::SONG_PLAY_PLAYING;	
	ffplay.play_song(input, h_sdl_window);
	//the song is not playing anymore
	song_status.song_playing = SongStatus::SongPlaying::SONG_PLAY_EMPTY;
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
		send_sdl_music_event(SdlMusicOptions::SDL_SONG_QUIT);
	}
	GdiplusShutdown(gdiplusToken);
}
void Menu::set_song_duration()
{
	int hours, mins, secs, us;
	int64_t duration;
	wchar_t total_time[64];
	wchar_t start_time[64];
	for (;;)
	{
		if (Ffplay::is_song_duration_set())
		{
			int64_t duration = Ffplay::get_song_duration();
			int64_t time = duration + (duration <= INT64_MAX - 5000 ? 5000 : 0);
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

Menu::~Menu()
{
}
