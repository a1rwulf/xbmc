/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "MediaDirectory.h"
#include "utils/URIUtils.h"
#include "MediaDirectory/QueryParams.h"
#include "music/MusicDatabase.h"
#include "filesystem/File.h"
#include "FileItem.h"
#include "utils/Crc32.h"
#include "ServiceBroker.h"
#include "guilib/TextureManager.h"
#include "guilib/LocalizeStrings.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"
#include "utils/LegacyPathTranslation.h"
#include "utils/StringUtils.h"
#include "video/VideoDatabase.h"

using namespace XFILE;
using namespace MEDIADIRECTORY;

CMediaDirectory::CMediaDirectory(void) = default;

CMediaDirectory::~CMediaDirectory(void) = default;

bool CMediaDirectory::GetDirectory(const CURL& url, CFileItemList &items)
{
  std::string path = CLegacyPathTranslation::TranslateMusicDbPath(url);
  items.SetPath(path);
  items.m_dwSize = -1;  // No size

  std::unique_ptr<CDirectoryNode> pNode(CDirectoryNode::ParseURL(path));

  if (!pNode.get())
    return false;

  bool bResult = pNode->GetChilds(items);
  for (int i=0;i<items.Size();++i)
  {
    CFileItemPtr item = items[i];
    if (item->m_bIsFolder && !item->HasIcon() && !item->HasArt("thumb"))
    {
      std::string strImage = GetIcon(item->GetPath());
      if (!strImage.empty() && CServiceBroker::GetGUI()->GetTextureManager().HasTexture(strImage))
        item->SetIconImage(strImage);
    }
  }
  items.SetLabel(pNode->GetLocalizedName());

  return bResult;
}

NODE_TYPE CMediaDirectory::GetDirectoryChildType(const std::string& strPath)
{
  std::string path = CLegacyPathTranslation::TranslateMusicDbPath(strPath);
  std::unique_ptr<CDirectoryNode> pNode(CDirectoryNode::ParseURL(path));

  if (!pNode.get())
    return NODE_TYPE_NONE;

  return pNode->GetChildType();
}

NODE_TYPE CMediaDirectory::GetDirectoryType(const std::string& strPath)
{
  std::string path = CLegacyPathTranslation::TranslateMusicDbPath(strPath);
  std::unique_ptr<CDirectoryNode> pNode(CDirectoryNode::ParseURL(path));

  if (!pNode.get())
    return NODE_TYPE_NONE;

  return pNode->GetType();
}

NODE_TYPE CMediaDirectory::GetDirectoryParentType(const std::string& strPath)
{
  std::string path = CLegacyPathTranslation::TranslateMusicDbPath(strPath);
  std::unique_ptr<CDirectoryNode> pNode(CDirectoryNode::ParseURL(path));

  if (!pNode.get())
    return NODE_TYPE_NONE;

  CDirectoryNode* pParentNode=pNode->GetParent();

  if (!pParentNode)
    return NODE_TYPE_NONE;

  return pParentNode->GetChildType();
}

bool CMediaDirectory::IsArtistDir(const std::string& strDirectory)
{
  NODE_TYPE node=GetDirectoryType(strDirectory);
  return (node==NODE_TYPE_ARTIST);
}

void CMediaDirectory::ClearDirectoryCache(const std::string& strDirectory)
{
  std::string path = CLegacyPathTranslation::TranslateMusicDbPath(strDirectory);
  URIUtils::RemoveSlashAtEnd(path);

  uint32_t crc = Crc32::ComputeFromLowerCase(path);

  std::string strFileName = StringUtils::Format("special://temp/archive_cache/%08x.fi", crc);
  CFile::Delete(strFileName);
}

bool CMediaDirectory::IsAllItem(const std::string& strDirectory)
{
  //Last query parameter, ignoring any appended options, is -1
  CURL url(strDirectory);
  if (StringUtils::EndsWith(url.GetWithoutOptions(), "/-1/"))
    return true;
  return false;
}

bool CMediaDirectory::GetLabel(const std::string& strDirectory, std::string& strLabel)
{
  strLabel = "";

  std::string path = CLegacyPathTranslation::TranslateMusicDbPath(strDirectory);
  std::unique_ptr<CDirectoryNode> pNode(CDirectoryNode::ParseURL(path));
  if (!pNode.get())
    return false;

  // first see if there's any filter criteria
  CQueryParams params;
  CDirectoryNode::GetDatabaseInfo(path, params);

  //! @todo a1rwulf - At some point we should remove musicdatabase/videodatabase
  // and work with one common interface for metadata.
  // This should also try to get labels from other providers depending
  // on the used protocol
  CMusicDatabase musicdatabase;
  if (!musicdatabase.Open())
    return false;

  // get genre
  if (params.GetGenreId() >= 0)
    strLabel += musicdatabase.GetGenreById(params.GetGenreId());

  // get artist
  if (params.GetArtistId() >= 0)
  {
    if (!strLabel.empty())
      strLabel += " / ";
    strLabel += musicdatabase.GetArtistById(params.GetArtistId());
  }

  // get album
  if (params.GetAlbumId() >= 0)
  {
    if (!strLabel.empty())
      strLabel += " / ";
    strLabel += musicdatabase.GetAlbumById(params.GetAlbumId());
  }

  CVideoDatabase videodatabase;
  if (!videodatabase.Open())
    return false;

  // get genre
  if (params.GetGenreId() != -1)
    strLabel += videodatabase.GetGenreById(params.GetGenreId());

  // get country
  if (params.GetCountryId() != -1)
    strLabel += videodatabase.GetCountryById(params.GetCountryId());

  // get set
  if (params.GetSetId() != -1)
    strLabel += videodatabase.GetSetById(params.GetSetId());

  // get tag
  if (params.GetTagId() != -1)
    strLabel += videodatabase.GetTagById(params.GetTagId());

  // get year
  if (params.GetYear() != -1)
  {
    std::string strTemp = StringUtils::Format("%li",params.GetYear());
    if (!strLabel.empty())
      strLabel += " / ";
    strLabel += strTemp;
  }

  if (strLabel.empty())
  {
    switch (pNode->GetChildType())
    {
    case NODE_TYPE_TOP100:
      strLabel = g_localizeStrings.Get(271);
      break;
    case NODE_TYPE_GENRE:
      strLabel = g_localizeStrings.Get(135);
      break;
    case NODE_TYPE_SOURCE:
      strLabel = g_localizeStrings.Get(39030);
      break;
    case NODE_TYPE_ROLE:
      strLabel = g_localizeStrings.Get(38033);
      break;
    case NODE_TYPE_ARTIST:
      strLabel = g_localizeStrings.Get(133);
      break;
    case NODE_TYPE_ALBUM:
      strLabel = g_localizeStrings.Get(132);
      break;
    case NODE_TYPE_ALBUM_RECENTLY_ADDED:
    case NODE_TYPE_ALBUM_RECENTLY_ADDED_SONGS:
      strLabel = g_localizeStrings.Get(359);
      break;
    case NODE_TYPE_ALBUM_RECENTLY_PLAYED:
    case NODE_TYPE_ALBUM_RECENTLY_PLAYED_SONGS:
      strLabel = g_localizeStrings.Get(517);
      break;
    case NODE_TYPE_ALBUM_TOP100:
    case NODE_TYPE_ALBUM_TOP100_SONGS:
      strLabel = g_localizeStrings.Get(10505);
      break;
    case NODE_TYPE_SINGLES:
      strLabel = g_localizeStrings.Get(1050);
      break;
    case NODE_TYPE_SONG:
      strLabel = g_localizeStrings.Get(134);
      break;
    case NODE_TYPE_SONG_TOP100:
      strLabel = g_localizeStrings.Get(10504);
      break;
    case NODE_TYPE_YEAR:
    case NODE_TYPE_YEAR_ALBUM:
    case NODE_TYPE_YEAR_SONG:
      strLabel = g_localizeStrings.Get(652);
      break;
    case NODE_TYPE_ALBUM_COMPILATIONS:
    case NODE_TYPE_ALBUM_COMPILATIONS_SONGS:
      strLabel = g_localizeStrings.Get(521);
      break;
    case NODE_TYPE_OVERVIEW:
      strLabel = "";
      break;
    case NODE_TYPE_PLAYLIST:
      strLabel = g_localizeStrings.Get(136);
      break;
    case NODE_TYPE_TITLE_MOVIES:
    case NODE_TYPE_TITLE_TVSHOWS:
    case NODE_TYPE_TITLE_MUSICVIDEOS:
      strLabel = g_localizeStrings.Get(369); break;
    case NODE_TYPE_ACTOR:
      strLabel = g_localizeStrings.Get(344); break;
    case NODE_TYPE_COUNTRY:
      strLabel = g_localizeStrings.Get(20451); break;
    case NODE_TYPE_DIRECTOR:
      strLabel = g_localizeStrings.Get(20348); break;
    case NODE_TYPE_SETS:
      strLabel = g_localizeStrings.Get(20434); break;
    case NODE_TYPE_TAGS:
      strLabel = g_localizeStrings.Get(20459); break;
    case NODE_TYPE_MOVIES_OVERVIEW:
      strLabel = g_localizeStrings.Get(342); break;
    case NODE_TYPE_TVSHOWS_OVERVIEW:
      strLabel = g_localizeStrings.Get(20343); break;
    case NODE_TYPE_RECENTLY_ADDED_MOVIES:
      strLabel = g_localizeStrings.Get(20386); break;
    case NODE_TYPE_RECENTLY_ADDED_EPISODES:
      strLabel = g_localizeStrings.Get(20387); break;
    case NODE_TYPE_STUDIO:
      strLabel = g_localizeStrings.Get(20388); break;
    case NODE_TYPE_MUSICVIDEOS_OVERVIEW:
      strLabel = g_localizeStrings.Get(20389); break;
    case NODE_TYPE_RECENTLY_ADDED_MUSICVIDEOS:
      strLabel = g_localizeStrings.Get(20390); break;
    case NODE_TYPE_SEASONS:
      strLabel = g_localizeStrings.Get(33054); break;
    case NODE_TYPE_EPISODES:
      strLabel = g_localizeStrings.Get(20360); break;
    case NODE_TYPE_INPROGRESS_TVSHOWS:
      strLabel = g_localizeStrings.Get(626); break;
    default:
      return false;
    }
  }

  return true;
}

bool CMediaDirectory::ContainsSongs(const std::string &path)
{
  MEDIADIRECTORY::NODE_TYPE type = GetDirectoryChildType(path);
  if (type == MEDIADIRECTORY::NODE_TYPE_SONG) return true;
  if (type == MEDIADIRECTORY::NODE_TYPE_SINGLES) return true;
  if (type == MEDIADIRECTORY::NODE_TYPE_ALBUM_RECENTLY_ADDED_SONGS) return true;
  if (type == MEDIADIRECTORY::NODE_TYPE_ALBUM_RECENTLY_PLAYED_SONGS) return true;
  if (type == MEDIADIRECTORY::NODE_TYPE_ALBUM_COMPILATIONS_SONGS) return true;
  if (type == MEDIADIRECTORY::NODE_TYPE_ALBUM_TOP100_SONGS) return true;
  if (type == MEDIADIRECTORY::NODE_TYPE_SONG_TOP100) return true;
  if (type == MEDIADIRECTORY::NODE_TYPE_YEAR_SONG) return true;
  return false;
}

bool CMediaDirectory::Exists(const CURL& url)
{
  std::string path = CLegacyPathTranslation::TranslateMusicDbPath(url);
  std::unique_ptr<CDirectoryNode> pNode(CDirectoryNode::ParseURL(path));

  if (!pNode.get())
    return false;

  if (pNode->GetChildType() == MEDIADIRECTORY::NODE_TYPE_NONE)
    return false;

  return true;
}

bool CMediaDirectory::CanCache(const std::string& strPath)
{
  std::string path = CLegacyPathTranslation::TranslateMusicDbPath(strPath);
  std::unique_ptr<CDirectoryNode> pNode(CDirectoryNode::ParseURL(path));
  if (!pNode.get())
    return false;
  return pNode->CanCache();
}

std::string CMediaDirectory::GetIcon(const std::string &strDirectory)
{
  std::string path = CLegacyPathTranslation::TranslateVideoDbPath(strDirectory);

  switch (GetDirectoryChildType(strDirectory))
  {
  case NODE_TYPE_ARTIST:
      return "DefaultMusicArtists.png";
  case NODE_TYPE_MUSICGENRE:
      return "DefaultMusicGenres.png";
  case NODE_TYPE_SOURCE:
    return "DefaultMusicSources.png";
  case NODE_TYPE_ROLE:
    return "DefaultMusicRoles.png";
  case NODE_TYPE_TOP100:
      return "DefaultMusicTop100.png";
  case NODE_TYPE_ALBUM:
  case NODE_TYPE_YEAR_ALBUM:
    return "DefaultMusicAlbums.png";
  case NODE_TYPE_ALBUM_RECENTLY_ADDED:
  case NODE_TYPE_ALBUM_RECENTLY_ADDED_SONGS:
    return "DefaultMusicRecentlyAdded.png";
  case NODE_TYPE_ALBUM_RECENTLY_PLAYED:
  case NODE_TYPE_ALBUM_RECENTLY_PLAYED_SONGS:
    return "DefaultMusicRecentlyPlayed.png";
  case NODE_TYPE_SINGLES:
  case NODE_TYPE_SONG:
  case NODE_TYPE_YEAR_SONG:
  case NODE_TYPE_ALBUM_COMPILATIONS_SONGS:
    return "DefaultMusicSongs.png";
  case NODE_TYPE_ALBUM_TOP100:
  case NODE_TYPE_ALBUM_TOP100_SONGS:
    return "DefaultMusicTop100Albums.png";
  case NODE_TYPE_SONG_TOP100:
    return "DefaultMusicTop100Songs.png";
  case NODE_TYPE_YEAR:
    return "DefaultMusicYears.png";
  case NODE_TYPE_ALBUM_COMPILATIONS:
    return "DefaultMusicCompilations.png";
  case NODE_TYPE_PLAYLIST:
    return "DefaultPlaylist.png";
  case NODE_TYPE_TITLE_MOVIES:
    if (URIUtils::PathEquals(path, "videodb://movies/titles/"))
    {
      if (CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(CSettings::SETTING_MYVIDEOS_FLATTEN))
        return "DefaultMovies.png";
      return "DefaultMovieTitle.png";
    }
    return "";
  case NODE_TYPE_TITLE_TVSHOWS:
    if (URIUtils::PathEquals(path, "videodb://tvshows/titles/"))
    {
      if (CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(CSettings::SETTING_MYVIDEOS_FLATTEN))
        return "DefaultTVShows.png";
      return "DefaultTVShowTitle.png";
    }
    return "";
  case NODE_TYPE_TITLE_MUSICVIDEOS:
    if (URIUtils::PathEquals(path, "videodb://musicvideos/titles/"))
    {
      if (CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(CSettings::SETTING_MYVIDEOS_FLATTEN))
        return "DefaultMusicVideos.png";
      return "DefaultMusicVideoTitle.png";
    }
    return "";
  case NODE_TYPE_ACTOR: // Actor
    return "DefaultActor.png";
  case NODE_TYPE_VIDEOGENRE: // Genres
    return "DefaultGenre.png";
  case NODE_TYPE_GENRE:
    return "DefaultGenre.png";
  case NODE_TYPE_COUNTRY: // Countries
    return "DefaultCountry.png";
  case NODE_TYPE_SETS: // Sets
    return "DefaultSets.png";
  case NODE_TYPE_TAGS: // Tags
    return "DefaultTags.png";
  case NODE_TYPE_DIRECTOR: // Director
    return "DefaultDirector.png";
  case NODE_TYPE_MOVIES_OVERVIEW: // Movies
    return "DefaultMovies.png";
  case NODE_TYPE_TVSHOWS_OVERVIEW: // TV Shows
    return "DefaultTVShows.png";
  case NODE_TYPE_RECENTLY_ADDED_MOVIES: // Recently Added Movies
    return "DefaultRecentlyAddedMovies.png";
  case NODE_TYPE_RECENTLY_ADDED_EPISODES: // Recently Added Episodes
    return "DefaultRecentlyAddedEpisodes.png";
  case NODE_TYPE_RECENTLY_ADDED_MUSICVIDEOS: // Recently Added Episodes
    return "DefaultRecentlyAddedMusicVideos.png";
  case NODE_TYPE_INPROGRESS_TVSHOWS: // InProgress TvShows
    return "DefaultInProgressShows.png";
  case NODE_TYPE_STUDIO: // Studios
    return "DefaultStudios.png";
  case NODE_TYPE_MUSICVIDEOS_OVERVIEW: // Music Videos
    return "DefaultMusicVideos.png";
  case NODE_TYPE_MUSICVIDEOS_ALBUM: // Music Videos - Albums
    return "DefaultMusicAlbums.png";
  default:
    break;
  }

  return "";
}

bool CMediaDirectory::GetQueryParams(const std::string& strPath, CQueryParams& params)
{
  std::string path = CLegacyPathTranslation::TranslateVideoDbPath(strPath);
  std::unique_ptr<CDirectoryNode> pNode(CDirectoryNode::ParseURL(path));

  if (!pNode.get())
    return false;

  CDirectoryNode::GetDatabaseInfo(strPath,params);
  return true;
}
