/*
 *      Copyright (C) 2017 Team Kodi
 *      http://kodi.tv
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
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "VideoDatabaseCache.h"
#include "VideoInfoTag.h"
#include "utils/StreamDetails.h"
#include "FileItem.h"
#include "VideoDatabase.h"
#include "settings/Settings.h"
#include <ServiceBroker.h>

CVideoDatabaseCache::CVideoDatabaseCache()
{
  m_language = "resource.language.en_gb";
}

CVideoDatabaseCache::~CVideoDatabaseCache()
{

}

void CVideoDatabaseCache::setCurrentLanguage()
{
  m_language = CServiceBroker::GetSettings().GetString(CSettings::SETTING_LOCALE_LANGUAGE);
}

void CVideoDatabaseCache::languageChange()
{
  if (m_language == CServiceBroker::GetSettings().GetString(CSettings::SETTING_LOCALE_LANGUAGE))
    return;
  
  m_language = CServiceBroker::GetSettings().GetString(CSettings::SETTING_LOCALE_LANGUAGE);
  CVideoDatabase videodb;
  
  CSingleLock lock(m_mutex);
  
  for (tVideoInfoTagCacheMap::iterator iter = m_MovieCacheMap.begin(); iter != m_MovieCacheMap.end(); ++iter)
  {
    videodb.GetMovieTranslation(iter->second.m_item.get(), true);
  }
  
  for (tFileItemCacheMap::iterator iter = m_TVShowCacheMap.begin(); iter != m_TVShowCacheMap.end(); ++iter)
  {
    videodb.GetTVShowTranslation(iter->second.m_item->GetVideoInfoTag(), true);
    
    //Set the item label again after translation
    iter->second.m_item->SetLabel(iter->second.m_item->GetVideoInfoTag()->m_strTitle);
    
    iter->second.m_item->SetProperty("castandrole", iter->second.m_item->GetVideoInfoTag()->GetCast(true));
  }
  
  for (tFileItemCacheMap::iterator iter = m_SeasonCacheMap.begin(); iter != m_SeasonCacheMap.end(); ++iter)
  {
    videodb.GetSeasonTranslation(iter->second.m_item->GetVideoInfoTag(), true);
    
    //Set the item label again after translation
    iter->second.m_item->SetLabel(iter->second.m_item->GetVideoInfoTag()->m_strTitle);
    
    iter->second.m_item->SetProperty("castandrole", iter->second.m_item->GetVideoInfoTag()->GetCast(true));
  }
  
  for (tFileItemCacheMap::iterator iter = m_EpisodeCacheMap.begin(); iter != m_EpisodeCacheMap.end(); ++iter)
  {
    videodb.GetEpisodeTranslation(iter->second.m_item->GetVideoInfoTag(), true);
    
    //Set the item label again after translation
    iter->second.m_item->SetLabel(iter->second.m_item->GetVideoInfoTag()->m_strTitle);
    
    iter->second.m_item->SetProperty("castandrole", iter->second.m_item->GetVideoInfoTag()->GetCast(true));
  }
}

void CVideoDatabaseCache::clearCacheByFileID(long id)
{
  CSingleLock lock(m_mutex);
  
  for (tVideoInfoTagCacheMap::iterator iter = m_MovieCacheMap.begin(); iter != m_MovieCacheMap.end();)
  {
    if (iter->second.m_item->m_iFileId == id)
    {
      iter = m_MovieCacheMap.erase(iter);
      continue;
    }
    
    ++iter;
  }
  
  for (tFileItemCacheMap::iterator iter = m_TVShowCacheMap.begin(); iter != m_TVShowCacheMap.end();)
  {
    if (iter->second.m_item->GetVideoInfoTag()->m_iFileId == id)
    {
      iter = m_TVShowCacheMap.erase(iter);
      continue;
    }
    
    ++iter;
  }
  
  for (tFileItemCacheMap::iterator iter = m_SeasonCacheMap.begin(); iter != m_SeasonCacheMap.end();)
  {
    if (iter->second.m_item->GetVideoInfoTag()->m_iFileId == id)
    {
      iter = m_SeasonCacheMap.erase(iter);
      continue;
    }
    
    ++iter;
  }
  
  for (tFileItemCacheMap::iterator iter = m_EpisodeCacheMap.begin(); iter != m_EpisodeCacheMap.end();)
  {
    if (iter->second.m_item->GetVideoInfoTag()->m_iFileId == id)
    {
      iter = m_EpisodeCacheMap.erase(iter);
      continue;
    }
    
    ++iter;
  }
}

void CVideoDatabaseCache::addMovie(long id, std::shared_ptr<CVideoInfoTag>& item, int getDetails, uint64_t updatedAt)
{
  CVideoDatabaseCacheItem<CVideoInfoTag> cacheItem;
  cacheItem.m_getDetails = getDetails;
  cacheItem.m_updatedAt = updatedAt;
  cacheItem.m_item = item;
  
  CSingleLock lock(m_mutex);
  m_MovieCacheMap.insert(std::make_pair(id, cacheItem));
  setCurrentLanguage();
}

std::shared_ptr<CVideoInfoTag> CVideoDatabaseCache::getMovie(long id, int getDetails, uint64_t updatedAt)
{
  CSingleLock lock(m_mutex);
  tVideoInfoTagCacheMap ::iterator it = m_MovieCacheMap.find(id);

  if (it != m_MovieCacheMap.end())
  {
    // If we do not have enough details, delete the item
    if (it->second.m_getDetails < getDetails || it->second.m_updatedAt != updatedAt)
    {
      m_MovieCacheMap.erase(it);
      return nullptr;
    }
    
    return it->second.m_item;
  }

  return nullptr;
}

void CVideoDatabaseCache::addStreamDetails(long id, std::shared_ptr<CStreamDetails>& item)
{
  CVideoDatabaseCacheItem<CStreamDetails> cacheItem;
  cacheItem.m_getDetails = 0;
  cacheItem.m_item = item;
  
  CSingleLock lock(m_mutex);
  m_StreamDetailsCacheMap.insert(std::make_pair(id, cacheItem));
  setCurrentLanguage();
}

std::shared_ptr<CStreamDetails> CVideoDatabaseCache::getStreamDetails(long id)
{
  CSingleLock lock(m_mutex);
  tStreamDetailsCacheMap ::iterator it = m_StreamDetailsCacheMap.find(id);
  
  if (it != m_StreamDetailsCacheMap.end())
  {
    // If we do not have enough details, delete the item
    if (it->second.m_getDetails < 0)
    {
      m_StreamDetailsCacheMap.erase(it);
      return nullptr;
    }
    
    return it->second.m_item;
  }
  
  return nullptr;
}

void CVideoDatabaseCache::addArtMap(long id, std::shared_ptr<tArtTypeCacheType>& item, std::string type)
{
  CVideoDatabaseCacheItem<tArtTypeCacheType> cacheItem;
  cacheItem.m_getDetails = 0;
  cacheItem.m_item = item;
  
  CSingleLock lock(m_mutex);
  m_ArtCacheMap[type].insert(std::make_pair(id, cacheItem));
  setCurrentLanguage();
}

std::shared_ptr<tArtTypeCacheType> CVideoDatabaseCache::getArtMap(long id, std::string type)
{
  CSingleLock lock(m_mutex);
  tArtTypeCacheMap ::iterator it = m_ArtCacheMap.find(type);
  
  if (it != m_ArtCacheMap.end())
  {
    tArtCacheMap ::iterator artIt = it->second.find(id);
    
    if (artIt != it->second.end())
    {
      return artIt->second.m_item;
    }
  }
  
  return nullptr;
}

void CVideoDatabaseCache::addPerson(long id, std::shared_ptr<CFileItem>& item)
{
  CVideoDatabaseCacheItem<CFileItem> cacheItem;
  cacheItem.m_getDetails = 0;
  cacheItem.m_item = item;
  
  CSingleLock lock(m_mutex);
  m_PersonCacheMap.insert(std::make_pair(id, cacheItem));
  setCurrentLanguage();
}

std::shared_ptr<CFileItem> CVideoDatabaseCache::getPerson(long id)
{
  CSingleLock lock(m_mutex);
  tFileItemCacheMap ::iterator it = m_PersonCacheMap.find(id);
  
  if (it != m_PersonCacheMap.end())
  {
    return it->second.m_item;
  }
  
  return nullptr;
}

void CVideoDatabaseCache::addTVShow(long id, std::shared_ptr<CFileItem>& item, int getDetails, uint64_t updatedAt)
{
  CVideoDatabaseCacheItem<CFileItem> cacheItem;
  cacheItem.m_getDetails = getDetails;
  cacheItem.m_updatedAt = updatedAt;
  cacheItem.m_item = item;
  
  CSingleLock lock(m_mutex);
  m_TVShowCacheMap.insert(std::make_pair(id, cacheItem));
  setCurrentLanguage();
}

std::shared_ptr<CFileItem> CVideoDatabaseCache::getTVShow(long id, int getDetails, uint64_t updatedAt)
{
  CSingleLock lock(m_mutex);
  tFileItemCacheMap ::iterator it = m_TVShowCacheMap.find(id);
  
  if (it != m_TVShowCacheMap.end())
  {
    // If we do not have enough details, delete the item
    if (it->second.m_getDetails < getDetails || it->second.m_updatedAt != updatedAt)
    {
      m_TVShowCacheMap.erase(it);
      return nullptr;
    }
    
    return it->second.m_item;
  }
  
  return nullptr;
}

void CVideoDatabaseCache::addSeason(long id, std::shared_ptr<CFileItem>& item, uint64_t updatedAt)
{
  CVideoDatabaseCacheItem<CFileItem> cacheItem;
  cacheItem.m_getDetails = 0;
  cacheItem.m_updatedAt = updatedAt;
  cacheItem.m_item = item;
  
  CSingleLock lock(m_mutex);
  m_SeasonCacheMap.insert(std::make_pair(id, cacheItem));
  setCurrentLanguage();
}

std::shared_ptr<CFileItem> CVideoDatabaseCache::getSeason(long id, uint64_t updatedAt)
{
  CSingleLock lock(m_mutex);
  tFileItemCacheMap ::iterator it = m_SeasonCacheMap.find(id);
  
  if (it != m_SeasonCacheMap.end())
  {
    if (it->second.m_updatedAt != updatedAt)
    {
      m_SeasonCacheMap.erase(it);
      return nullptr;
    }
    
    return it->second.m_item;
  }
  
  return nullptr;
}

void CVideoDatabaseCache::addEpisode(long id, std::shared_ptr<CFileItem>& item, uint64_t updatedAt)
{
  CVideoDatabaseCacheItem<CFileItem> cacheItem;
  cacheItem.m_getDetails = 0;
  cacheItem.m_updatedAt = updatedAt;
  cacheItem.m_item = item;
  
  CSingleLock lock(m_mutex);
  m_EpisodeCacheMap.insert(std::make_pair(id, cacheItem));
  setCurrentLanguage();
}

std::shared_ptr<CFileItem> CVideoDatabaseCache::getEpisode(long id, uint64_t updatedAt)
{
  CSingleLock lock(m_mutex);
  tFileItemCacheMap ::iterator it = m_EpisodeCacheMap.find(id);
  
  if (it != m_EpisodeCacheMap.end())
  {
    if (it->second.m_updatedAt != updatedAt)
    {
      m_EpisodeCacheMap.erase(it);
      return nullptr;
    }
    
    return it->second.m_item;
  }
  
  return nullptr;
}
