/*
 *  Copyright (C) 2005-2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "MetadataProvider.h"
#include "VFSEntry.h"
#include "utils/log.h"

namespace ADDON
{

CMetadataProvider::CMetadataProvider(BinaryAddonBasePtr addonBase)
 : IAddonInstanceHandler(ADDON_INSTANCE_METADATAPROVIDER, addonBase)
{
  m_struct.props = new AddonProps_MetadataProvider;

  m_struct.toKodi = new AddonToKodiFuncTable_MetadataProvider;
  m_struct.toKodi->kodiInstance = this;
  m_struct.toKodi->transfer_list_entry = transfer_list_entry;

  m_struct.toAddon = new KodiToAddonFuncTable_MetadataProvider;
  memset(m_struct.toAddon, 0, sizeof(AddonToKodiFuncTable_MetadataProvider));

  /* Open the class "kodi::addon::CInstanceMetadataProvider" on add-on side */
  if (CreateInstance(&m_struct) != ADDON_STATUS_OK)
    CLog::Log(LOGFATAL, "MetadataProvider: failed to create instance for '%s'!", ID().c_str());
}

CMetadataProvider::~CMetadataProvider()
{
  /* Destroy the class "kodi::addon::CInstanceMetadataProvider" on add-on side */
  DestroyInstance();

  delete m_struct.toAddon;
  delete m_struct.toKodi;
  delete m_struct.props;
}

bool CMetadataProvider::GetPlaylists(const std::string& strBaseDir,
                  CFileItemList& items,
                  const CDatabase::Filter& filter,
                  const SortDescription& sortDescription,
                  bool countOnly)
{
  if (!m_struct.toAddon->GetPlaylists)
    return false;

  std::string strSQL;
  CDatabase::BuildSQL("", filter, strSQL);

  return m_struct.toAddon->GetPlaylists(&m_struct, &items, strBaseDir.c_str(), strSQL.c_str(),
                                        kodi::addon::TransToAddonSortBy(sortDescription.sortBy),
                                        kodi::addon::TransToAddonSortOrder(sortDescription.sortOrder),
                                        kodi::addon::TransToAddonSortAttribute(sortDescription.sortAttributes),
                                        sortDescription.limitStart,
                                        sortDescription.limitEnd,
                                        countOnly);
}

bool CMetadataProvider::GetSongs(const std::string& strBaseDir,
              CFileItemList& items,
              int idGenre,
              int idArtist,
              int idAlbum,
              int idPlaylist,
              const SortDescription &sortDescription)
{
  if (!m_struct.toAddon->GetSongs)
    return false;

  return m_struct.toAddon->GetSongs(&m_struct, &items, strBaseDir.c_str(), idGenre, idArtist, idAlbum, idPlaylist,
                                    kodi::addon::TransToAddonSortBy(sortDescription.sortBy),
                                    kodi::addon::TransToAddonSortOrder(sortDescription.sortOrder),
                                    kodi::addon::TransToAddonSortAttribute(sortDescription.sortAttributes),
                                    sortDescription.limitStart,
                                    sortDescription.limitEnd);
}

void CMetadataProvider::transfer_list_entry(void* ctx, void* hdl, struct VFSDirEntry* entry)
{
  CFileItemList* items = static_cast<CFileItemList*>(hdl);
  if (!ctx || !items)
  {
    CLog::Log(LOGERROR, "CMetadataProvider::%s - invalid data (ctx='%p', hdl='%p')", __func__, ctx, hdl);
    return;
  }

  CVFSEntry::VFSDirEntriesToCFileItemList(1, entry, *items);
}


} /* namespace ADDON */
