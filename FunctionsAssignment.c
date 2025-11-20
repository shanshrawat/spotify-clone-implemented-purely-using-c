#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Functions.h"
#include "Structs.h"

AlbumNodes *AlbumHead = NULL;


void LoadSongsFromFile() {
    FILE *fp = fopen("SongsCollection.txt", "r");
    if(!fp) {
        printf("Error opening SongsCollection.txt!\n");
        return;
    }

    char line[300];

    while(fgets(line, sizeof(line), fp)) {

        Songs *newSong = malloc(sizeof(Songs));
        if (!newSong) {
            printf("Memory allocation failed for song\n");
            continue;
        }

        int min, sec;
        char rest[250];

        if(sscanf(line, "%d %d:%d %[^\n]", &newSong->id, &min, &sec, rest) == 4) {

            newSong->duration = min + (sec / 60.0f);

            char *pipePos = strchr(rest, '|');
            if(pipePos) {
                int titleLen = pipePos - rest;
                if (titleLen >= (int)sizeof(newSong->title)) titleLen = sizeof(newSong->title) - 1;
                strncpy(newSong->title, rest, titleLen);
                newSong->title[titleLen] = '\0';

                int i = titleLen - 1;
                while(i >= 0 && newSong->title[i] == ' ') {
                    newSong->title[i] = '\0';
                    i--;
                }

                strncpy(newSong->artist, pipePos + 1, sizeof(newSong->artist) - 1);
                newSong->artist[sizeof(newSong->artist)-1] = '\0';

                i = 0;
                while(newSong->artist[i] == ' ') i++;
                if(i > 0) {
                    memmove(newSong->artist, newSong->artist + i, strlen(newSong->artist) - i + 1);
                }

                i = strlen(newSong->artist) - 1;
                while(i >= 0 && (newSong->artist[i] == ' ' || newSong->artist[i] == '\n')) {
                    newSong->artist[i] = '\0';
                    i--;
                }
            } else {
                
                strncpy(newSong->title, rest, sizeof(newSong->title)-1);
                newSong->title[sizeof(newSong->title)-1] = '\0';
                newSong->artist[0] = '\0';
            }

            SongNodes *newNode = malloc(sizeof(SongNodes));
            if (!newNode) {
                free(newSong);
                printf("Memory allocation failed for song node\n");
                continue;
            }
            newNode->Song = newSong;
            newNode->nextSong = NULL;

            if(SongLibraryHead == NULL) {
                SongLibraryHead = newNode;
            }
            else {
                SongNodes *temp = SongLibraryHead;
                while(temp->nextSong != NULL) {
                    temp = temp->nextSong;
                }
                temp->nextSong = newNode;
            }
        } else {
          
            free(newSong);
        }
    }

    fclose(fp);
    printf("Songs loaded from file!\n");
}

void ListAllSongs() {
    if(SongLibraryHead == NULL) {
        printf("No songs in library!\n");
        return;
    }

    printf("\n===== All Songs in Library =====\n");
    SongNodes *cur = SongLibraryHead;
    int count = 1;

    while(cur != NULL) {
        printf("%d. ID: %d | %s | %s | %.2f min\n",
            count, cur->Song->id, cur->Song->title,
            cur->Song->artist, cur->Song->duration);
        cur = cur->nextSong;
        count++;
    }
}

void ListAllAlbums() {
    if(AlbumHead == NULL) {
        printf("No albums found!\n");
        return;
    }

    printf("\n===== All Albums =====\n");
    AlbumNodes *cur = AlbumHead;
    int count = 1;

    while(cur != NULL) {
        printf("%d. ID: %d | %s\n", count, cur->Album->id, cur->Album->title);
        cur = cur->nextAlbum;
        count++;
    }
}

void ViewAlbum() {
    int albumId = ValidId();

    AlbumNodes *albumNode = FindAlbumById(AlbumHead, albumId);

    if(albumNode == NULL) {
        printf("Album with ID %d not found!\n", albumId);
        return;
    }

    printf("\n===== Album: %s =====\n", albumNode->Album->title);

    if(albumNode->Album->SongHead == NULL) {
        printf("No songs in this album!\n");
        return;
    }

    SongNodes *cur = albumNode->Album->SongHead;
    int count = 1;

    while(cur != NULL) {
        printf("%d. ID: %d | %s | %s | %.2f min\n",
            count, cur->Song->id, cur->Song->title,
            cur->Song->artist, cur->Song->duration);
        cur = cur->nextSong;
        count++;
    }
}

void AddSongToAlbum() {
    printf("Enter Album ID: ");
    int albumId = ValidId();

    AlbumNodes *albumNode = FindAlbumById(AlbumHead, albumId);

    if(albumNode == NULL) {
        printf("Album not found!\n");
        return;
    }

    printf("Enter Song ID: ");
    int songId = ValidId();

    SongNodes *songNode = FindSongId(SongLibraryHead, songId);

    if(songNode == NULL) {
        printf("Song not found in library!\n");
        return;
    }

    SongNodes *check = FindSongId(albumNode->Album->SongHead, songId);
    if(check != NULL) {
        printf("Song already in album!\n");
        return;
    }

    SongNodes *newNode = malloc(sizeof(SongNodes));
    if (!newNode) {
        printf("Memory allocation failed\n");
        return;
    }
    newNode->Song = songNode->Song;
    newNode->nextSong = NULL;

    if(albumNode->Album->SongHead == NULL) {
        albumNode->Album->SongHead = newNode;
    }
    else {
        SongNodes *temp = albumNode->Album->SongHead;
        while(temp->nextSong != NULL) {
            temp = temp->nextSong;
        }
        temp->nextSong = newNode;
    }

    SaveAllAlbums();

    printf("Song added to album successfully!\n");
}

void DeleteSongFromAlbum() {
    printf("Enter Album ID: ");
    int albumId = ValidId();

    AlbumNodes *albumNode = FindAlbumById(AlbumHead, albumId);

    if(albumNode == NULL) {
        printf("Album not found!\n");
        return;
    }

    printf("Enter Song ID to delete: ");
    int songId = ValidId();

    SongNodes *cur = albumNode->Album->SongHead;
    SongNodes *prev = NULL;

    while(cur != NULL) {
        if(cur->Song->id == songId) {
            if(prev == NULL) {
                albumNode->Album->SongHead = cur->nextSong;
            }
            else {
                prev->nextSong = cur->nextSong;
            }
            free(cur);
            SaveAllAlbums();
            printf("Song deleted from album!\n");
            return;
        }
        prev = cur;
        cur = cur->nextSong;
    }

    printf("Song not found in album!\n");
}

void AddAlbumToPlaylist() {
    printf("Enter Album ID to add to playlist: ");
    int albumId = ValidId();

    AlbumNodes *albumNode = FindAlbumById(AlbumHead, albumId);

    if(albumNode == NULL) {
        printf("Album not found!\n");
        return;
    }

    if(albumNode->Album->SongHead == NULL) {
        printf("Album is empty!\n");
        return;
    }

    SongNodes *cur = albumNode->Album->SongHead;

    while(cur != NULL) {
        SongNodes *newNode = malloc(sizeof(SongNodes));
        if (!newNode) {
            printf("Memory allocation failed\n");
            return;
        }
        newNode->Song = cur->Song;
        newNode->nextSong = NULL;

        if(PlaylistHead == NULL) {
            PlaylistHead = newNode;
        }
        else {
            SongNodes *temp = PlaylistHead;
            while(temp->nextSong != NULL) {
                temp = temp->nextSong;
            }
            temp->nextSong = newNode;
        }

        cur = cur->nextSong;
    }

    printf("Album added to playlist!\n");
}

void PlayNext() {
    if(PlaylistHead == NULL) {
        printf("Playlist is empty!\n");
        return;
    }

    if(CurrentSong == NULL) {
        CurrentSong = PlaylistHead;
    }
    else {
        if(CurrentSong->nextSong == NULL) {
            CurrentSong = PlaylistHead;
        }
        else {
            CurrentSong = CurrentSong->nextSong;
        }
    }

    printf("\nNow Playing: %s by %s (%.2f min)\n",
        CurrentSong->Song->title,
        CurrentSong->Song->artist,
        CurrentSong->Song->duration);
}

void PlayPrevious() {
    if(PlaylistHead == NULL) {
        printf("Playlist is empty!\n");
        return;
    }

    if(CurrentSong == NULL) {
        SongNodes *temp = PlaylistHead;
        while(temp->nextSong != NULL) {
            temp = temp->nextSong;
        }
        CurrentSong = temp;
    }
    else {
        if(CurrentSong == PlaylistHead) {
            SongNodes *temp = PlaylistHead;
            while(temp->nextSong != NULL) {
                temp = temp->nextSong;
            }
            CurrentSong = temp;
        }
        else {
            SongNodes *temp = PlaylistHead;
            while(temp->nextSong != CurrentSong) {
                temp = temp->nextSong;
            }
            CurrentSong = temp;
        }
    }

    printf("\nNow Playing: %s by %s (%.2f min)\n",
        CurrentSong->Song->title,
        CurrentSong->Song->artist,
        CurrentSong->Song->duration);
}

void RemoveSongFromPlaylist() {
    if(PlaylistHead == NULL) {
        printf("Playlist is empty!\n");
        return;
    }

    printf("Enter Song ID to remove from playlist: ");
    int songId = ValidId();

    SongNodes *cur = PlaylistHead;
    SongNodes *prev = NULL;

    while(cur != NULL) {
        if(cur->Song->id == songId) {
            if(prev == NULL) {
                PlaylistHead = cur->nextSong;
            }
            else {
                prev->nextSong = cur->nextSong;
            }

            if(CurrentSong == cur) {
                CurrentSong = NULL;
            }

            free(cur);
            printf("Song removed from playlist!\n");
            return;
        }
        prev = cur;
        cur = cur->nextSong;
    }

    printf("Song not found in playlist!\n");
}

void AddSongToPlaylist() {
    printf("Enter Song ID to add to playlist: ");
    int songId = ValidId();

    SongNodes *songNode = FindSongId(SongLibraryHead, songId);

    if(songNode == NULL) {
        printf("Song not found in library!\n");
        return;
    }

    SongNodes *newNode = malloc(sizeof(SongNodes));
    if (!newNode) {
        printf("Memory allocation failed\n");
        return;
    }
    newNode->Song = songNode->Song;
    newNode->nextSong = NULL;

    if(PlaylistHead == NULL) {
        PlaylistHead = newNode;
    }
    else {
        SongNodes *temp = PlaylistHead;
        while(temp->nextSong != NULL) {
            temp = temp->nextSong;
        }
        temp->nextSong = newNode;
    }

    printf("Song added to playlist!\n");
}

void LogCommand(char *command) {
    FILE *fp = fopen("log.txt", "a");
    if(!fp) {
        return;
    }

    fprintf(fp, "%s\n", command);
    fclose(fp);
}

void DisplayLog() {
    FILE *fp = fopen("log.txt", "r");
    if(!fp) {
        printf("No log file found!\n");
        return;
    }

    printf("\n===== Command History =====\n");
    char line[200];
    int count = 1;

    while(fgets(line, sizeof(line), fp)) {
        printf("%d. %s", count, line);
        count++;
    }

    fclose(fp);
}

void LoadAlbumsFromFile() {
    FILE *fp = fopen("albums.txt", "r");
    if(!fp) {
        return;
    }

    char line[300];
    Albums *currentAlbum = NULL;

    while(fgets(line, sizeof(line), fp)) {

        if(strncmp(line, "END", 3) == 0) {
            currentAlbum = NULL;
            continue;
        }

        if(strchr(line, '|') != NULL && line[0] != 'S') {
            Albums *newAlbum = malloc(sizeof(Albums));
            if (!newAlbum) {
                printf("Memory allocation failed for album\n");
                continue;
            }

            char *pipePos = strchr(line, '|');
            char idStr[20];
            int idLen = pipePos - line;
            if (idLen >= (int)sizeof(idStr)) idLen = sizeof(idStr)-1;
            strncpy(idStr, line, idLen);
            idStr[idLen] = '\0';
            newAlbum->id = atoi(idStr);

            strncpy(newAlbum->title, pipePos + 1, sizeof(newAlbum->title)-1);
            newAlbum->title[sizeof(newAlbum->title)-1] = '\0';
            newAlbum->title[strcspn(newAlbum->title, "\n")] = '\0';

            newAlbum->SongHead = NULL;

            AlbumNodes *node = malloc(sizeof(AlbumNodes));
            if (!node) {
                free(newAlbum);
                continue;
            }
            node->Album = newAlbum;
            node->nextAlbum = NULL;

            if(AlbumHead == NULL) {
                AlbumHead = node;
            }
            else {
                AlbumNodes *temp = AlbumHead;
                while(temp->nextAlbum != NULL) {
                    temp = temp->nextAlbum;
                }
                temp->nextAlbum = node;
            }

            currentAlbum = newAlbum;
        }
        else if(line[0] == 'S' && currentAlbum != NULL) {

            int songId;
            char title[100], artist[100];
            float duration;

            if(sscanf(line, "S,%d,%[^,],%[^,],%f", &songId, title, artist, &duration) == 4) {

                SongNodes *songNode = FindSongId(SongLibraryHead, songId);

                if(songNode != NULL) {
                    SongNodes *newNode = malloc(sizeof(SongNodes));
                    if (!newNode) continue;
                    newNode->Song = songNode->Song;
                    newNode->nextSong = NULL;

                    if(currentAlbum->SongHead == NULL) {
                        currentAlbum->SongHead = newNode;
                    }
                    else {
                        SongNodes *temp = currentAlbum->SongHead;
                        while(temp->nextSong != NULL) {
                            temp = temp->nextSong;
                        }
                        temp->nextSong = newNode;
                    }
                }
            }
        }
    }

    fclose(fp);
}


int ValidId()
{
    char temp[100];
    int id;
    char garbage;

    while (1)
    {
        printf("Enter Id\n");
        if (!fgets(temp, sizeof(temp), stdin)) return -1;

        if (sscanf(temp, " %d %c", &id, &garbage) == 1)
        {
            return id;
        }
        printf("Invalid Id! Try again!\n");
    }
}

SongNodes* FindSongId(SongNodes *head, int id) {
    SongNodes *cur = head;
    while (cur != NULL) {
        if (cur->Song && cur->Song->id == id) {
            return cur;
        }
        cur = cur->nextSong;
    }
    return NULL;
}

AlbumNodes* FindAlbumById(AlbumNodes *head, int id) {
    AlbumNodes *cur = head;
    while (cur != NULL) {
        if (cur->Album && cur->Album->id == id) {
            return cur;
        }
        cur = cur->nextAlbum;
    }
    return NULL;
}

Albums* CreateAlbum()
{
    Albums *newAlbum = malloc(sizeof(Albums));
    if (!newAlbum) {
        printf("Memory error!\n");
        return NULL;
    }

    printf("Enter Album ID: ");
    if (scanf("%d", &newAlbum->id) != 1) {
        printf("Invalid input.\n");
        getchar();
        free(newAlbum);
        return NULL;
    }
    getchar();

    printf("Enter Album Title: ");
    if (!fgets(newAlbum->title, sizeof(newAlbum->title), stdin)) {
        free(newAlbum);
        return NULL;
    }
    newAlbum->title[strcspn(newAlbum->title, "\n")] = '\0';

    newAlbum->SongHead = NULL;

    AlbumNodes *node = malloc(sizeof(AlbumNodes));
    if (!node) {
        free(newAlbum);
        printf("Memory error!\n");
        return NULL;
    }
    node->Album = newAlbum;
    node->nextAlbum = NULL;

    if (AlbumHead == NULL) {
        AlbumHead = node;
    } else {
        AlbumNodes *temp = AlbumHead;
        while (temp->nextAlbum != NULL)
            temp = temp->nextAlbum;
        temp->nextAlbum = node;
    }

    SaveAllAlbums();

    printf("Album '%s' created successfully.\n", newAlbum->title);
    return newAlbum;
}

void SaveAllAlbums()
{
    FILE *fp = fopen("albums.txt", "w");
    if (!fp) {
        printf("Error opening albums.txt for saving!\n");
        return;
    }

    AlbumNodes *A = AlbumHead;

    while (A != NULL) {
        Albums *alb = A->Album;
        fprintf(fp, "%d|%s\n", alb->id, alb->title);

        SongNodes *S = alb->SongHead;
        while (S != NULL) {
            fprintf(fp, "S,%d,%s,%s,%.2f\n",
                S->Song->id,
                S->Song->title,
                S->Song->artist,
                S->Song->duration
            );
            S = S->nextSong;
        }

        fprintf(fp, "END\n");
        A = A->nextAlbum;
    }

    fclose(fp);
}
