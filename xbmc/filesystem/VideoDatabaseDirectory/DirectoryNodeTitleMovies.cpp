/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DirectoryNodeTitleMovies.h"
#include "filesystem/MediaDirectory/QueryParams.h"
#include "video/VideoDatabase.h"

using namespace XFILE::VIDEODATABASEDIRECTORY;

CDirectoryNodeTitleMovies::CDirectoryNodeTitleMovies(const std::string& strName, XFILE::MEDIADIRECTORY::CDirectoryNode* pParent, const std::string& strOrigin)
  : CDirectoryNode(XFILE::MEDIADIRECTORY::NODE_TYPE_TITLE_MOVIES, strName, pParent, strOrigin)
{

}

bool CDirectoryNodeTitleMovies::GetContent(CFileItemList& items) const
{
  CVideoDatabase videodatabase;
  if (!videodatabase.Open())
    return false;

  XFILE::MEDIADIRECTORY::CQueryParams params;
  CollectQueryParams(params);

  bool bSuccess=videodatabase.GetMoviesNav(BuildPath(), items, params.GetGenreId(), params.GetYear(), params.GetActorId(), params.GetDirectorId(), params.GetStudioId(), params.GetCountryId(), params.GetSetId(), params.GetTagId());

  videodatabase.Close();

  return bSuccess;
}
