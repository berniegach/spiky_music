#include "playlist_view.h"
#include <stdio.h>

/******************************************************************************
   CreateListView
******************************************************************************/
#define ID_LISTVIEW  2000
HWND PlaylistView::CreateListView(HINSTANCE hInstance, HWND hwndParent)
{
    DWORD       dwStyle;
    HWND        hwndListView;
    HIMAGELIST  himlSmall;
    HIMAGELIST  himlLarge;
    BOOL        bSuccess = TRUE;
   
    dwStyle = WS_TABSTOP |
        WS_CHILD |
       // WS_VISIBLE |
        //LVS_OWNERDRAWFIXED |
        WS_BORDER|
        LVS_AUTOARRANGE |
        LVS_REPORT |
        LVS_OWNERDATA;

    hwndListView = CreateWindowEx(0,          // ex style
        WC_LISTVIEW,               // class name - defined in commctrl.h
        TEXT(""),                        // dummy text
        dwStyle,                   // style
        0,                         // x position
        0,                         // y position
        0,                         // width
        0,                         // height
        hwndParent,                // parent
        (HMENU)ID_LISTVIEW,        // ID
        hInstance,                   // instance
        NULL);                     // no extra data

    if (!hwndListView)
        return NULL;

    ListView_SetExtendedListViewStyle(hwndListView, LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES);

    ResizeListView(hwndListView, 0,0,10,10);

    //set the image lists
    himlSmall = ImageList_Create(16, 16, ILC_COLORDDB | ILC_MASK, 1, 0);
    himlLarge = ImageList_Create(32, 32, ILC_COLORDDB | ILC_MASK, 1, 0);

    if (himlSmall && himlLarge)
    {
        HICON hIcon;

        //set up the small image list
        hIcon = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_HEADPHONES_W), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
        ImageList_AddIcon(himlSmall, hIcon);

        //set up the large image list
        hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HEADPHONES_W));
        ImageList_AddIcon(himlLarge, hIcon);

        ListView_SetImageList(hwndListView, himlSmall, LVSIL_SMALL);
        ListView_SetImageList(hwndListView, himlLarge, LVSIL_NORMAL);
    }

    return hwndListView;
}

/******************************************************************************
   ResizeListView
******************************************************************************/

void PlaylistView::ResizeListView(HWND hwndListView, int x, int y, int w, int h)
{

    MoveWindow(hwndListView,
        x,
        y,
        w,
        h,
        TRUE);

    //only call this if we want the LVS_NOSCROLL style
    //PositionHeader(hwndListView);
}

/******************************************************************************
   PositionHeader
   this needs to be called when the ListView is created, resized, the view is
   changed or a WM_SYSPARAMETERCHANGE message is received
******************************************************************************/

void PlaylistView::PositionHeader(HWND hwndListView)
{
    HWND  hwndHeader = GetWindow(hwndListView, GW_CHILD);
    DWORD dwStyle = GetWindowLong(hwndListView, GWL_STYLE);

    /*To ensure that the first item will be visible, create the control without
    the LVS_NOSCROLL style and then add it here*/
    dwStyle |= LVS_NOSCROLL;
    SetWindowLong(hwndListView, GWL_STYLE, dwStyle);

    //only do this if we are in report view and were able to get the header hWnd
    if (((dwStyle & LVS_TYPEMASK) == LVS_REPORT) && hwndHeader)
    {
        RECT        rc;
        HD_LAYOUT   hdLayout;
        WINDOWPOS   wpos;

        GetClientRect(hwndListView, &rc);
        hdLayout.prc = &rc;
        hdLayout.pwpos = &wpos;

        Header_Layout(hwndHeader, &hdLayout);

        SetWindowPos(hwndHeader,
            wpos.hwndInsertAfter,
            wpos.x,
            wpos.y,
            wpos.cx,
            wpos.cy,
            wpos.flags | SWP_SHOWWINDOW);

        ListView_EnsureVisible(hwndListView, 0, FALSE);
    }
}

/******************************************************************************
   InitListView
******************************************************************************/

BOOL PlaylistView::InitListView(HWND hwndListView)
{
    LV_COLUMN   lvColumn;
    int         i;
    TCHAR       szString[5][20] = { TEXT("Title"), TEXT("Duration"), TEXT("Album") };
    HWND h_parent =   GetParent(hwndListView);
    RECT rect{};
    GetClientRect(h_parent, &rect);
    //empty the list
    ListView_DeleteAllItems(hwndListView);

    //initialize the columns
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM ;
    lvColumn.fmt = LVCFMT_LEFT;
    for (i = 0; i < 3; i++)
    {
        lvColumn.cx = i == 4 ? rect.right - (200 * 1) : 200;
        lvColumn.pszText = szString[i];
        ListView_InsertColumn(hwndListView, i, &lvColumn);
    }

    InsertListViewItems(hwndListView,0);

    return TRUE;
}

/******************************************************************************
   InsertListViewItems
******************************************************************************/
#define ITEM_COUNT   100000
BOOL PlaylistView::InsertListViewItems(HWND hwndListView, int size)
{
    //empty the list
    ListView_DeleteAllItems(hwndListView);

    //set the number of items in the list
    ListView_SetItemCount(hwndListView, size);

    return TRUE;
}

/**************************************************************************
   ListViewNotify()
**************************************************************************/

LRESULT PlaylistView::ListViewNotify(HWND hWnd, LPARAM lParam, vector<wstring> songs)
{
    LPNMHDR  lpnmh = (LPNMHDR)lParam;
    HWND     hwndListView = GetDlgItem(hWnd, ID_LISTVIEW);

    switch (lpnmh->code)
    {
    case LVN_GETDISPINFO:
    {
        LV_DISPINFO* lpdi = (LV_DISPINFO*)lParam;
        TCHAR szString[MAX_PATH];

        if (lpdi->item.iSubItem)
        {
            if (lpdi->item.mask & LVIF_TEXT)
            {
                _sntprintf_s(szString, _countof(szString), _TRUNCATE, TEXT("Item %d - Column %d"),
                    lpdi->item.iItem + 1, lpdi->item.iSubItem );
                _tcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,
                    szString, _TRUNCATE);
            }
        }
        else
        {
            if (lpdi->item.mask & LVIF_TEXT)
            {
                wstring song;
                wchar_t w_song[256];
                if (songs.size() == 1)
                {
                    song = songs.at(0);

                    wchar_t w_str[256];
                    wsprintf(w_str, L"%s", song.c_str());

                    wchar_t* buffer;
                    wchar_t* token = wcstok_s(w_str, L"\\", &buffer);
                    std::vector<wchar_t*>v_str;
                    while (token) {
                        token = wcstok_s(NULL, L"\\", &buffer);
                        v_str.push_back(token);
                    }
                    wsprintf(w_song, L"%s", v_str.at(v_str.size() - 2));
                }
                else if (songs.size() > 1)
                {
                    song = songs.at(lpdi->item.iItem + 1);
                    wsprintf(w_song, L"%s", song.c_str());
                }
                lstrcpyW(lpdi->item.pszText, w_song);
            }

            if (lpdi->item.mask & LVIF_IMAGE)
            {
                lpdi->item.iImage = 0;
            }
        }
    }
    return 0;

    case LVN_ODCACHEHINT:
    {
        LPNMLVCACHEHINT   lpCacheHint = (LPNMLVCACHEHINT)lParam;
        /*
        This sample doesn't use this notification, but this is sent when the
        ListView is about to ask for a range of items. On this notification,
        you should load the specified items into your local cache. It is still
        possible to get an LVN_GETDISPINFO for an item that has not been cached,
        therefore, your application must take into account the chance of this
        occurring.
        */
    }
    return 0;

    case LVN_ODFINDITEM:
    {
        LPNMLVFINDITEM lpFindItem = (LPNMLVFINDITEM)lParam;
        /*
        This sample doesn't use this notification, but this is sent when the
        ListView needs a particular item. Return -1 if the item is not found.
        */
    }
    return 0;
    case NM_CUSTOMDRAW:
    {
        LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)lParam;
        LV_DISPINFO* lpdi = (LV_DISPINFO*)lParam;

        switch (lplvcd->nmcd.dwDrawStage)
        {

        case CDDS_PREPAINT:
            return CDRF_NOTIFYITEMDRAW;

        case CDDS_ITEMPREPAINT:
            //SelectObject(lplvcd->nmcd.hdc,	GetFontForItem(lplvcd->nmcd.dwItemSpec,	lplvcd->nmcd.lItemlParam));
            //lplvcd->clrText = GetColorForItem(lplvcd->nmcd.dwItemSpec,		lplvcd->nmcd.lItemlParam);
            lplvcd->clrTextBk = lplvcd->nmcd.dwItemSpec % 2 ? RGB(255, 255, 255) : RGB(240, 240, 240);
            /* At this point, you can change the background colors for the item
            and any subitems and return CDRF_NEWFONT. If the list-view control
            is in report mode, you can simply return CDRF_NOTIFYSUBITEMDRAW
            to customize the item's subitems individually */

            return CDRF_NEWFONT;
            //  or return CDRF_NOTIFYSUBITEMDRAW;

        case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
            /*SelectObject(lplvcd->nmcd.hdc,
                GetFontForSubItem(lplvcd->nmcd.dwItemSpec,
                    lplvcd->nmcd.lItemlParam,
                    lplvcd->iSubItem));
            lplvcd->clrText = GetColorForSubItem(lplvcd->nmcd.dwItemSpec,
                lplvcd->nmcd.lItemlParam,
                lplvcd->iSubItem));
                lplvcd->clrTextBk = GetBkColorForSubItem(lplvcd->nmcd.dwItemSpec,
                    lplvcd->nmcd.lItemlParam,
                    lplvcd->iSubItem));*/

                    /* This notification is received only if you are in report mode and
                    returned CDRF_NOTIFYSUBITEMDRAW in the previous step. At
                    this point, you can change the background colors for the
                    subitem and return CDRF_NEWFONT.*/
            return CDRF_NEWFONT;
        }
    }


    }

    return 0;
}

/**************************************************************************
   ErrorHandlerEx()
**************************************************************************/

void PlaylistView::ErrorHandlerEx(WORD wLine, LPSTR lpszFile)
{
    LPVOID lpvMessage;
    DWORD  dwError;
    TCHAR  szBuffer[256];

    // Allow FormatMessage() to look up the error code returned by GetLastError
    dwError = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPTSTR)&lpvMessage,
        0,
        NULL);

    // Check to see if an error occurred calling FormatMessage()
    if (0 == dwError)
    {
        _sntprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE,
            TEXT("An error occurred calling FormatMessage().")
            TEXT("Error Code %d"),
            GetLastError());
        MessageBox(NULL,
            szBuffer,
            TEXT("Generic"),
            MB_ICONSTOP | MB_ICONEXCLAMATION);
        return;
    }

    // Display the error information along with the place the error happened.
    _sntprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE,
        TEXT("Generic, Line=%d, File=%s"), wLine, lpszFile);
    MessageBox(NULL, (LPCWSTR) lpvMessage, szBuffer, MB_ICONEXCLAMATION | MB_OK);
}

/**************************************************************************
   SwitchView()
**************************************************************************/

void PlaylistView::SwitchView(HWND hwndListView, DWORD dwView)
{
    DWORD dwStyle = GetWindowLong(hwndListView, GWL_STYLE);

    SetWindowLong(hwndListView, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | dwView);

    //ResizeListView(hwndListView, GetParent(hwndListView));
    ResizeListView(hwndListView, 0, 0, 10, 10);
}

/**************************************************************************
   DoContextMenu()
**************************************************************************/

/*BOOL DoContextMenu(HWND hWnd, HINSTANCE hinst,  WPARAM wParam,   LPARAM lParam)
{
    HWND  hwndListView = (HWND)wParam;
    HMENU hMenuLoad,
        hMenu;

    if (hwndListView != GetDlgItem(hWnd, ID_LISTVIEW))
        return FALSE;

    hMenuLoad = LoadMenu(hinst, MAKEINTRESOURCE(IDM));
    hMenu = GetSubMenu(hMenuLoad, 0);

    UpdateMenu(hwndListView, hMenu);

    TrackPopupMenu(hMenu,
        TPM_LEFTALIGN | TPM_RIGHTBUTTON,
        LOWORD(lParam),
        HIWORD(lParam),
        0,
        hWnd,
        NULL);

    DestroyMenu(hMenuLoad);

    return TRUE;
}*/

/**************************************************************************
   UpdateMenu()
**************************************************************************/

/*void UpdateMenu(HWND hwndListView, HMENU hMenu)
{
    UINT  uID = IDM_LIST;
    DWORD dwStyle;

    //uncheck all of these guys
    CheckMenuItem(hMenu, IDM_LARGE_ICONS, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_SMALL_ICONS, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_LIST, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_REPORT, MF_BYCOMMAND | MF_UNCHECKED);

    //check the appropriate view menu item
    dwStyle = GetWindowLong(hwndListView, GWL_STYLE);
    switch (dwStyle & LVS_TYPEMASK)
    {
    case LVS_ICON:
        uID = IDM_LARGE_ICONS;
        break;

    case LVS_SMALLICON:
        uID = IDM_SMALL_ICONS;
        break;

    case LVS_LIST:
        uID = IDM_LIST;
        break;

    case LVS_REPORT:
        uID = IDM_REPORT;
        break;
    }
    CheckMenuRadioItem(hMenu, IDM_LARGE_ICONS, IDM_REPORT, uID, MF_BYCOMMAND | MF_CHECKED);

}*/
