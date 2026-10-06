#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Structs.h"

extern SongNodes *SongLibraryHead;
extern SongNodes *PlaylistHead;
extern SongNodes *CurrentSong;
extern AlbumNodes *AlbumHead;

int ValidId(void);
SongNodes* FindSongId(SongNodes *head, int id);
AlbumNodes* FindAlbumById(AlbumNodes *head, int id);
Albums* CreateAlbum(void);
void SaveAllAlbums(void);

void LoadSongsFromFile(void);
void LoadAlbumsFromFile(void);
void ListAllSongs(void);
void ListAllAlbums(void);
void ViewAlbum(void);
void AddSongToAlbum(void);
void DeleteSongFromAlbum(void);
void AddAlbumToPlaylist(void);
void PlayNext(void);
void PlayPrevious(void);
void RemoveSongFromPlaylist(void);
void AddSongToPlaylist(void);
void LogCommand(const char *command);
void DisplayLog(void);
void FreeAllMemory(void);

#endif
