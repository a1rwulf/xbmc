set(SOURCES DirectoryNodeAlbumCompilations.cpp
            DirectoryNodeAlbumCompilationsSongs.cpp
            DirectoryNodeAlbum.cpp
            DirectoryNodeAlbumRecentlyAdded.cpp
            DirectoryNodeAlbumRecentlyAddedSong.cpp
            DirectoryNodeAlbumRecentlyPlayed.cpp
            DirectoryNodeAlbumRecentlyPlayedSong.cpp
            DirectoryNodeAlbumTop100.cpp
            DirectoryNodeAlbumTop100Song.cpp
            DirectoryNodeArtist.cpp
            DirectoryNodeSingles.cpp
            DirectoryNodeSong.cpp
            DirectoryNodeSongTop100.cpp
            DirectoryNodeTop100.cpp
            DirectoryNodeYearAlbum.cpp
            DirectoryNodeYearSong.cpp
            DirectoryNodePlaylist.cpp)

set(HEADERS DirectoryNodeAlbum.h
            DirectoryNodeAlbumCompilations.h
            DirectoryNodeAlbumCompilationsSongs.h
            DirectoryNodeAlbumRecentlyAdded.h
            DirectoryNodeAlbumRecentlyAddedSong.h
            DirectoryNodeAlbumRecentlyPlayed.h
            DirectoryNodeAlbumRecentlyPlayedSong.h
            DirectoryNodeAlbumTop100.h
            DirectoryNodeAlbumTop100Song.h
            DirectoryNodeArtist.h
            DirectoryNodeSingles.h
            DirectoryNodeSong.h
            DirectoryNodeSongTop100.h
            DirectoryNodeTop100.h
            DirectoryNodeYearAlbum.h
            DirectoryNodeYearSong.h
            DirectoryNodePlaylist.h)

core_add_library(musicdatabasedirectory)
