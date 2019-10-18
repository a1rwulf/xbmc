/*
 *  Copyright (C) 2005-2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "MetadataUtils.h"
#include "../Filesystem.h"
#include "../AddonBase.h"

namespace kodi
{
namespace addon
{
class CInstanceMetadataProvider;
}
} // namespace kodi

extern "C"
{

  struct AddonInstance_MetadataProvider;

  /*!
 * @brief MetadataProvider properties
 *
 * Not to be used outside this header.
 */
  typedef struct AddonProps_MetadataProvider
  {
    int dummy;
    //! @todo
    // Add addon properties
  } AddonProps_MetadataProvider;

  /*!
 * @brief MetadataProvider callbacks
 *
 * Not to be used outside this header.
 */
  typedef struct AddonToKodiFuncTable_MetadataProvider
  {
    KODI_HANDLE kodiInstance;
    void (__cdecl* transfer_list_entry)(void* ctx, void* hdl, struct VFSDirEntry* entry);
  } AddonToKodiFuncTable_MetadataProvider;

  /*!
 * @brief MetadataProvider function hooks
 *
 * Not to be used outside this header.
 */
  typedef struct KodiToAddonFuncTable_MetadataProvider
  {
    kodi::addon::CInstanceMetadataProvider* addonInstance;

    bool(__cdecl* GetPlaylists)(AddonInstance_MetadataProvider* instance,
                                void* hdl,
                                const char* strBaseDir,
                                const char* sqlFilter,
                                int sortBy,
                                int sortOrder,
                                int sortAttributes,
                                int sortLimitStart,
                                int sortLimitEnd,
                                bool countOnly);

    bool(__cdecl* GetSongs)(AddonInstance_MetadataProvider* instance,
                            void* hdl,
                            const char* strBaseDir,
                            int idGenre,
                            int idArtist,
                            int idAlbum,
                            int idPlaylist,
                            int sortBy,
                            int sortOrder,
                            int sortAttributes,
                            int sortLimitStart,
                            int sortLimitEnd);

  } KodiToAddonFuncTable_MetadataProvider;

  /*!
 * @brief MetadataProvider instance
 *
 * Not to be used outside this header.
 */
  typedef struct AddonInstance_MetadataProvider
  {
    AddonProps_MetadataProvider* props;
    AddonToKodiFuncTable_MetadataProvider* toKodi;
    KodiToAddonFuncTable_MetadataProvider* toAddon;
  } AddonInstance_MetadataProvider;

} /* extern "C" */

namespace kodi
{
namespace addon
{

//============================================================================
///
/// \addtogroup cpp_kodi_addon_MetadataProvider
/// @brief \cpp_class{ kodi::addon::CInstanceMetadataProvider }
/// **MetadataProvider add-on instance**
///
/// A MetadataProvider is a Kodi addon that provides kodi with
/// metadata for movies, tvshows, musicvideos and/or music.
///
/// Include the header \ref MetadataProvider.h "#include <kodi/addon-instance/MetadataProvider.h>"
/// to use this class.
///
///
/// The destruction of the example class `CMyMetadataProvider` is called from
/// Kodi's header. Manually deleting the add-on instance is not required.
///
//----------------------------------------------------------------------------
class CInstanceMetadataProvider : public IAddonInstance
{
public:
  //==========================================================================
  ///
  /// @ingroup cpp_kodi_addon_MetadataProvider
  /// @brief MetadataProvider class constructor
  ///
  /// Used by an add-on that only supports MetadataProviders.
  ///
  CInstanceMetadataProvider()
      : IAddonInstance(ADDON_INSTANCE_METADATAPROVIDER)
  {
    if (CAddonBase::m_interface->globalSingleInstance != nullptr)
      throw std::logic_error("kodi::addon::CInstanceMetadataProvider: Creation of more as one in "
                             "single instance way is not allowed!");

    SetAddonStruct(CAddonBase::m_interface->firstKodiInstance);
    CAddonBase::m_interface->globalSingleInstance = this;
  }
  //--------------------------------------------------------------------------

  //==========================================================================
  ///
  /// @ingroup cpp_kodi_addon_MetadataProvider
  /// @brief MetadataProvider class constructor used to support multiple instance
  /// types
  ///
  /// @param[in] instance               The instance value given to
  ///                                   <b>`kodi::addon::CAddonBase::CreateInstance(...)`</b>.
  ///
  /// @warning Only use `instance` from the CreateInstance call
  ///
  explicit CInstanceMetadataProvider(KODI_HANDLE instance)
      : IAddonInstance(ADDON_INSTANCE_METADATAPROVIDER)
  {
    if (CAddonBase::m_interface->globalSingleInstance != nullptr)
      throw std::logic_error("kodi::addon::CInstanceMetadataProvider: Creation of multiple "
                             "together with single instance way is not allowed!");

    SetAddonStruct(instance);
  }
  //--------------------------------------------------------------------------

  //==========================================================================
  ///
  /// @ingroup cpp_kodi_addon_MetadataProvider
  /// @brief Destructor
  ///
  ~CInstanceMetadataProvider() override = default;
  //--------------------------------------------------------------------------

  virtual bool GetPlaylists(const std::string& strBaseDir,
                            std::vector<vfs::CDirEntry>& items,
                            const std::string& sqlFilter,
                            const AddonSortDescription& sortDescription,
                            bool countOnly)
  {
    return true;
  }

  virtual bool GetSongs(const std::string& strBaseDir,
                        std::vector<vfs::CDirEntry>& items,
                        int idGenre,
                        int idArtist,
                        int idAlbum,
                        int idPlaylist,
                        const AddonSortDescription& sortDescription)
  {
    return true;
  }

private:
  void SetAddonStruct(KODI_HANDLE instance)
  {
    if (instance == nullptr)
      throw std::logic_error("kodi::addon::CInstanceMetadataProvider: Creation with empty addon "
                             "structure not allowed, table must be given from Kodi!");

    m_instanceData = static_cast<AddonInstance_MetadataProvider*>(instance);
    m_instanceData->toAddon->addonInstance = this;

    m_instanceData->toAddon->GetPlaylists = ADDON_GetPlaylists;
    m_instanceData->toAddon->GetSongs = ADDON_GetSongs;
  }

  inline static bool ADDON_GetPlaylists(AddonInstance_MetadataProvider* instance,
                                        void* hdl,
                                        const char* strBaseDir,
                                        const char* sqlFilter,
                                        int sortBy,
                                        int sortOrder,
                                        int sortAttributes,
                                        int sortLimitStart,
                                        int sortLimitEnd,
                                        bool countOnly)
  {
    AddonSortDescription sortDescription;
    sortDescription.sortBy = static_cast<AddonSortBy>(sortBy);
    sortDescription.sortOrder = static_cast<AddonSortOrder>(sortOrder);
    sortDescription.sortAttributes = static_cast<AddonSortAttribute>(sortAttributes);
    sortDescription.limitStart = sortLimitStart;
    sortDescription.limitEnd = sortLimitEnd;

    std::vector<vfs::CDirEntry> items;
    bool ret = instance->toAddon->addonInstance->GetPlaylists(strBaseDir, items, sqlFilter, sortDescription,
                                                              countOnly);
    if (ret)
      instance->toAddon->addonInstance->TransferListEntries(items, hdl);
    return ret;
  }

  inline static bool ADDON_GetSongs(AddonInstance_MetadataProvider* instance,
                                    void* hdl,
                                    const char* strBaseDir,
                                    int idGenre,
                                    int idArtist,
                                    int idAlbum,
                                    int idPlaylist,
                                    int sortBy,
                                    int sortOrder,
                                    int sortAttributes,
                                    int sortLimitStart,
                                    int sortLimitEnd)
  {
    AddonSortDescription sortDescription;
    sortDescription.sortBy = static_cast<AddonSortBy>(sortBy);
    sortDescription.sortOrder = static_cast<AddonSortOrder>(sortOrder);
    sortDescription.sortAttributes = static_cast<AddonSortAttribute>(sortAttributes);
    sortDescription.limitStart = sortLimitStart;
    sortDescription.limitEnd = sortLimitEnd;

    std::vector<vfs::CDirEntry> items;
    bool ret = instance->toAddon->addonInstance->GetSongs(strBaseDir, items, idGenre, idArtist, idAlbum,
                                                         idPlaylist, sortDescription);
    if (ret)
      instance->toAddon->addonInstance->TransferListEntries(items, hdl);

    return ret;
  }

  void TransferListEntries(std::vector<vfs::CDirEntry>& items, void* hdl)
  {
    VFSDirEntry entry;
    entry.properties = nullptr;

    for (auto& item : items)
    {
      entry.label = const_cast<char*>(item.Label().c_str());
      entry.title = const_cast<char*>(item.Title().c_str());
      entry.path = const_cast<char*>(item.Path().c_str());
      entry.date_time = item.DateTime();
      entry.folder = item.IsFolder();
      entry.size = item.Size();

      entry.num_props = 0;
      const std::map<std::string, std::string>& props = item.GetProperties();
      if (!props.empty())
      {
        entry.properties = static_cast<VFSProperty*>(realloc(entry.properties, entry.num_props*sizeof(VFSProperty)));
        for (const auto& prop : props)
        {
          entry.properties[entry.num_props].name = const_cast<char*>(prop.first.c_str());
          entry.properties[entry.num_props].val = const_cast<char*>(prop.second.c_str());
          ++entry.num_props;
        }
      }

      m_instanceData->toKodi->transfer_list_entry(m_instanceData->toKodi->kodiInstance, hdl, &entry);
    }

    if (entry.properties)
      free(entry.properties);
  }

  AddonInstance_MetadataProvider* m_instanceData;
};

} /* namespace addon */
} /* namespace kodi */
