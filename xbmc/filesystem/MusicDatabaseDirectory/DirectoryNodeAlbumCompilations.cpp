/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DirectoryNodeAlbumCompilations.h"
#include "filesystem/MediaDirectory/QueryParams.h"
#include "guilib/LocalizeStrings.h"
#include "music/MusicDatabase.h"

using namespace XFILE::MUSICDATABASEDIRECTORY;

CDirectoryNodeAlbumCompilations::CDirectoryNodeAlbumCompilations(const std::string& strName, XFILE::MEDIADIRECTORY::CDirectoryNode* pParent, const std::string& strOrigin)
  : CDirectoryNode(XFILE::MEDIADIRECTORY::NODE_TYPE_ALBUM_COMPILATIONS, strName, pParent, strOrigin)
{

}

XFILE::MEDIADIRECTORY::NODE_TYPE CDirectoryNodeAlbumCompilations::GetChildType() const
{
  if (GetName()=="-1")
    return XFILE::MEDIADIRECTORY::NODE_TYPE_ALBUM_COMPILATIONS_SONGS;

  return XFILE::MEDIADIRECTORY::NODE_TYPE_SONG;
}

std::string CDirectoryNodeAlbumCompilations::GetLocalizedName() const
{
  if (GetID() == -1)
    return g_localizeStrings.Get(15102); // All Albums
  CMusicDatabase db;
  if (db.Open())
    return db.GetAlbumById(GetID());
  return "";
}

bool CDirectoryNodeAlbumCompilations::GetContent(CFileItemList& items) const
{
  CMusicDatabase musicdatabase;
  if (!musicdatabase.Open())
    return false;

  XFILE::MEDIADIRECTORY::CQueryParams params;
  CollectQueryParams(params);

  bool bSuccess=musicdatabase.GetCompilationAlbums(BuildPath(), items);

  musicdatabase.Close();

  return bSuccess;
}
