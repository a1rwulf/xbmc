#pragma once
/*
 *      Copyright (C) 2017 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <map>
#include <memory>
#include <string>
#include <threads/Thread.h>

class CVideoInfoTag;
class CStreamDetails;
class CFileItem;

template <typename T>
class CVideoDatabaseCacheItem
{
public:
  CVideoDatabaseCacheItem()
  {
    m_getDetails = 0;
    m_updatedAt = 0;
  }
  
  int m_getDetails;
  uint64_t m_updatedAt;
  std::shared_ptr<T> m_item;
};

typedef std::map<long, CVideoDatabaseCacheItem<CVideoInfoTag> > tVideoInfoTagCacheMap;
typedef std::map<long, CVideoDatabaseCacheItem<CStreamDetails> > tStreamDetailsCacheMap;
typedef std::map<long, CVideoDatabaseCacheItem<CFileItem> > tFileItemCacheMap;
typedef std::map<std::string, std::string> tArtTypeCacheType;
typedef std::map<long, CVideoDatabaseCacheItem<tArtTypeCacheType> > tArtCacheMap;
typedef std::map<std::string, tArtCacheMap> tArtTypeCacheMap;

class CVideoDatabaseCache
{
public:
  CVideoDatabaseCache();
  virtual ~CVideoDatabaseCache();
  
  void languageChange();
  
  void clearCacheByFileID(long id);

  void addMovie(long id, std::shared_ptr<CVideoInfoTag>& item, int getDetails, uint64_t updatedAt);
  std::shared_ptr<CVideoInfoTag> getMovie(long id, int getDetails, uint64_t updatedAt);
  
  void addStreamDetails(long id, std::shared_ptr<CStreamDetails>& item);
  std::shared_ptr<CStreamDetails> getStreamDetails(long id);
  
  void addArtMap(long id, std::shared_ptr<tArtTypeCacheType>& item, std::string type);
  std::shared_ptr<tArtTypeCacheType> getArtMap(long id, std::string type);
  
  void addPerson(long id, std::shared_ptr<CFileItem>& item);
  std::shared_ptr<CFileItem> getPerson(long id);
  
  void addTVShow(long id, std::shared_ptr<CFileItem>& item, int getDetails, uint64_t updatedAt);
  std::shared_ptr<CFileItem> getTVShow(long id, int getDetails, uint64_t updatedAt);
  
  void addSeason(long id, std::shared_ptr<CFileItem>& item, uint64_t updatedAt);
  std::shared_ptr<CFileItem> getSeason(long id, uint64_t updatedAt);
  
  void addEpisode(long id, std::shared_ptr<CFileItem>& item, uint64_t updatedAt);
  std::shared_ptr<CFileItem> getEpisode(long id, uint64_t updatedAt);
private:
  tVideoInfoTagCacheMap m_MovieCacheMap;
  tStreamDetailsCacheMap m_StreamDetailsCacheMap;
  tArtTypeCacheMap m_ArtCacheMap;
  tFileItemCacheMap m_PersonCacheMap;
  tFileItemCacheMap m_TVShowCacheMap;
  tFileItemCacheMap m_SeasonCacheMap;
  tFileItemCacheMap m_EpisodeCacheMap;
  
  std::string m_language;
  CCriticalSection m_mutex;
};
