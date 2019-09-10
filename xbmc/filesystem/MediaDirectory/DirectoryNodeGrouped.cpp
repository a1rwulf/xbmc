/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DirectoryNodeGrouped.h"
#include "QueryParams.h"
#include "music/MusicDatabase.h"
#include "video/VideoDatabase.h"

using namespace XFILE::MEDIADIRECTORY;

CDirectoryNodeGrouped::CDirectoryNodeGrouped(NODE_TYPE type, const std::string& strName, CDirectoryNode* pParent, const std::string& strOrigin)
  : CDirectoryNode(type, strName, pParent, strOrigin)
{ }

NODE_TYPE CDirectoryNodeGrouped::GetChildType() const
{
  CQueryParams params;
  CollectQueryParams(params);

  if (params.GetContentType() == VIDEODB_CONTENT_MOVIES)
    return NODE_TYPE_TITLE_MOVIES;
  if (params.GetContentType() == VIDEODB_CONTENT_MUSICVIDEOS)
  {
    if (GetType() == NODE_TYPE_ACTOR)
      return NODE_TYPE_MUSICVIDEOS_ALBUM;
    else
      return NODE_TYPE_TITLE_MUSICVIDEOS;
  }

  if (params.GetContentType() == VIDEODB_CONTENT_TVSHOWS)
    return NODE_TYPE_TITLE_TVSHOWS;

  if (GetType() == NODE_TYPE_YEAR)
    return NODE_TYPE_YEAR_ALBUM;

  return NODE_TYPE_ARTIST;
}

std::string CDirectoryNodeGrouped::GetLocalizedName() const
{
  CMusicDatabase db;
  if (db.Open())
    return db.GetItemById(GetContentType(), GetID());
  return "";
}

bool CDirectoryNodeGrouped::GetContent(CFileItemList& items) const
{
  //! @todo a1rwulf - At some point we should work with one common interface to get this data
  // and also use the registered providers in metadata manager

  if (GetMediaType() == "music")
  {
    CMusicDatabase musicdatabase;
    if (!musicdatabase.Open())
      return false;

    return musicdatabase.GetItems(BuildPath(), GetContentType(), items);
  }
  else if (GetMediaType() == "video")
  {
    CVideoDatabase videodatabase;
    if (!videodatabase.Open())
      return false;

    CQueryParams params;
    CollectQueryParams(params);

    std::string itemType = GetContentType(params);
    if (itemType.empty())
      return false;

    // make sure to translate all IDs in the path into URL parameters
    CVideoDbUrl videoUrl;
    if (!videoUrl.FromString(BuildPath()))
      return false;

    return videodatabase.GetItems(videoUrl.ToString(), (VIDEODB_CONTENT_TYPE)params.GetContentType(), itemType, items);
  }

  return false;
}

std::string CDirectoryNodeGrouped::GetContentType() const
{
  CQueryParams params;
  CollectQueryParams(params);
  return GetContentType(params);
}

std::string CDirectoryNodeGrouped::GetContentType(const CQueryParams &params) const
{
  switch (GetType())
  {
    case NODE_TYPE_GENRE:
      return "genres";
    case NODE_TYPE_SOURCE:
      return "sources";
    case NODE_TYPE_ROLE:
      return "roles";
    case NODE_TYPE_YEAR:
      return "years";
    case NODE_TYPE_COUNTRY:
      return "countries";
    case NODE_TYPE_SETS:
      return "sets";
    case NODE_TYPE_TAGS:
      return "tags";
    case NODE_TYPE_ACTOR:
      if ((VIDEODB_CONTENT_TYPE)params.GetContentType() == VIDEODB_CONTENT_MUSICVIDEOS)
        return "artists";
      else
        return "actors";
    case NODE_TYPE_DIRECTOR:
      return "directors";
    case NODE_TYPE_STUDIO:
      return "studios";
    case NODE_TYPE_MUSICVIDEOS_ALBUM:
      return "albums";

    case NODE_TYPE_EPISODES:
    case NODE_TYPE_MOVIES_OVERVIEW:
    case NODE_TYPE_MUSICVIDEOS_OVERVIEW:
    case NODE_TYPE_NONE:
    case NODE_TYPE_OVERVIEW:
    case NODE_TYPE_RECENTLY_ADDED_EPISODES:
    case NODE_TYPE_RECENTLY_ADDED_MOVIES:
    case NODE_TYPE_RECENTLY_ADDED_MUSICVIDEOS:
    case NODE_TYPE_INPROGRESS_TVSHOWS:
    case NODE_TYPE_ROOT:
    case NODE_TYPE_SEASONS:
    case NODE_TYPE_TITLE_MOVIES:
    case NODE_TYPE_TITLE_MUSICVIDEOS:
    case NODE_TYPE_TITLE_TVSHOWS:
    case NODE_TYPE_TVSHOWS_OVERVIEW:
    default:
      break;
  }

  return "";
}

std::string CDirectoryNodeGrouped::GetMediaType() const
{
  CQueryParams params;
  CollectQueryParams(params);
  return GetMediaType(params);
}

std::string CDirectoryNodeGrouped::GetMediaType(const CQueryParams &params) const
{
  switch (GetType())
  {
    case NODE_TYPE_NONE:
    case NODE_TYPE_ROOT:
    case NODE_TYPE_OVERVIEW:
    case NODE_TYPE_TOP100:
    case NODE_TYPE_ROLE:
    case NODE_TYPE_SOURCE:
    case NODE_TYPE_GENRE:
    case NODE_TYPE_MUSICGENRE:
    case NODE_TYPE_VIDEOGENRE:
    case NODE_TYPE_ARTIST:
    case NODE_TYPE_ALBUM:
    case NODE_TYPE_ALBUM_RECENTLY_ADDED:
    case NODE_TYPE_ALBUM_RECENTLY_ADDED_SONGS:
    case NODE_TYPE_ALBUM_RECENTLY_PLAYED:
    case NODE_TYPE_ALBUM_RECENTLY_PLAYED_SONGS:
    case NODE_TYPE_ALBUM_TOP100:
    case NODE_TYPE_ALBUM_TOP100_SONGS:
    case NODE_TYPE_ALBUM_COMPILATIONS:
    case NODE_TYPE_ALBUM_COMPILATIONS_SONGS:
    case NODE_TYPE_SONG:
    case NODE_TYPE_SONG_TOP100:
    case NODE_TYPE_YEAR:
    case NODE_TYPE_YEAR_ALBUM:
    case NODE_TYPE_YEAR_SONG:
    case NODE_TYPE_SINGLES:
    case NODE_TYPE_PLAYLIST:
      return "music";
    case NODE_TYPE_MOVIES_OVERVIEW:
    case NODE_TYPE_TVSHOWS_OVERVIEW:
    case NODE_TYPE_ACTOR:
    case NODE_TYPE_TITLE_MOVIES:
    case NODE_TYPE_DIRECTOR:
    case NODE_TYPE_TITLE_TVSHOWS:
    case NODE_TYPE_SEASONS:
    case NODE_TYPE_EPISODES:
    case NODE_TYPE_RECENTLY_ADDED_MOVIES:
    case NODE_TYPE_RECENTLY_ADDED_EPISODES:
    case NODE_TYPE_STUDIO:
    case NODE_TYPE_MUSICVIDEOS_OVERVIEW:
    case NODE_TYPE_RECENTLY_ADDED_MUSICVIDEOS:
    case NODE_TYPE_TITLE_MUSICVIDEOS:
    case NODE_TYPE_MUSICVIDEOS_ALBUM:
    case NODE_TYPE_SETS:
    case NODE_TYPE_COUNTRY:
    case NODE_TYPE_TAGS:
    case NODE_TYPE_INPROGRESS_TVSHOWS:
    default:
      return "video";
  }
}
