#include "file_explorer.h"

FileExplorer::FileExplorer()
{
	
}

void FileExplorer::openDialogWindow(int for_what)
{
    OPENFILENAME ofn;       // common dialog box structure
    wchar_t szFile[1024];       // buffer for file name
    HWND hwnd{};              // owner window
    HANDLE hf;              // file handle

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"Music\0*.mp3\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    // Display the Open dialog box. 

    if (GetOpenFileName(&ofn) == TRUE)
    {
        //for more than one file, the filenames in the lpstrfile are separated by NULL thereby we have to find a way to get the individual names
        //if only one file is selected the v_file_paths contains the path of the file
        //if more than one file is selected element[0] in v_file_paths is the directory and the rest are the file names
        std::vector<std::wstring>v_file_paths;
        wchar_t* str = ofn.lpstrFile;
        std::wstring directory = str;
        v_file_paths.push_back(directory);
        str += (directory.length() + 1);
        while (*str)         {
            std::wstring filename = str;
            str += (filename.length() + 1);
            v_file_paths.push_back(filename);            
        }
        if (for_what == 1)
        {
            Favorites favorites;
            favorites.initialAddFavorites(v_file_paths);
        }
    }
        /*hf = CreateFile(ofn.lpstrFile,
            GENERIC_READ,
            0,
            (LPSECURITY_ATTRIBUTES)NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            (HANDLE)NULL);*/
    
}



FileExplorer::~FileExplorer()
{
	
}
