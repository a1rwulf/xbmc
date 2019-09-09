/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "MusicInfoTagLoaderDatabase.h"
#include "music/MusicDatabase.h"
#include "filesystem/MediaDirectory.h"
#include "filesystem/MediaDirectory/DirectoryNode.h"
#include "MusicInfoTag.h"

using namespace MUSIC_INFO;

CMusicInfoTagLoaderDatabase::CMusicInfoTagLoaderDatabase(void) = default;

CMusicInfoTagLoaderDatabase::~CMusicInfoTagLoaderDatabase() = default;

bool CMusicInfoTagLoaderDatabase::Load(const std::string& strFileName, CMusicInfoTag& tag, EmbeddedArt *art)
{
  tag.SetLoaded(false);
  CMusicDatabase database;
  database.Open();
  XFILE::MEDIADIRECTORY::CQueryParams param;
  XFILE::MEDIADIRECTORY::CDirectoryNode::GetDatabaseInfo(strFileName,param);

  CSong song;
  if (database.GetSong(param.GetSongId(),song))
    tag.SetSong(song);

  database.Close();

  return tag.Loaded();
}

