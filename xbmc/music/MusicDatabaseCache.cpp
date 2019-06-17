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

#include "MusicDatabaseCache.h"

#include <odb/odb_gen/ODBTranslation.h>
#include <odb/odb_gen/ODBTranslation_odb.h>

#include "FileItem.h"
#include "settings/Settings.h"
#include <ServiceBroker.h>
#include "settings/SettingsComponent.h"

namespace MUSICDBCACHE
{
  CMusicDatabaseCache::CMusicDatabaseCache()
  {
    m_language = "resource.language.en_gb";
  }

  CMusicDatabaseCache::~CMusicDatabaseCache()
  {

  }

  void CMusicDatabaseCache::setCurrentLanguage()
  {
    m_language = CServiceBroker::GetSettingsComponent()->GetSettings()->GetString(CSettings::SETTING_LOCALE_LANGUAGE);
  }

  void CMusicDatabaseCache::loadTranslations()
  {
    CSingleLock lock(m_mutex);
    m_TranslationCacheMap.clear();

    std::shared_ptr<odb::transaction> odb_transaction (CCommonDatabase::GetInstance().getTransaction());
    typedef odb::query<CODBTranslation> query;

    std::string language = m_language.substr(18);

    odb::result<CODBTranslation> r(CCommonDatabase::GetInstance().getDB()->query<CODBTranslation>(query::language == language));
    for (CODBTranslation& translation : r)
    {
      CMusicDatabaseTranslationItem item;
      item.m_updatedAt = 0;
      item.m_language = language;
      item.m_text = translation.m_text;

      m_TranslationCacheMap.insert(std::make_pair(translation.m_key, item));
    }
  }

  void CMusicDatabaseCache::languageChange()
  {
    m_language = CServiceBroker::GetSettingsComponent()->GetSettings()->GetString(CSettings::SETTING_LOCALE_LANGUAGE);

    loadTranslations();
  }

  void CMusicDatabaseCache::addSong(long id, std::shared_ptr<MUSIC_INFO::CMusicInfoTag>& item, int getDetails)
  {
    if (!item)
      return;

    CMusicDatabaseCacheItem<MUSIC_INFO::CMusicInfoTag> cacheItem;
    cacheItem.m_getDetails = getDetails;
    cacheItem.m_item = item;

    CSingleLock lock(m_mutex);
    m_songCacheMap.insert(std::make_pair(id, cacheItem));
  }

  std::shared_ptr<MUSIC_INFO::CMusicInfoTag> CMusicDatabaseCache::getSong(long id, int getDetails)
  {
    CSingleLock lock(m_mutex);
    tMusicInfoTagCacheMap ::iterator it = m_songCacheMap.find(id);

    if (it != m_songCacheMap.end())
    {
      // If we do not have enough details, delete the item
      if (it->second.m_getDetails < getDetails)
      {
        m_songCacheMap.erase(it);
        return nullptr;
      }

      return it->second.m_item;
    }

    return nullptr;
  }

  void CMusicDatabaseCache::addAlbum(long id, std::shared_ptr<MUSIC_INFO::CMusicInfoTag>& item, int getDetails)
  {
    if (!item)
      return;

    CMusicDatabaseCacheItem<MUSIC_INFO::CMusicInfoTag> cacheItem;
    cacheItem.m_getDetails = getDetails;
    cacheItem.m_item = item;

    CSingleLock lock(m_mutex);
    m_albumCacheMap.insert(std::make_pair(id, cacheItem));
  }

  std::shared_ptr<MUSIC_INFO::CMusicInfoTag> CMusicDatabaseCache::getAlbum(long id, int getDetails)
  {
    CSingleLock lock(m_mutex);
    tMusicInfoTagCacheMap::iterator it = m_albumCacheMap.find(id);

    if (it != m_albumCacheMap.end())
    {
      if (it->second.m_getDetails < getDetails)
      {
        m_albumCacheMap.erase(it);
        return nullptr;
      }

      return it->second.m_item;
    }

    return nullptr;
  }

  void CMusicDatabaseCache::addArtMap(long id, std::shared_ptr<tArtTypeCacheType>& item, std::string type)
  {
    CMusicDatabaseCacheItem<tArtTypeCacheType> cacheItem;
    cacheItem.m_getDetails = 0;
    cacheItem.m_item = item;

    CSingleLock lock(m_mutex);
    m_ArtCacheMap[type].insert(std::make_pair(id, cacheItem));
  }

  std::shared_ptr<tArtTypeCacheType> CMusicDatabaseCache::getArtMap(long id, std::string type)
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

  void CMusicDatabaseCache::addArtistArtMap(long id, std::shared_ptr<tArtTypeCacheType>& item, std::string type)
  {
    CMusicDatabaseCacheItem<tArtTypeCacheType> cacheItem;
    cacheItem.m_getDetails = 0;
    cacheItem.m_item = item;

    CSingleLock lock(m_mutex);
    m_ArtistArtMapCacheMap[type].insert(std::make_pair(id, cacheItem));
  }

  std::shared_ptr<tArtTypeCacheType> CMusicDatabaseCache::getArtistArtMap(long id, std::string type)
  {
    CSingleLock lock(m_mutex);
    tArtTypeCacheMap ::iterator it = m_ArtistArtMapCacheMap.find(type);

    if (it != m_ArtistArtMapCacheMap.end())
    {
      tArtCacheMap ::iterator artIt = it->second.find(id);

      if (artIt != it->second.end())
      {
        return artIt->second.m_item;
      }
    }

    return nullptr;
  }

  void CMusicDatabaseCache::addArtistArt(long id, std::shared_ptr<tArtistArtTypeCacheType>& item, std::string type)
  {
    CMusicDatabaseCacheItem<tArtistArtTypeCacheType> cacheItem;
    cacheItem.m_getDetails = 0;
    cacheItem.m_item = item;

    CSingleLock lock(m_mutex);
    m_ArtistArtCacheMap[type].insert(std::make_pair(id, cacheItem));
  }

  std::shared_ptr<tArtistArtTypeCacheType> CMusicDatabaseCache::getArtistArt(long id, std::string type)
  {
    CSingleLock lock(m_mutex);
    tArtistArtTypeCacheMap ::iterator it = m_ArtistArtCacheMap.find(type);

    if (it != m_ArtistArtCacheMap.end())
    {
      tArtistArtCacheMap ::iterator artIt = it->second.find(id);

      if (artIt != it->second.end())
      {
        return artIt->second.m_item;
      }
    }

    return nullptr;
  }

  void CMusicDatabaseCache::addArtist(long id, std::shared_ptr<CFileItem>& item)
  {
    if (!item)
      return;

    CMusicDatabaseCacheItem<CFileItem> cacheItem;
    cacheItem.m_getDetails = 0;
    cacheItem.m_item = item;

    CSingleLock lock(m_mutex);
    m_ArtistCacheMap.insert(std::make_pair(id, cacheItem));
  }

  std::shared_ptr<CFileItem> CMusicDatabaseCache::getArtist(long id)
  {
    CSingleLock lock(m_mutex);
    tFileItemCacheMap ::iterator it = m_ArtistCacheMap.find(id);

    if (it != m_ArtistCacheMap.end())
    {
      return it->second.m_item;
    }

    return nullptr;
  }

  void CMusicDatabaseCache::addArtThumbLoader(int songId, int albumId, int artistId, int playlistId, bool bPrimaryArtist, std::vector<ArtForThumbLoader> &art)
  {
    CSingleLock lock(m_mutex);
    std::shared_ptr<std::vector<ArtForThumbLoader> > art_ptr(new std::vector<ArtForThumbLoader>());
    *art_ptr = art;
    m_ArtThumbLoaderCacheMap[songId][albumId][artistId][bPrimaryArtist][playlistId] = art_ptr;
  }

  std::shared_ptr<std::vector<ArtForThumbLoader> > CMusicDatabaseCache::getArtThumbLoader(int songId, int albumId, int artistId, int playlistId, bool bPrimaryArtist)
  {
    CSingleLock lock(m_mutex);

    tArtThumbLoaderTypeMap::iterator it1 = m_ArtThumbLoaderCacheMap.find(songId);
    if (it1 == m_ArtThumbLoaderCacheMap.end())
      return nullptr;

    tArtThumbLoaderType_c::iterator it2 = it1->second.find(albumId);
    if (it2 == it1->second.end())
      return nullptr;

    tArtThumbLoaderType_b::iterator it3 = it2->second.find(artistId);
    if (it3 == it2->second.end())
      return nullptr;

    tArtThumbLoaderType_a::iterator it4 = it3->second.find(bPrimaryArtist);
    if (it4 == it3->second.end())
      return nullptr;

    tArtThumbLoaderType::iterator it5 = it4->second.find(playlistId);
    if (it5 == it4->second.end())
      return nullptr;

    return it5->second;
  }

  std::string CMusicDatabaseCache::getTranslation(std::string key, uint64_t updatedAt)
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
    // We cache all available translations when we load a language
    // The below code makes loading covers incredibly slow if not all translations
    // are in place, because then we start to lookup every item in the database again.
    // This is not what we want, I keep the code just for reference if we need to revise my
    // decision again.
    // else
    // {
    //   std::shared_ptr<odb::transaction> odb_transaction (CCommonDatabase::GetInstance().getTransaction());
    //   typedef odb::query<CODBTranslation> query;
    //   std::string language = m_language.substr(18);
    //   CODBTranslation translation;
    //   if (CCommonDatabase::GetInstance().getDB()->query_one<CODBTranslation>(query::key == key && query::language == language, translation))
    //   {
    //     CVideoDatabaseTranslationItem item;
    //     item.m_updatedAt = updatedAt;
    //     item.m_language = language;
    //     item.m_text = translation.m_text;

    //     m_TranslationCacheMap.insert(std::make_pair(translation.m_key, item));

    //     return item.m_text;
    //   }
    // }

    return "";
  }
}
