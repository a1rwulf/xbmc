/*
 *  Copyright (C) 2005-2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addons/kodi-addon-dev-kit/include/kodi/addon-instance/MetadataProvider.h"
#include "addons/binary-addons/AddonInstanceHandler.h"

namespace ADDON
{

class CMetadataProvider : public IAddonInstanceHandler
{
public:
  explicit CMetadataProvider(BinaryAddonBasePtr addonBase);
  ~CMetadataProvider() override;

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
  static void transfer_list_entry(void* ctx, void* hdl, struct VFSDirEntry* entry);

  AddonInstance_MetadataProvider m_struct;
};

} /* namespace ADDON */
