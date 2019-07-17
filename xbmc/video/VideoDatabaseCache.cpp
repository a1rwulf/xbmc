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

#include <odb/session.hxx>

#include <odb/odb_gen/ODBTranslation.h>
#include <odb/odb_gen/ODBTranslation_odb.h>

#include "VideoDatabaseCache.h"
#include "VideoInfoTag.h"
#include "utils/StreamDetails.h"
#include "FileItem.h"
#include "VideoDatabase.h"
#include "settings/Settings.h"
#include <ServiceBroker.h>
#include "settings/SettingsComponent.h"
#include "VideoDatabase.h"
#include "guilib/LocalizeStrings.h"
#include "utils/StringUtils.h"
#include "utils/log.h"
#include "../dbwrappers/CommonDatabase.h"

CVideoDatabaseCache::CVideoDatabaseCache()
{
  m_language = "resource.language.en_gb";
  m_tryReload = true;
}

CVideoDatabaseCache::~CVideoDatabaseCache()
{

}

void CVideoDatabaseCache::setCurrentLanguage()
{
  m_language = CServiceBroker::GetSettingsComponent()->GetSettings()->GetString(CSettings::SETTING_LOCALE_LANGUAGE);
}

void CVideoDatabaseCache::loadTranslations()
{
  CSingleLock lock(m_mutex);
  m_TranslationCacheMap.clear();
  
  std::shared_ptr<odb::transaction> odb_transaction (CCommonDatabase::GetInstance().getTransaction());
  typedef odb::query<CODBTranslation> query;
  
  std::string language = m_language.substr(18);
  
  odb::result<CODBTranslation> r(CCommonDatabase::GetInstance().getDB()->query<CODBTranslation>(query::language == language));
  for (CODBTranslation& translation : r)
  {
    CVideoDatabaseTranslationItem item;
    item.m_updatedAt = 0;
    item.m_language = language;
    item.m_text = translation.m_text;
    
    m_TranslationCacheMap.insert(std::make_pair(translation.m_key, item));
  }
}

void CVideoDatabaseCache::languageChange()
{
  m_language = CServiceBroker::GetSettingsComponent()->GetSettings()->GetString(CSettings::SETTING_LOCALE_LANGUAGE);
  
  
  loadTranslations();

  // Clear cached art because we can have different covers per language
  m_ArtCacheMap.clear();

  // Now update movie and tvshow translations
  // Note that tvshow translations, update tvshows, seasons and episodes
  GetMovieTranslations();
  GetTVShowTranslations();
  
  CSingleLock lock(m_mutex);

  for (tVideoInfoTagCacheMap::iterator iter = m_SeasonCacheMap.begin(); iter != m_SeasonCacheMap.end(); ++iter)
  {
    iter->second.m_item->SetTitle(iter->second.m_item->m_strTitle);
  }

  for (tVideoInfoTagCacheMap::iterator iter = m_EpisodeCacheMap.begin(); iter != m_EpisodeCacheMap.end(); ++iter)
  {
    iter->second.m_item->SetTitle(iter->second.m_item->m_strTitle);
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
  
  for (tVideoInfoTagCacheMap::iterator iter = m_TVShowCacheMap.begin(); iter != m_TVShowCacheMap.end();)
  {
    if (iter->second.m_item->m_iFileId == id)
    {
      iter = m_TVShowCacheMap.erase(iter);
      continue;
    }
    
    ++iter;
  }
  
  for (tVideoInfoTagCacheMap::iterator iter = m_SeasonCacheMap.begin(); iter != m_SeasonCacheMap.end();)
  {
    if (iter->second.m_item->m_iFileId == id)
    {
      iter = m_SeasonCacheMap.erase(iter);
      continue;
    }
    
    ++iter;
  }
  
  for (tVideoInfoTagCacheMap::iterator iter = m_EpisodeCacheMap.begin(); iter != m_EpisodeCacheMap.end();)
  {
    if (iter->second.m_item->m_iFileId == id)
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

void CVideoDatabaseCache::addTVShow(long id, std::shared_ptr<CVideoInfoTag>& item, int getDetails, uint64_t updatedAt)
{
  CVideoDatabaseCacheItem<CVideoInfoTag> cacheItem;
  cacheItem.m_getDetails = getDetails;
  cacheItem.m_updatedAt = updatedAt;
  cacheItem.m_item = item;
  
  CSingleLock lock(m_mutex);
  m_TVShowCacheMap.insert(std::make_pair(id, cacheItem));
  setCurrentLanguage();
}

std::shared_ptr<CVideoInfoTag> CVideoDatabaseCache::getTVShow(long id, int getDetails, uint64_t updatedAt)
{
  CSingleLock lock(m_mutex);
  tVideoInfoTagCacheMap ::iterator it = m_TVShowCacheMap.find(id);
  
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

void CVideoDatabaseCache::addSeason(long id, std::shared_ptr<CVideoInfoTag>& item, int getDetails, uint64_t updatedAt)
{
  CVideoDatabaseCacheItem<CVideoInfoTag> cacheItem;
  cacheItem.m_getDetails = getDetails;
  cacheItem.m_updatedAt = updatedAt;
  cacheItem.m_item = item;
  
  CSingleLock lock(m_mutex);
  m_SeasonCacheMap.insert(std::make_pair(id, cacheItem));
  setCurrentLanguage();
}

std::shared_ptr<CVideoInfoTag> CVideoDatabaseCache::getSeason(long id, int getDetails, uint64_t updatedAt)
{
  CSingleLock lock(m_mutex);
  tVideoInfoTagCacheMap ::iterator it = m_SeasonCacheMap.find(id);

  if (it != m_SeasonCacheMap.end())
  {
    if (it->second.m_getDetails < getDetails || it->second.m_updatedAt != updatedAt)
    {
      m_SeasonCacheMap.erase(it);
      return nullptr;
    }

    return it->second.m_item;
  }

  return nullptr;
}

void CVideoDatabaseCache::addEpisode(long id, std::shared_ptr<CVideoInfoTag>& item, int getDetails, uint64_t updatedAt)
{
  CVideoDatabaseCacheItem<CVideoInfoTag> cacheItem;
  cacheItem.m_getDetails = getDetails;
  cacheItem.m_updatedAt = updatedAt;
  cacheItem.m_item = item;
  
  CSingleLock lock(m_mutex);
  m_EpisodeCacheMap.insert(std::make_pair(id, cacheItem));
  setCurrentLanguage();
}

std::shared_ptr<CVideoInfoTag> CVideoDatabaseCache::getEpisode(long id, uint64_t updatedAt)
{
  CSingleLock lock(m_mutex);
  tVideoInfoTagCacheMap ::iterator it = m_EpisodeCacheMap.find(id);
  
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

std::string CVideoDatabaseCache::getTranslation(std::string key, uint64_t updatedAt)
{
  CSingleLock lock(m_mutex);
  tTranslationCacheMap::iterator it = m_TranslationCacheMap.find(key);
  
  if (it != m_TranslationCacheMap.end())
  {
    // Only go to the database if the translation is outdated
    if (it->second.m_updatedAt != 0 && updatedAt > it->second.m_updatedAt)
    {
      std::shared_ptr<odb::transaction> odb_transaction (CCommonDatabase::GetInstance().getTransaction());
      typedef odb::query<CODBTranslation> query;
      std::string language = m_language.substr(18);

      CODBTranslation translation;
      if (CCommonDatabase::GetInstance().getDB()->query_one<CODBTranslation>(query::key == key && query::language == language, translation))
      {
        it->second.m_updatedAt = updatedAt;
        it->second.m_text = translation.m_text;
      }
    }
    else
    {
      it->second.m_updatedAt = updatedAt;
    }
    return it->second.m_text;
  }
  else
  {
    if (m_tryReload)
    {
      // Reload only once until the flag is reset, otherwise we will
      // pretty sure end up reloading all the time which is a pain, performancewise
      // This flag gets reset in our GetXXXXNav(...) methods, which
      // form the public interface of the videodb
      loadTranslations();
      m_tryReload = false;
    }
  }

  return "";
}

bool CVideoDatabaseCache::GetTVShowTranslations()
{
  try
  {
    // Translate TVShows elements
    for (auto& item : m_TVShowCacheMap)
    {
      std::stringstream ss;
      ss << "tvshow." << item.second.m_item->m_iDbId << ".title";
      std::string key = ss.str();
      std::string title = getTranslation(key, item.second.m_updatedAt);
      if (!title.empty())
      {
        item.second.m_item->SetTitle(title);
        item.second.m_item->SetSortTitle(title);
      }
      
      ss.str("");
      ss.clear();
      ss << "tvshow." << item.second.m_item->m_iDbId << ".plot";
      key = ss.str();
      std::string plot = getTranslation(key, item.second.m_updatedAt);
      if (!plot.empty())
        item.second.m_item->SetPlot(plot);
    }

    // Translate Season elements
    for (auto& item : m_SeasonCacheMap)
    {
      std::stringstream ss;
      ss << "tvshow." << item.second.m_item->m_iDbId << ".title";
      std::string key = ss.str();
      std::string title = getTranslation(key, item.second.m_updatedAt);
      if (!title.empty())
        item.second.m_item->SetShowTitle(title);

      ss.str("");
      ss.clear();
      ss << "tvshow." << item.second.m_item->m_iDbId << ".plot";
      key = ss.str();
      std::string plot = getTranslation(key, item.second.m_updatedAt);
      if (!plot.empty())
        item.second.m_item->SetPlot(plot);

      ss.str("");
      ss.clear();
      ss << "season." << item.second.m_item->m_iDbId << ".title";
      key = ss.str();
      title = getTranslation(key, item.second.m_updatedAt);
      if (!title.empty())
      {
        item.second.m_item->SetTitle(title);
        item.second.m_item->SetSortTitle(title);
      }

      if (item.second.m_item->m_iSeason == 0)
      {
        item.second.m_item->SetTitle(g_localizeStrings.Get(20381));
        item.second.m_item->SetSortTitle(g_localizeStrings.Get(20381));
      }
      else
      {
        item.second.m_item->SetTitle(StringUtils::Format(g_localizeStrings.Get(20358).c_str(), item.second.m_item->m_iSeason));
        item.second.m_item->SetSortTitle(StringUtils::Format(g_localizeStrings.Get(20358).c_str(), item.second.m_item->m_iSeason));
      }
    }

    // Translate Episode elements
    for (auto& item : m_EpisodeCacheMap)
    {
      std::stringstream ss;
      ss << "episode." << item.second.m_item->m_iDbId << ".title";
      std::string key = ss.str();
      std::string title = getTranslation(key, item.second.m_updatedAt);
      if (!title.empty())
        item.second.m_item->SetShowTitle(title);

      ss.str("");
      ss.clear();
      ss << "episode." << item.second.m_item->m_iDbId << ".plot";
      key = ss.str();
      std::string plot = getTranslation(key, item.second.m_updatedAt);
      if (!plot.empty())
        item.second.m_item->SetPlot(plot);
    }
  }
  catch (std::exception& e)
  {
    CLog::Log(LOGERROR, "%s failed - %s", __FUNCTION__, e.what());
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s failed", __FUNCTION__);
  }
  return true;
}

bool CVideoDatabaseCache::GetMovieTranslations()
{
  bool ret = false;
  
  try
  {
    for (auto& item : m_MovieCacheMap)
    {
      std::stringstream ss;
      ss << "movie." << item.second.m_item->m_iDbId << ".title";
      std::string key = ss.str();
      std::string title = getTranslation(key, item.second.m_updatedAt);
      if (!title.empty())
      {
        item.second.m_item->SetTitle(title);
        item.second.m_item->SetSortTitle(title);
      }

      ss.str("");
      ss.clear();
      ss << "movie." << item.second.m_item->m_iDbId << ".plot";
      key = ss.str();
      std::string plot = getTranslation(key, item.second.m_updatedAt);
      if (!plot.empty())
        item.second.m_item->SetPlot(plot);
    }

    ret = true;
  }
  catch (std::exception& e)
  {
    CLog::Log(LOGERROR, "%s failed - %s", __FUNCTION__, e.what());
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s failed", __FUNCTION__);
  }
  
  return ret;
}
