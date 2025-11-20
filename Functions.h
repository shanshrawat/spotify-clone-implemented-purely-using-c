#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Structs.h"

int ValidId();
SongNodes* FindSongId(SongNodes *head, int id);
AlbumNodes* FindAlbumById(AlbumNodes *head, int id);
Albums* CreateAlbum();
void SaveAllAlbums();

extern AlbumNodes *AlbumHead;  

#endif
