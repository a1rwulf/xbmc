/*
 *  Copyright (C) 2012-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <vector>

#include "dbwrappers/Database.h"

#include "MetadataProvider.h"

class CURL;

namespace METADATA
{
class CApiMetadataProvider : public IMetadataProvider
{
public:
  CApiMetadataProvider(std::string baseUrl);
  virtual ~CApiMetadataProvider() = default;

  virtual bool GetPlaylists(const std::string& strBaseDir,
                            CFileItemList& items,
                            const SortDescription& sortDescription,
                            bool countOnly) override;

  virtual bool GetSongs(const std::string& strBaseDir,
                CFileItemList& items,
                int idGenre,
                int idArtist,
                int idAlbum,
                int idPlaylist,
                const SortDescription &sortDescription) override;

  virtual SupportedEntities GetSupportedEntities() override;

private:
  std::string GetRequestUrl(const CURL& url) const;

  std::string m_baseUrl;
};
} // namespace METADATA
