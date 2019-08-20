/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DirectoryNodeAlbumCompilationsSongs.h"
#include "filesystem/MediaDirectory/QueryParams.h"
#include "music/MusicDatabase.h"

using namespace XFILE::MUSICDATABASEDIRECTORY;

CDirectoryNodeAlbumCompilationsSongs::CDirectoryNodeAlbumCompilationsSongs(const std::string& strName, XFILE::MEDIADIRECTORY::CDirectoryNode* pParent, const std::string& strOrigin)
  : CDirectoryNode(XFILE::MEDIADIRECTORY::NODE_TYPE_ALBUM_COMPILATIONS_SONGS, strName, pParent, strOrigin)
{

}


bool CDirectoryNodeAlbumCompilationsSongs::GetContent(CFileItemList& items) const
{
  CMusicDatabase musicdatabase;
  if (!musicdatabase.Open())
    return false;

  XFILE::MEDIADIRECTORY::CQueryParams params;
  CollectQueryParams(params);

  bool bSuccess=musicdatabase.GetCompilationSongs(BuildPath(), items);

  musicdatabase.Close();

  return bSuccess;
}
