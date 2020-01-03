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
		if (*hwnds[c] == NULL)
			displayLastErrorDebug((LPTSTR)L"NULL");
	}
	//create the center buttons
	long l_x_center = rect.right / 2;
	i_x = l_x_center - (i_btn_width + i_distance_between + (i_btn_width / 2));
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
			hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_PLAY_NORMAL));
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
		default:
			hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_COMMENT_NORMAL));

	}
	if (pdis->CtlID >= 15 && pdis->CtlID <= 16)
		return;
	GetObject(hBitmap, sizeof(BITMAP), &bitmap);
	hdcMem = CreateCompatibleDC(pdis->hDC);
	SelectObject(hdcMem, hBitmap);
	if(pdis->CtlID==17)
		StretchBlt(pdis->hDC, pdis->rcItem.left, pdis->rcItem.top, pdis->rcItem.right - pdis->rcItem.left, pdis->rcItem.bottom - pdis->rcItem.top, hdcMem, 0, 0, 8, 8, SRCCOPY);
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
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_PLAY_PRESSED));
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
			default:
				hBitmap = LoadBitmap(*hinst, MAKEINTRESOURCE(IDB_COMMENT_PRESSED));

		}
		GetObject(hBitmap, sizeof(BITMAP), &bitmap);
		hdcMem = CreateCompatibleDC(pdis->hDC);
		SelectObject(hdcMem, hBitmap);
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
	int i_btn_width = 30, i_btn_height = 30;
	int i_distance_between = 10;
	int i_total_left_btns_width = (8 * i_distance_between) + (8 * i_btn_width);
	int i_total_center_btns_width = (3 * i_distance_between) + (3 * i_btn_width);
	int i_total_right_btns_width = (3 * i_distance_between) + (3 * i_btn_width);
	int i_x = 0, i_y = rect.bottom - 35;
	//check if we can fit all of them
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
		}
		else
		{
			long l_x_center = rect.right / 2;
			i_x = l_x_center - (i_btn_width + i_distance_between + (i_btn_width / 2));
			for (int c = 8; c <= 10; c++)
			{
				MoveWindow(*hwnds[c], i_x, i_y, i_btn_width, i_btn_height, true);
				i_x += i_btn_width + i_distance_between;
			}
		}
		
	}
	else
		for (int c = 8; c <= 10; c++)
			ShowWindow(*hwnds[c], false);
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
}
void Menu::mainButtonClicked(int id,HWND h_clicked)
{
	i_which_main_btn_pressed = id;

	if (id == i_repeat_btn_id)
	{
		i_repeat_btn_status = (i_repeat_btn_status + 1) % 3;		
		InvalidateRect(h_clicked, NULL, true);
		UpdateWindow(h_clicked);
	}
	else if (id == i_shuffle_btn_id)
	{
		i_shuffle_btn_status = (i_shuffle_btn_status + 1) % 2;
		InvalidateRect(h_clicked, NULL, true);
		UpdateWindow(h_clicked);
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
	GdiplusShutdown(gdiplusToken);
}
