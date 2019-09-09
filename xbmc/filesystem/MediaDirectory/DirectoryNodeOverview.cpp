/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DirectoryNodeOverview.h"
#include "FileItem.h"
#include "ServiceBroker.h"
#include "music/MusicDatabase.h"
#include "guilib/LocalizeStrings.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"
#include "utils/StringUtils.h"
#include "video/VideoDatabase.h"

namespace XFILE
{
  namespace MEDIADIRECTORY
  {
    Node OverviewChildren[] = {
                                { NODE_TYPE_GENRE,                      "genres",               135 },
                                { NODE_TYPE_ARTIST,                     "artists",              133 },
                                { NODE_TYPE_ALBUM,                      "albums",               132 },
                                { NODE_TYPE_SINGLES,                    "singles",              1050 },
                                { NODE_TYPE_SONG,                       "songs",                134 },
                                { NODE_TYPE_YEAR,                       "years",                652 },
                                { NODE_TYPE_TOP100,                     "top100",               271 },
                                { NODE_TYPE_ALBUM_RECENTLY_ADDED,       "recentlyaddedalbums",  359 },
                                { NODE_TYPE_ALBUM_RECENTLY_PLAYED,      "recentlyplayedalbums", 517 },
                                { NODE_TYPE_ALBUM_COMPILATIONS,         "compilations",         521 },
                                { NODE_TYPE_ROLE,                       "roles",              38033 },
                                { NODE_TYPE_SOURCE,                     "sources",            39031 },
                                { NODE_TYPE_PLAYLIST,                   "playlists",            136 },
                                { NODE_TYPE_MOVIES_OVERVIEW,            "movies",                   342 },
                                { NODE_TYPE_TVSHOWS_OVERVIEW,           "tvshows",                  20343 },
                                { NODE_TYPE_MUSICVIDEOS_OVERVIEW,       "musicvideos",              20389 },
                                { NODE_TYPE_RECENTLY_ADDED_MOVIES,      "recentlyaddedmovies",      20386 },
                                { NODE_TYPE_RECENTLY_ADDED_EPISODES,    "recentlyaddedepisodes",    20387 },
                                { NODE_TYPE_RECENTLY_ADDED_MUSICVIDEOS, "recentlyaddedmusicvideos", 20390 },
                                { NODE_TYPE_INPROGRESS_TVSHOWS,         "inprogresstvshows",        626 }
                              };
  };
};

using namespace XFILE::MEDIADIRECTORY;

CDirectoryNodeOverview::CDirectoryNodeOverview(const std::string& strName, CDirectoryNode* pParent, const std::string& strOrigin)
  : CDirectoryNode(NODE_TYPE_OVERVIEW, strName, pParent, strOrigin)
{

}

NODE_TYPE CDirectoryNodeOverview::GetChildType() const
{
  for (const Node& node : OverviewChildren)
    if (GetName() == node.id)
      return node.node;
  return NODE_TYPE_NONE;
}

std::string CDirectoryNodeOverview::GetLocalizedName() const
{
  for (const Node& node : OverviewChildren)
    if (GetName() == node.id)
      return g_localizeStrings.Get(node.label);
  return "";
}

bool CDirectoryNodeOverview::GetContent(CFileItemList& items) const
{
  CMusicDatabase musicDatabase;
  musicDatabase.Open();

  bool hasSingles = (musicDatabase.GetSinglesCount() > 0);
  bool hasCompilations = (musicDatabase.GetCompilationAlbumsCount() > 0);

  CVideoDatabase database;
  database.Open();
  bool hasMovies = database.HasContent(VIDEODB_CONTENT_MOVIES);
  bool hasTvShows = database.HasContent(VIDEODB_CONTENT_TVSHOWS);
  bool hasMusicVideos = database.HasContent(VIDEODB_CONTENT_MUSICVIDEOS);
  std::vector<std::pair<const char*, int> > vec;

  vec.push_back(std::make_pair("genres", 135));
  vec.push_back(std::make_pair("artists", 133));
  vec.push_back(std::make_pair("albums", 132));
  vec.push_back(std::make_pair("singles", 1050));
  vec.push_back(std::make_pair("songs", 134));
  vec.push_back(std::make_pair("years", 652));
  vec.push_back(std::make_pair("top100", 271));
  vec.push_back(std::make_pair("recentlyaddedalbums", 359));
  vec.push_back(std::make_pair("recentlyplayedalbums", 517));
  vec.push_back(std::make_pair("compilations", 521));
  vec.push_back(std::make_pair("roles", 38033));
  vec.push_back(std::make_pair("sources", 39031));
  vec.push_back(std::make_pair("playlists", 136));

  if (hasMovies)
  {
    if (CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(CSettings::SETTING_MYVIDEOS_FLATTEN))
      vec.push_back(std::make_pair("movies/titles", 342));
    else
      vec.push_back(std::make_pair("movies", 342));   // Movies
  }
  if (hasTvShows)
  {
    if (CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(CSettings::SETTING_MYVIDEOS_FLATTEN))
      vec.push_back(std::make_pair("tvshows/titles", 20343));
    else
      vec.push_back(std::make_pair("tvshows", 20343)); // TV Shows
  }
  if (hasMusicVideos)
  {
    if (CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(CSettings::SETTING_MYVIDEOS_FLATTEN))
      vec.push_back(std::make_pair("musicvideos/titles", 20389));
    else
      vec.push_back(std::make_pair("musicvideos", 20389)); // Music Videos
  }
  {
    if (hasMovies)
      vec.push_back(std::make_pair("recentlyaddedmovies", 20386));  // Recently Added Movies
    if (hasTvShows)
    {
      vec.push_back(std::make_pair("recentlyaddedepisodes", 20387)); // Recently Added Episodes
      vec.push_back(std::make_pair("inprogresstvshows", 626)); // InProgress TvShows
    }
    if (hasMusicVideos)
      vec.push_back(std::make_pair("recentlyaddedmusicvideos", 20390)); // Recently Added Music Videos
  }

  std::string path = BuildPath();
  for (unsigned int i = 0; i < vec.size(); ++i)
  {
    if (i == 3 && !hasSingles)
      continue;
    if (i == 9 && !hasCompilations)
      continue;

    CFileItemPtr pItem(new CFileItem(path + vec[i].first + "/", true));
    pItem->SetLabel(g_localizeStrings.Get(vec[i].second));
    pItem->SetLabelPreformatted(true);
    pItem->SetCanQueue(false);
    items.Add(pItem);
  }

  return true;
}
