/*
 *  Copyright (C) 2012-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "dbwrappers/Database.h"

#include <string>
#include <vector>

class CMusicPlaylist;
class CFileItemList;
class SortDescription;

namespace METADATA
{
enum SupportedEntities
{
  Nothing = 0,
  Album = 1,
  AlbumCompilation = 1 << 1,
  AlbumCompilationSongs = 1 << 2,
  AlbumRecentlyAdded = 1 << 3,
  AlbumRecentlyAddedSong = 1 << 4,
  AlbumRecentlyPlayed = 1 << 5,
  AlbumRecentlyPlayedSong = 1 << 6,
  AlbumTop100 = 1 << 7,
  AlbumTop100Song = 1 << 8,
  Artist = 1 << 9,
  Singles = 1 << 10,
  Song = 1 << 11,
  SongTop100 = 1 << 12,
  Top100 = 1 << 13,
  YearAlbum = 1 << 14,
  YearSong = 1 << 15,
  Playlist = 1 << 16,
  Everything = 0xFFFF
};

class IMetadataProvider
{
public:
  IMetadataProvider() = default;
  virtual ~IMetadataProvider() = default;

  virtual bool GetPlaylists(const std::string& strBaseDir,
                            CFileItemList& items,
                            const SortDescription& sortDescription,
                            bool countOnly) = 0;

  virtual bool GetSongs(const std::string& strBaseDir,
              CFileItemList& items,
              int idGenre,
              int idArtist,
              int idAlbum,
              int idPlaylist,
              const SortDescription &sortDescription) = 0;

  virtual SupportedEntities GetSupportedEntities() = 0;

protected:
  uint32_t m_supportedEntities;
};
} // namespace METADATA
