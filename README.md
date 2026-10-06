# Spotify Clone Implemented Purely in C

A lightweight, high-performance, command-line music library and playlist manager built from scratch in pure C. This project simulates core streaming-service capabilities—such as music library ingestion, album structuring, custom playlist queueing, circular track navigation, command auditing, and persistent disk storage—using low-level pointer manipulation and dynamic memory management.

---

## 🧠 My Understanding & Core Architecture of the Project

Below is my breakdown of how the entire system is architected, how memory and pointers flow across structures, and how low-level systems programming concepts are applied to build this application.

### 1. Pointer Architecture & Single Source of Truth
The central design challenge in a music management system is avoiding redundant data copies while allowing songs to exist simultaneously in a master library, inside multiple albums, and within an active playback queue.

```
+-------------------------------------------------------------------+
|                        Song Library (Head)                        |
|  [SongNode 1] -------> [SongNode 2] -------> [SongNode 3] ...     |
|       |                     |                     |               |
|       v                     v                     v               |
|  (Songs: ID 1)         (Songs: ID 2)         (Songs: ID 3)        |
+-------^---------------------^-------------------------------------+
        |                     |
        | [SongNode]          | [SongNode]
        |      ^              |      ^
        |      |              |      |
+-------+------|--------------+------|------------------------------+
| Album 1 SongHead      | Album 2 SongHead                          |
+-----------------------+-------------------------------------------+
        |
        | [SongNode]
        |      ^
+-------+------|----------------------------------------------------+
| PlaylistHead                                                      |
+-------------------------------------------------------------------+
```

- **Master Library as Canonical Storage**: When songs are ingested from `SongsCollection.txt`, a single `Songs` structure is allocated on the heap for each track. `SongLibraryHead` maintains a linked list of `SongNodes`, each pointing to one `Songs` instance.
- **Reference Sharing (Zero Redundancy)**: When a song is added to an album or playlist, only a lightweight `SongNodes` wrapper node is allocated on the heap. Its `Song` pointer directly references the canonical `Songs` struct in the library. This guarantees:
  - Zero memory duplication for metadata strings (`title`, `artist`).
  - Strict data consistency across the entire runtime.
  - Minimal memory footprint.

### 2. Strict Memory Ownership & Leak-Free Teardown
Because `Songs` pointers are shared across multiple linked lists, memory ownership must be strictly defined:
- **`SongLibraryHead`** has *primary ownership* over both its `SongNodes` and the underlying `Songs` data structs.
- **`AlbumHead`** has *structural ownership* over `AlbumNodes` and `Albums` containers, and its `SongHead` chain owns only its intermediate `SongNodes`. It **must not** free the underlying `Songs` struct when deleting a track.
- **`PlaylistHead`** has *transient ownership* solely over its queue `SongNodes`.
- **Teardown (`FreeAllMemory`)**: Upon exiting the program, memory is freed in strict reverse order:
  1. `PlaylistHead` nodes are traversed and freed (leaving `Songs` untouched).
  2. Each album in `AlbumHead` has its internal `SongHead` list freed, then the `Albums` struct, and finally the `AlbumNodes` wrapper.
  3. `SongLibraryHead` traverses and frees both the `Songs` struct and the `SongNodes` wrapper.
  4. Global head pointers (`PlaylistHead`, `AlbumHead`, `SongLibraryHead`, `CurrentSong`) are reset to `NULL`.

### 3. Circular Playback on Singly Linked Lists
The playback queue mimics Spotify's continuous play mode on top of a singly linked list (`nextSong` only):
- **`PlayNext()`**:
  - Moves `CurrentSong` to `CurrentSong->nextSong`.
  - If `CurrentSong->nextSong == NULL` (end of queue), it circles back to `PlaylistHead`.
- **`PlayPrevious()`**:
  - Singly linked lists have no `prev` pointer. To move backwards, the function finds the predecessor node by scanning from `PlaylistHead` until `temp->nextSong == CurrentSong`.
  - If `CurrentSong == PlaylistHead` (start of queue), it scans to the tail of `PlaylistHead` and sets `CurrentSong = tail`, enabling seamless backward wrapping.
  - Guarded against dangling pointers: `CurrentSong` is validated against `PlaylistHead` prior to traversal, preventing segmentation faults.

### 4. File Persistence & Custom Parsing Protocols
All application data is persisted to disk using deterministic text protocols:
- **`SongsCollection.txt`** (`<id> <min>:<sec> <title> | <artist>`):
  - Ingested at startup. Time is converted into decimal minutes: $\text{duration} = \text{min} + \frac{\text{sec}}{60.0}$.
  - Custom whitespace tokenizers strip leading/trailing spaces, horizontal tabs (`\t`), and carriage returns (`\r\n`) to ensure cross-platform compatibility across Windows, Linux, and macOS.
- **`albums.txt`** (Hierarchical block structure):
  - Albums are demarcated by `<id>|<title>`, followed by track lines (`S,<id>,<title>,<artist>,<duration>`), and terminated by `END`.
  - The parser reads the album header, instantiates the album node, and links songs by extracting `songId` via `sscanf(line, "S,%d", &songId)` and performing an $O(N)$ lookup in `SongLibraryHead`. Decoupling the ID from track title/artist ensures titles containing commas do not break the parser.
- **`log.txt`**:
  - Every valid, invalid, and navigational command is logged in append mode (`"a"`).
  - Can be inspected anytime through the in-app command history viewer.

### 5. Input Validation & Fault Tolerance
- **Buffer Safety**: Replaced unsafe standard `scanf`/`getchar` combinations with `fgets` + `sscanf` validation loops in `ValidId()`. This eliminates unread newline leftovers and prevents malformed strings from causing infinite loops in the menu.
- **Constraint Enforcement**: IDs are strictly validated to be positive integers (`id > 0`).
- **Duplicate Prevention**: `CreateAlbum()` verifies that the requested ID does not already exist in `AlbumHead` before allocating memory, preventing hidden duplicate records and un-viewable albums.
- **EOF / Pipe Handling**: If `stdin` is closed (e.g. piped automated tests), functions abort safely without dereferencing `-1`.

---

## 🛠️ Data Structures

Defined in `Structs.h`:

```c
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
```

---

## 📋 Features & Menu Options

| # | Feature | Description |
|---|---|---|
| `1` | **List All Songs** | Displays all tracks present in the master library with ID, title, artist, and duration. |
| `2` | **List All Albums** | Prints all registered albums with their unique IDs and titles. |
| `3` | **View Album** | Displays all songs contained within a specific album. |
| `4` | **Add Song to Album** | Adds a song from the master library into an album (duplicates rejected). |
| `5` | **Delete Song from Album**| Removes a track from an album and persists the updated state to `albums.txt`. |
| `6` | **Create New Album** | Prompts for a unique ID and title, initializes an album, and appends to disk. |
| `7` | **Add Album to Playlist** | Enqueues all songs from an album into the active playback queue. |
| `8` | **Play Next Song** | Advances playback queue with circular loop back to head. |
| `9` | **Play Previous Song** | Steps backward in playback queue with circular loop to tail. |
| `10`| **Remove Song from Playlist** | Evicts a specific song from the active playlist; safely resets `CurrentSong`. |
| `11`| **Add Song to Playlist** | Enqueues an individual track from library into the playlist. |
| `12`| **Display Command Log** | Prints timestamped/indexed audit history of all session commands from `log.txt`. |
| `13`| **Exit** | Flushes remaining state, frees all dynamic heap allocations, and terminates cleanly. |

---

## 📂 Project Structure

```
├── Structs.h               # Data structures (Songs, SongNodes, Albums, AlbumNodes)
├── Functions.h             # Function prototypes and global extern declarations
├── FunctionsAssignment.c   # Core implementation: parsing, list operations, file I/O
├── MainCode.c              # Application entry point, menu dispatch loop, memory teardown
├── SongsCollection.txt     # Initial library dataset
├── albums.txt              # Persistent storage for user-created albums
├── log.txt                 # Persistent session command history audit log
└── README.md               # Technical documentation & architectural analysis
```

---

## 🚀 Building & Running

### Prerequisites
- GCC / Clang / MSVC supporting C99 or later.

### Compilation
Compile with all strict diagnostic warnings:

```bash
gcc -Wall -Wextra -Wpedantic -std=c99 -o music.exe MainCode.c FunctionsAssignment.c
```

### Execution
Run the compiled binary:

```bash
# Windows
./music.exe

# Linux / macOS
./music
```

---

## 🧪 Edge Cases Handled

- **CRLF & Cross-Platform Line Endings**: Correctly strips both `\r` and `\n` so files saved in Windows run seamlessly on Linux/macOS.
- **Titles with Commas**: Robust parsing prevents corruption when track titles or artists contain commas.
- **Empty Queues / Non-Existent IDs**: Operations on empty playlists or invalid IDs return user-friendly error notices without crashes.
- **Memory Leaks**: All allocated blocks are tracked and freed upon exit; verified clean under memory audit tools.

---

## 👤 Author
- **GitHub**: [@shanshrawat](https://github.com/shanshrawat)
