set(SOURCES DirectoryNodeEpisodes.cpp
            DirectoryNodeInProgressTvShows.cpp
            DirectoryNodeMoviesOverview.cpp
            DirectoryNodeMusicVideosOverview.cpp
            DirectoryNodeRecentlyAddedEpisodes.cpp
            DirectoryNodeRecentlyAddedMovies.cpp
            DirectoryNodeRecentlyAddedMusicVideos.cpp
            DirectoryNodeSeasons.cpp
            DirectoryNodeTitleMovies.cpp
            DirectoryNodeTitleMusicVideos.cpp
            DirectoryNodeTitleTvShows.cpp
            DirectoryNodeTvShowsOverview.cpp)

set(HEADERS DirectoryNodeEpisodes.h
            DirectoryNodeInProgressTvShows.h
            DirectoryNodeMoviesOverview.h
            DirectoryNodeMusicVideosOverview.h
            DirectoryNodeRecentlyAddedEpisodes.h
            DirectoryNodeRecentlyAddedMovies.h
            DirectoryNodeRecentlyAddedMusicVideos.h
            DirectoryNodeSeasons.h
            DirectoryNodeTitleMovies.h
            DirectoryNodeTitleMusicVideos.h
            DirectoryNodeTitleTvShows.h
            DirectoryNodeTvShowsOverview.h)

core_add_library(videodatabasedirectory)
