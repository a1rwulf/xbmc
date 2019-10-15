/*
 *  Copyright (C) 2005-2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "MetadataProvider.h"
#include "utils/log.h"

namespace ADDON
{

CMetadataProvider::CMetadataProvider(BinaryAddonBasePtr addonBase)
 : IAddonInstanceHandler(ADDON_INSTANCE_METADATAPROVIDER, addonBase)
{
  //! @todo a1rwulf - check why this gives a compiler error (it works for screensaver/pvr)
  //m_struct = {{0}};

  m_struct.toKodi.kodiInstance = this;

  /* Open the class "kodi::addon::CInstanceMetadataProvider" on add-on side */
  if (CreateInstance(&m_struct) != ADDON_STATUS_OK)
    CLog::Log(LOGFATAL, "MetadataProvider: failed to create instance for '%s'!", ID().c_str());
}

CMetadataProvider::~CMetadataProvider()
{
  /* Destroy the class "kodi::addon::CInstanceMetadataProvider" on add-on side */
  DestroyInstance();
}

bool CMetadataProvider::GetPlaylists(const std::string& strBaseDir,
                  CFileItemList& items,
                  const CDatabase::Filter& filter,
                  const SortDescription& sortDescription,
                  bool countOnly)
{
  return m_struct.toAddon.GetPlaylists(&m_struct, strBaseDir, items, filter, sortDescription, countOnly);
}

bool CMetadataProvider::GetSongs(const std::string& strBaseDir,
              CFileItemList& items,
              int idGenre,
              int idArtist,
              int idAlbum,
              int idPlaylist,
              const SortDescription &sortDescription)
{
  return m_struct.toAddon.GetSongs(&m_struct, strBaseDir, items, idGenre, idArtist, idAlbum, idPlaylist, sortDescription);
}

} /* namespace ADDON */
