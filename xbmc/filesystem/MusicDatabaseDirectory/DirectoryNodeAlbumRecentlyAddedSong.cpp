/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DirectoryNodeAlbumRecentlyAddedSong.h"
#include "music/MusicDatabase.h"

using namespace XFILE::MUSICDATABASEDIRECTORY;

CDirectoryNodeAlbumRecentlyAddedSong::CDirectoryNodeAlbumRecentlyAddedSong(const std::string& strName, XFILE::MEDIADIRECTORY::CDirectoryNode* pParent, const std::string& strOrigin)
  : CDirectoryNode(XFILE::MEDIADIRECTORY::NODE_TYPE_ALBUM_RECENTLY_ADDED_SONGS, strName, pParent, strOrigin)
{

}

bool CDirectoryNodeAlbumRecentlyAddedSong::GetContent(CFileItemList& items) const
{
  CMusicDatabase musicdatabase;
  if (!musicdatabase.Open())
    return false;

  std::string strBaseDir=BuildPath();
  bool bSuccess=musicdatabase.GetRecentlyAddedAlbumSongs(strBaseDir, items);

  musicdatabase.Close();

  return bSuccess;
}
