/*
 *  Copyright (C) 2012-2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DatabaseMetadataProvider.h"

#include "dbwrappers/Database.h"
#include "music/MusicDatabase.h"
#include "utils/log.h"

using namespace METADATA;

CDatabaseMetadataProvider::CDatabaseMetadataProvider()
    : IMetadataProvider()
{
  m_supportedEntities = SupportedEntities::Everything;
  m_supportedEntities &= ~SupportedEntities::Playlist;
}

bool CDatabaseMetadataProvider::GetPlaylists(const std::string& strBaseDir,
                                             CFileItemList& items,
                                             const CDatabase::Filter& filter,
                                             const SortDescription& sortDescription,
                                             bool countOnly)
{
  CLog::Log(LOGNOTICE, "CDatabaseMetadataProvider::%s - GetPlaylists", __FUNCTION__);
  CMusicDatabase musicdb;
  musicdb.Open();
  return musicdb.GetPlaylistsNav(strBaseDir, items, filter, sortDescription, countOnly);
}

bool CDatabaseMetadataProvider::GetSongs(const std::string& strBaseDir,
              CFileItemList& items,
              int idGenre,
              int idArtist,
              int idAlbum,
              int idPlaylist,
              const SortDescription &sortDescription)
{
  CLog::Log(LOGNOTICE, "CDatabaseMetadataProvider::%s - GetSongs", __FUNCTION__);
  CMusicDatabase musicdb;
  musicdb.Open();
  return musicdb.GetSongsNav(strBaseDir, items, idGenre, idArtist, idAlbum, idPlaylist, sortDescription);
}

SupportedEntities CDatabaseMetadataProvider::GetSupportedEntities()
{
  return static_cast<SupportedEntities>(m_supportedEntities);
}
