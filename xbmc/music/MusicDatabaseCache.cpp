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

#include "FileItem.h"

CMusicDatabaseCache::CMusicDatabaseCache()
{
  
}

CMusicDatabaseCache::~CMusicDatabaseCache()
{
  
}

void CMusicDatabaseCache::addSong(long id, std::shared_ptr<CFileItem>& item)
{
  if (!item)
    return;
  
  CMusicDatabaseCacheItem<CFileItem> cacheItem;
  cacheItem.m_getDetails = 0;
  cacheItem.m_item = item;
  
  CSingleLock lock(m_mutex);
  m_fileItemCacheMap.insert(std::make_pair(id, cacheItem));
}

std::shared_ptr<CFileItem> CMusicDatabaseCache::getSong(long id)
{
  CSingleLock lock(m_mutex);
  tFileItemCacheMap ::iterator it = m_fileItemCacheMap.find(id);
  
  if (it != m_fileItemCacheMap.end())
  {
    // If we do not have enough details, delete the item
    if (it->second.m_getDetails < 0)
    {
      m_fileItemCacheMap.erase(it);
      return nullptr;
    }
    
    return it->second.m_item;
  }
  
  return nullptr;
}

void CMusicDatabaseCache::addAlbum(long id, std::shared_ptr<CFileItem>& item)
{
  if (!item)
    return;
  
  CMusicDatabaseCacheItem<CFileItem> cacheItem;
  cacheItem.m_getDetails = 0;
  cacheItem.m_item = item;
  
  CSingleLock lock(m_mutex);
  m_albumCacheMap.insert(std::make_pair(id, cacheItem));
}

std::shared_ptr<CFileItem> CMusicDatabaseCache::getAlbum(long id)
{
  CSingleLock lock(m_mutex);
  tFileItemCacheMap ::iterator it = m_albumCacheMap.find(id);
  
  if (it != m_albumCacheMap.end())
  {
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

void CMusicDatabaseCache::addArtThumbLoader(int songId, int albumId, int artistId, bool bPrimaryArtist, std::vector<ArtForThumbLoader> &art)
{
  CSingleLock lock(m_mutex);
  std::shared_ptr<std::vector<ArtForThumbLoader> > art_ptr(new std::vector<ArtForThumbLoader>());
  *art_ptr = art;
  m_ArtThumbLoaderCacheMap[songId][albumId][artistId][bPrimaryArtist] = art_ptr;
}

std::shared_ptr<std::vector<ArtForThumbLoader> > CMusicDatabaseCache::getArtThumbLoader(int songId, int albumId, int artistId, bool bPrimaryArtist)
{
  CSingleLock lock(m_mutex);

  tArtThumbLoaderTypeMap::iterator it1 = m_ArtThumbLoaderCacheMap.find(songId);
  if (it1 == m_ArtThumbLoaderCacheMap.end())
    return nullptr;

  tArtThumbLoaderType_b::iterator it2 = it1->second.find(albumId);
  if (it2 == it1->second.end())
    return nullptr;

  tArtThumbLoaderType_a::iterator it3 = it2->second.find(artistId);
  if (it3 == it2->second.end())
    return nullptr;

  tArtThumbLoaderType::iterator it4 = it3->second.find(bPrimaryArtist);
  if (it4 == it3->second.end())
    return nullptr;

  return it4->second;
}
