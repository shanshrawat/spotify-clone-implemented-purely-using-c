#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Functions.h"
#include "Structs.h"

AlbumNodes *AlbumHead = NULL;

int ValidId()
{

    char temp[100];
    int id;
    char garbage;

    while (1)
    {

        printf("Enter Id\n");
        fgets(temp, sizeof(temp), stdin);

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
        if (cur->Song->id == id) {
            return cur;  
        }
        cur = cur->nextSong;
    }

    return NULL;          
}

AlbumNodes* FindAlbumById(AlbumNodes *head, int id) {
    AlbumNodes *cur = head;

    while (cur != NULL) {
        if (cur->Album->id == id) {
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
    scanf("%d", &newAlbum->id);
    getchar();

    printf("Enter Album Title: ");
    fgets(newAlbum->title, sizeof(newAlbum->title), stdin);
    newAlbum->title[strcspn(newAlbum->title, "\n")] = '\0';

    newAlbum->SongHead = NULL;  


    AlbumNodes *node = malloc(sizeof(AlbumNodes));
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


