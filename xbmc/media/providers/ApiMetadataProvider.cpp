/*
 *  Copyright (C) 2012-2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "ApiMetadataProvider.h"

#include "URL.h"
#include "dbwrappers/Database.h"
#include "filesystem/CurlFile.h"
#include "music/MusicDatabase.h"
#include "rapidjson/document.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

using namespace METADATA;

CApiMetadataProvider::CApiMetadataProvider(std::string baseUrl)
    : IMetadataProvider()
{
  m_supportedEntities = SupportedEntities::Nothing;
  m_supportedEntities |= SupportedEntities::Playlist;
  m_baseUrl = baseUrl;
}

bool CApiMetadataProvider::GetPlaylists(const std::string& strBaseDir,
                                        CFileItemList& items,
                                        const CDatabase::Filter& filter,
                                        const SortDescription& sortDescription,
                                        bool countOnly)
{
  CLog::Log(LOGNOTICE, "CApiMetadataProvider::%s - GetPlaylists", __FUNCTION__);
  try
  {
    int total = 0;
    CURL url;
    url.Parse(strBaseDir);

    std::string requestUrl = GetRequestUrl(url);

    XFILE::CCurlFile webrequest;
    std::string response;
    webrequest.Get(m_baseUrl + requestUrl, response);

    rapidjson::Document doc;
    doc.Parse(response.c_str());

    if (response.empty())
      return false;

    for (auto& playlist : doc["data"]["items"].GetArray())
    {
      CMusicPlaylist pl;

      if (playlist["id"].IsString())
        pl.uuidPlaylist = playlist["id"].GetString();
      else if (playlist["id"].IsInt())
        pl.idPlaylist = playlist["id"].GetInt();

      pl.strPlaylist = playlist["label"].GetString();

      CMusicDbUrl itemUrl;
      std::string path = StringUtils::Format(
          "oam://playlists/{}/",
          !pl.uuidPlaylist.empty() ? pl.uuidPlaylist : std::to_string(pl.idPlaylist));
      itemUrl.FromString(path);

      CFileItemPtr item(new CFileItem(itemUrl.ToString(), pl));
      item->m_dwSize = doc["data"]["items"].GetArray().Size();
      std::string iconPath = playlist["thumbnail"].IsString() ? playlist["thumbnail"].GetString()
                                                              : "DefaultMusicPlaylists.png";

      item->SetArt("thumb", iconPath);
      item->SetIconImage(iconPath);

      item->SetProvider(playlist["provider"].GetString());
      item->SetProperty("provider", playlist["provider"].GetString());

      items.Add(item);
      total++;
    }

    return true;
  }
  catch (std::exception& e)
  {
    CLog::Log(LOGERROR, "%s (%s) exception - %s", __FUNCTION__, filter.where.c_str(), e.what());
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, filter.where.c_str());
  }
  return false;
}

bool CApiMetadataProvider::GetSongs(const std::string& strBaseDir,
                                    CFileItemList& items,
                                    int idGenre,
                                    int idArtist,
                                    int idAlbum,
                                    int idPlaylist,
                                    const SortDescription& sortDescription)
{
  CLog::Log(LOGNOTICE, "CApiMetadataProvider::%s - GetPlaylists", __FUNCTION__);
  try
  {
    int total = 0;
    CURL url;
    url.Parse(strBaseDir);

    std::string requestUrl = GetRequestUrl(url);

    XFILE::CCurlFile webrequest;
    std::string response;
    webrequest.Get(m_baseUrl + requestUrl, response);

    rapidjson::Document doc;
    doc.Parse(response.c_str());

    if (response.empty())
      return false;

    for (auto& track : doc["data"]["tracks"].GetArray())
    {
      CSong song;
      song.strTitle = track["label"].GetString();
      song.strFileName = track["filepath"].GetString();
      song.iDuration = track["duration"].GetInt();
      song.strAlbum = track["album"].GetString();
      song.strArtistDesc = track["artist"].GetString();
      if (track.HasMember("genre") && track["genre"].IsString())
        song.genre.push_back(track["genre"].GetString());
      if (track.HasMember("genre") && track["genre"].IsObject())
        song.genre.push_back(track["genre"]["name"].GetString());

      // Some providers send a year in songs
      // Others have a releaseDate
      if (track.HasMember("year") && track["year"].IsInt())
        song.iYear = track["year"].GetInt();
      else if (track.HasMember("releaseDate") && track["releaseDate"].IsString())
      {
        CDateTime releaseDate;
        releaseDate.SetFromDateString(track["releaseDate"].GetString());
        song.iYear = releaseDate.GetYear();
      }

      CMusicDbUrl itemUrl;
      std::string path;
      if (track["id"].IsString())
        path = StringUtils::Format("oam://songs/{}/", track["id"].GetString());
      else if (track["id"].IsInt())
        path = StringUtils::Format("oam://songs/{}/", track["id"].GetInt());

      itemUrl.FromString(path);

      CFileItemPtr item(new CFileItem(song));
      item->SetLabel(song.strTitle);
      // item->m_lStartOffset = r.song->m_startOffset;
      // item->SetProperty("item_start", r.song->m_startOffset);
      // item->m_lEndOffset = r.song->m_endOffset;
      std::string iconPath =
          track["thumbnail"].IsString() ? track["thumbnail"].GetString() : "DefaultAlbumCover.png";

      item->SetArt("thumb", iconPath);
      item->SetIconImage(iconPath);
      item->SetProvider(track["provider"].GetString());
      item->SetProperty("provider", track["provider"].GetString());

      // If we get the songs from a playlist, save the playlistid in
      // the fileitem as property.
      // This way we can supply the playlist that's being played in Player.GetItem
      if (StringUtils::StartsWith(url.GetFileName(), "playlist"))
        item->SetProperty("playlistid", StringUtils::Split(url.GetFileNameWithoutPath(), "&")[0]);

      items.Add(item);
      total++;
    }

    return true;
  }
  catch (std::exception& e)
  {
    CLog::Log(LOGERROR, "%s exception - %s", __FUNCTION__, e.what());
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s failed", __FUNCTION__);
  }

  return false;
}

SupportedEntities CApiMetadataProvider::GetSupportedEntities()
{
  return static_cast<SupportedEntities>(m_supportedEntities);
}

std::string CApiMetadataProvider::GetRequestUrl(const CURL& url) const
{
  if ((url.GetProtocol() == "oam" || url.GetProtocol() == "musicdb") &&
      StringUtils::StartsWith(url.GetFileName(), "playlist"))
  {
    if (!StringUtils::EndsWith(url.GetFileName(), "playlists/"))
    {
      std::string uri = "/view/1/playlist?id=" + url.GetFileNameWithoutPath();
      URIUtils::RemoveSlashAtEnd(uri);
      return uri;
    }
    else
    {
      std::string uri = "/view/1/" + url.GetFileName();
      URIUtils::RemoveSlashAtEnd(uri);
      return uri;
    }
  }

  return "/error";
}

struct MemoryStruct
{
  char* memory;
  size_t size;
};
