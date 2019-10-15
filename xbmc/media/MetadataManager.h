/*
 *  Copyright (C) 2012-2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include "FileItem.h"
#include "dbwrappers/Database.h"

namespace ADDON
{
  class CMetadataProvider;
}

namespace METADATA
{
class IMetadataProvider;
class CMetadataProviders;

typedef std::map<std::string, std::shared_ptr<ADDON::CMetadataProvider>> ProviderMap;

class CMetadataManager
{
public:
  CMetadataManager();
  ~CMetadataManager() = default;

  void AddProvider(const std::string& name, std::shared_ptr<ADDON::CMetadataProvider> provider);
  void RemoveProvider(const std::string& name);

  bool GetPlaylists(const std::string& strBaseDir,
                    CFileItemList& items,
                    const CDatabase::Filter& filter,
                    const SortDescription& sortDescription,
                    bool countOnly);

  bool GetSongs(const std::string& strBaseDir,
                CFileItemList& items,
                int idGenre,
                int idArtist,
                int idAlbum,
                int idPlaylist,
                const SortDescription &sortDescription = SortDescription());

private:
  ProviderMap m_providers;
};
} // namespace METADATA
