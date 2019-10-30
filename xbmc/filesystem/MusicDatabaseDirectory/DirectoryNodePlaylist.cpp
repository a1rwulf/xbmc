/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DirectoryNodePlaylist.h"
#include "filesystem/MediaDirectory/QueryParams.h"
#include "guilib/LocalizeStrings.h"
#include "media/MetadataManager.h"
#include "music/MusicDatabase.h"
#include "ServiceBroker.h"

using namespace XFILE::MUSICDATABASEDIRECTORY;

CDirectoryNodePlaylist::CDirectoryNodePlaylist(const std::string& strName, XFILE::MEDIADIRECTORY::CDirectoryNode* pParent, const std::string& strOrigin)
    : XFILE::MEDIADIRECTORY::CDirectoryNode(XFILE::MEDIADIRECTORY::NODE_TYPE_PLAYLIST, strName, pParent, strOrigin)
{

}

XFILE::MEDIADIRECTORY::NODE_TYPE CDirectoryNodePlaylist::GetChildType() const
{
  return XFILE::MEDIADIRECTORY::NODE_TYPE_SONG;
}

bool CDirectoryNodePlaylist::GetContent(CFileItemList& items) const
{
  CMusicDatabase musicdatabase;
  if (!musicdatabase.Open())
    return false;

  XFILE::MEDIADIRECTORY::CQueryParams params;
  CollectQueryParams(params);

  std::string strBaseDir=BuildPath();
  bool bSuccess = CServiceBroker::GetMetadataManager().GetPlaylists(strBaseDir, items, SortDescription(), false);
  musicdatabase.Close();

  return bSuccess;
}

std::string CDirectoryNodePlaylist::GetLocalizedName() const
{
  if (GetID() == -1)
    return g_localizeStrings.Get(80001); // All playlists
  CMusicDatabase db;
  if (db.Open())
  {
    CODBPlaylist playlist;
    if (db.GetPlaylistById(GetID(), playlist))
      return playlist.m_name;
  }

  return "";
}
