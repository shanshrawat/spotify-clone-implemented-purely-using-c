#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Functions.h"
#include "Structs.h"

SongNodes *SongLibraryHead = NULL;
SongNodes *PlaylistHead = NULL;
SongNodes *CurrentSong = NULL;

void PrintMenu();

int main() {
    LoadSongsFromFile();
    LoadAlbumsFromFile();

    int choice;
    char temp[100];
    char garbage;

    while(1) {
        PrintMenu();

        if (!fgets(temp, sizeof(temp), stdin)) {
            printf("Input error, exiting.\n");
            break;
        }

        if(sscanf(temp, " %d %c", &choice, &garbage) != 1) {
            printf("Invalid choice! Try again!\n");
            LogCommand("Invalid Input");
            continue;
        }

        if(choice == 1) {
            LogCommand("List All Songs");
            ListAllSongs();
        }
        else if(choice == 2) {
            LogCommand("List All Albums");
            ListAllAlbums();
        }
        else if(choice == 3) {
            LogCommand("View Album");
            ViewAlbum();
        }
        else if(choice == 4) {
            LogCommand("Add Song to Album");
            AddSongToAlbum();
        }
        else if(choice == 5) {
            LogCommand("Delete Song from Album");
            DeleteSongFromAlbum();
        }
        else if(choice == 6) {
            LogCommand("Create Album");
            CreateAlbum();
        }
        else if(choice == 7) {
            LogCommand("Add Album to Playlist");
            AddAlbumToPlaylist();
        }
        else if(choice == 8) {
            LogCommand("Play Next");
            PlayNext();
        }
        else if(choice == 9) {
            LogCommand("Play Previous");
            PlayPrevious();
        }
        else if(choice == 10) {
            LogCommand("Remove Song from Playlist");
            RemoveSongFromPlaylist();
        }
        else if(choice == 11) {
            LogCommand("Add Song to Playlist");
            AddSongToPlaylist();
        }
        else if(choice == 12) {
            LogCommand("Display Command Log");
            DisplayLog();
        }
        else if(choice == 13) {
            LogCommand("Exit");
            printf("Goodbye!\n");
            break;
        }
        else {
            printf("Invalid choice! Try again!\n");
            LogCommand("Invalid Choice");
        }
    }

    return 0;
}

void PrintMenu() {
    printf("\n========== C-Unplugged Music App ==========\n");
    printf("1. List all songs in library\n");
    printf("2. List all albums\n");
    printf("3. View, Add, Delete songs in a specific album\n");
    printf("4. Add song to album\n");
    printf("5. Delete song from album\n");
    printf("6. Create new album\n");
    printf("7. Add album to current playlist\n");
    printf("8. Play next song\n");
    printf("9. Play previous song\n");
    printf("10. Remove song from playlist\n");
    printf("11. Add song to playlist\n");
    printf("12. Display command log\n");
    printf("13. Exit\n");
    printf("Enter your choice (1,2...n): ");
}
