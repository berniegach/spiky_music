#include "favorites.h"

Favorites::Favorites(HWND parent)
{
    h_parent = parent;
	
}

void Favorites::initialAddFavorites(std::vector<std::wstring>& file_names)
{
	//first we load the songs data
	for (int c = 0; c < file_names.size(); c++)
	{
		std::string input{ file_names.at(c).begin(),file_names.at(c).end() };
		if (file_names.size() == 1)
		{
           // Ffplay ffplay(input, h_parent);
		}

	}
}

Favorites::~Favorites()
{
}
