/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DirectoryNodeRecentlyAddedMusicVideos.h"
#include "video/VideoDatabase.h"

using namespace XFILE::VIDEODATABASEDIRECTORY;

CDirectoryNodeRecentlyAddedMusicVideos::CDirectoryNodeRecentlyAddedMusicVideos(const std::string& strName, XFILE::MEDIADIRECTORY::CDirectoryNode* pParent, const std::string& strOrigin)
  : CDirectoryNode(XFILE::MEDIADIRECTORY::NODE_TYPE_RECENTLY_ADDED_MUSICVIDEOS, strName, pParent, strOrigin)
{

}

bool CDirectoryNodeRecentlyAddedMusicVideos::GetContent(CFileItemList& items) const
{
  CVideoDatabase videodatabase;
  if (!videodatabase.Open())
    return false;

  bool bSuccess=videodatabase.GetRecentlyAddedMusicVideosNav(BuildPath(), items);

  videodatabase.Close();

  return bSuccess;
}

