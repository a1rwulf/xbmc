/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DirectoryNodeAlbumTop100.h"
#include "music/MusicDatabase.h"
#include "FileItem.h"
#include "utils/StringUtils.h"

using namespace XFILE::MUSICDATABASEDIRECTORY;

CDirectoryNodeAlbumTop100::CDirectoryNodeAlbumTop100(const std::string& strName, XFILE::MEDIADIRECTORY::CDirectoryNode* pParent, const std::string& strOrigin)
  : CDirectoryNode(XFILE::MEDIADIRECTORY::NODE_TYPE_ALBUM_TOP100, strName, pParent, strOrigin)
{

}

XFILE::MEDIADIRECTORY::NODE_TYPE CDirectoryNodeAlbumTop100::GetChildType() const
{
  if (GetName()=="-1")
    return XFILE::MEDIADIRECTORY::NODE_TYPE_ALBUM_TOP100_SONGS;

  return XFILE::MEDIADIRECTORY::NODE_TYPE_SONG;
}

std::string CDirectoryNodeAlbumTop100::GetLocalizedName() const
{
  CMusicDatabase db;
  if (db.Open())
    return db.GetAlbumById(GetID());
  return "";
}

bool CDirectoryNodeAlbumTop100::GetContent(CFileItemList& items) const
{
  CMusicDatabase musicdatabase;
  if (!musicdatabase.Open())
    return false;

  VECALBUMS albums;
  if (!musicdatabase.GetTop100Albums(albums))
  {
    musicdatabase.Close();
    return false;
  }

  for (int i=0; i<(int)albums.size(); ++i)
  {
    CAlbum& album=albums[i];
    std::string strDir = StringUtils::Format("%s%ld/", BuildPath().c_str(), album.idAlbum);
    CFileItemPtr pItem(new CFileItem(strDir, album));
    items.Add(pItem);
  }

  musicdatabase.Close();

  return true;
}
