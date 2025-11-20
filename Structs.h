#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct Songs {
    int id;
    char title[100];
    char artist[100];
    float duration; 
} Songs;

typedef struct SongNodes {
    Songs *Song;
    struct SongNodes* nextSong;
} SongNodes;

typedef struct Albums {
    int id;
    char title[100];
    SongNodes* SongHead;
} Albums;

typedef struct AlbumNodes {
    Albums *Album;
    struct AlbumNodes* nextAlbum;
} AlbumNodes;

#endif
