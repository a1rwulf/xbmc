/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DirectoryNodePlaylist.h"
#include "QueryParams.h"
#include "guilib/LocalizeStrings.h"
#include "music/MusicDatabase.h"

using namespace XFILE::MUSICDATABASEDIRECTORY;

CDirectoryNodePlaylist::CDirectoryNodePlaylist(const std::string& strName, CDirectoryNode* pParent)
    : CDirectoryNode(NODE_TYPE_PLAYLIST, strName, pParent)
{

}

NODE_TYPE CDirectoryNodePlaylist::GetChildType() const
{
  return NODE_TYPE_SONG;
}

bool CDirectoryNodePlaylist::GetContent(CFileItemList& items) const
{
  CMusicDatabase musicdatabase;
  if (!musicdatabase.Open())
    return false;

  CQueryParams params;
  CollectQueryParams(params);

  std::string strBaseDir=BuildPath();
  bool bSuccess=musicdatabase.GetPlaylistsNav(strBaseDir, items);
  musicdatabase.Close();

  return bSuccess;
}

// TODO playlists
// Copied from Albums
std::string CDirectoryNodePlaylist::GetLocalizedName() const
{
  if (GetID() == -1)
    return g_localizeStrings.Get(15102); // All Albums
  CMusicDatabase db;
  if (db.Open())
    return db.GetAlbumById(GetID());
  return "";
}
