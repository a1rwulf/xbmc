/*
 *  Copyright (C) 2012-2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "MetadataManager.h"

#include "URL.h"
#include "dbwrappers/Database.h"
#include "providers/ApiMetadataProvider.h"
#include "providers/DatabaseMetadataProvider.h"
#include "providers/MetadataProvider.h"

using namespace METADATA;

CMetadataManager::CMetadataManager()
{
  std::shared_ptr<IMetadataProvider> dbprovider(new CDatabaseMetadataProvider());
  std::shared_ptr<IMetadataProvider> apiprovider(new CApiMetadataProvider("http://omniyon.local:10101"));
  AddProvider("commondb", dbprovider);
  AddProvider("oam", apiprovider);
}

void CMetadataManager::AddProvider(const std::string& name,
                                   std::shared_ptr<IMetadataProvider> provider)
{
  m_providers.insert(std::make_pair(name, provider));
}

void CMetadataManager::RemoveProvider(const std::string& name)
{
  m_providers.erase(name);
}

bool CMetadataManager::GetPlaylists(const std::string& strBaseDir,
                                    CFileItemList& items,
                                    const CDatabase::Filter& filter,
                                    const SortDescription& sortDescription,
                                    bool countOnly)
{
  CURL url(strBaseDir);

  //! @todo - change protocol to "all" later
  if (url.GetProtocol() == "musicdb")
  {
    for (auto& provider : m_providers)
    {
      if (provider.second->GetSupportedEntities() & SupportedEntities::Playlist)
        provider.second->GetPlaylists(strBaseDir, items, filter, sortDescription, countOnly);
    }
  }
  else
  {
    ProviderMap::iterator it = m_providers.find(url.GetProtocol());
    if (it != m_providers.end())
      it->second->GetPlaylists(strBaseDir, items, filter, sortDescription, countOnly);
  }

  return true;
}

bool CMetadataManager::GetSongs(const std::string& strBaseDir,
              CFileItemList& items,
              int idGenre,
              int idArtist,
              int idAlbum,
              int idPlaylist,
              const SortDescription &sortDescription)
{
  CURL url(strBaseDir);

  //! @todo - change protocol to "all" later
  if (url.GetProtocol() == "musicdb")
  {
    for (auto& provider : m_providers)
    {
      provider.second->GetSongs(strBaseDir, items, idGenre, idArtist, idAlbum, idPlaylist, sortDescription);
    }
  }
  else
  {
    ProviderMap::iterator it = m_providers.find(url.GetProtocol());
    if (it != m_providers.end())
      it->second->GetSongs(strBaseDir, items, idGenre, idArtist, idAlbum, idPlaylist, sortDescription);
  }

  return true;
}
