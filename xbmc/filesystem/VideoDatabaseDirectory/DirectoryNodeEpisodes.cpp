/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DirectoryNodeEpisodes.h"
#include "filesystem/MediaDirectory/QueryParams.h"
#include "video/VideoDatabase.h"

using namespace XFILE::VIDEODATABASEDIRECTORY;

CDirectoryNodeEpisodes::CDirectoryNodeEpisodes(const std::string& strName, XFILE::MEDIADIRECTORY::CDirectoryNode* pParent, const std::string& strOrigin)
  : CDirectoryNode(XFILE::MEDIADIRECTORY::NODE_TYPE_EPISODES, strName, pParent, strOrigin)
{

}

bool CDirectoryNodeEpisodes::GetContent(CFileItemList& items) const
{
  CVideoDatabase videodatabase;
  if (!videodatabase.Open())
    return false;

  XFILE::MEDIADIRECTORY::CQueryParams params;
  CollectQueryParams(params);

  int season = (int)params.GetSeason();
  if (season == -2)
    season = -1;

  bool bSuccess=videodatabase.GetEpisodesNav(BuildPath(), items, params.GetGenreId(), params.GetYear(), params.GetActorId(), params.GetDirectorId(), params.GetTvShowId(), season);

  videodatabase.Close();

  return bSuccess;
}

XFILE::MEDIADIRECTORY::NODE_TYPE CDirectoryNodeEpisodes::GetChildType() const
{
  return XFILE::MEDIADIRECTORY::NODE_TYPE_EPISODES;
}
