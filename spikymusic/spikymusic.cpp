﻿// spikymusic.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "spikymusic.h"
#include "menu.h"
#include <math.h>
#include "logger.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
Menu menu;
int i_min_menu_id = 1;
int i_max_menu_id = 23;
int i_current_window = 1;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,   _In_opt_ HINSTANCE hPrevInstance,   _In_ LPWSTR    lpCmdLine,                   _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	//initialize common controls
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccx.dwICC = ICC_ANIMATE_CLASS | ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&iccx);

    // initialize the logger
	logger.init();

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SPIKYMUSIC, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
		return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SPIKYMUSIC));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
	//display memory leaks
	//_CrtDumpMemoryLeaks();
    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SPIKYMUSIC);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MAIN));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   //i include WS_CLIPCHILDREN to prevent the windows from flickering when updating
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int i_main_screen_width, i_main_screen_height;
	static int i_playlist_view_item = 0;
	HMENU main_menu;
	switch (message)
	{
		case WM_CREATE:
		{
			i_main_screen_width = GetSystemMetrics(SM_CXSCREEN);
			i_main_screen_height = GetSystemMetrics(SM_CYSCREEN);
			//create the menu buttons
			menu.Init(hWnd, hInst,i_main_screen_width,i_main_screen_height);

		}
		break;
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			HWND h_clicked = HWND(lParam);
			if (wmId >= i_min_menu_id && wmId <= i_max_menu_id)
				menu.mainButtonClicked(wmId, h_clicked);
			else
			{
				main_menu = GetMenu(hWnd);
				// Parse the menu selections:
				switch (wmId)
				{
					case IDM_ABOUT:
						DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
						break;
					case IDM_EXIT:
						DestroyWindow(hWnd);
						break;
					case ID_GRAPHICS_BLANK:
					case ID_GRAPHICS_ILLUSTRATIONS:
					case ID_GRAPHICS_LOGO:
					case ID_GRAPHICS_MEDIAART:
					case ID_GRAPHICS_SOUNDWAVE:
					case ID_GRAPHICS_TASTEBUBBLE:
					case ID_GRAPHICS_SPECTRUM:
						menu.menu_header_clicked(wmId, main_menu);
						break;

					default:
						return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
			
			
		}
		break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			//draw the menu content
			if (i_current_window == 1)
				menu.paint(&hdc,&hWnd);
			//Rectangle(hdc, 3, 3, 40, 40);
			
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_SIZE:
		{
			i_main_screen_width = LOWORD(lParam);
			i_main_screen_height = HIWORD(lParam);
			menu.windowSizeChanged(&hWnd);
		}
		break;
		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
			int i_id = pdis->CtlID;
			//the buttons belong to the menu class
			if (i_id >= i_min_menu_id && i_id <= i_max_menu_id)
				menu.drawButtons(pdis);
		}
		break;
		case WM_CTLCOLORSTATIC:
		{
			HDC hdc = (HDC)wParam;
			HWND hwnd = (HWND)lParam;
			SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
			return (INT_PTR)GetSysColorBrush(COLOR_WINDOW);
		}
		break;
		case WM_NOTIFY:
		{
			if ((((LPNMHDR)lParam)->hwndFrom) == menu.get_h_playlist_view())
			{
				switch (((LPNMHDR)lParam)->code)
				{
				case LVN_GETDISPINFO:
				case LVN_ODCACHEHINT:
				case LVN_ODFINDITEM:
				case NM_CUSTOMDRAW:
					return menu.playlistview_notify(hWnd, lParam);
				
				}
			}
			
		}
		break;
		case WM_DESTROY:
			menu.exit();
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
