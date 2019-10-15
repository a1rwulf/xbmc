/*
 *  Copyright (C) 2005-2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once



#ifdef BUILD_KODI_ADDON
#include "omniplayer/AddonBase.h"
#include "../FileItem.h"
#else
#include "FileItem.h"
#include "dbwrappers/Database.h"
#include "../AddonBase.h"
#endif

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
                                const std::string& strBaseDir,
                                CFileItemList& items,
                                const CDatabase::Filter& filter,
                                const SortDescription& sortDescription,
                                bool countOnly);

    bool(__cdecl* GetSongs)(AddonInstance_MetadataProvider* instance,
                            const std::string& strBaseDir,
                            CFileItemList& items,
                            int idGenre,
                            int idArtist,
                            int idAlbum,
                            int idPlaylist,
                            const SortDescription& sortDescription);

  } KodiToAddonFuncTable_MetadataProvider;

  /*!
 * @brief MetadataProvider instance
 *
 * Not to be used outside this header.
 */
  typedef struct AddonInstance_MetadataProvider
  {
    AddonProps_MetadataProvider props;
    AddonToKodiFuncTable_MetadataProvider toKodi;
    KodiToAddonFuncTable_MetadataProvider toAddon;
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
                            CFileItemList& items,
                            const CDatabase::Filter& filter,
                            const SortDescription& sortDescription,
                            bool countOnly)
  {
    return true;
  }

  virtual bool GetSongs(const std::string& strBaseDir,
                        CFileItemList& items,
                        int idGenre,
                        int idArtist,
                        int idAlbum,
                        int idPlaylist,
                        const SortDescription& sortDescription = SortDescription())
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
    m_instanceData->toAddon.addonInstance = this;

    m_instanceData->toAddon.GetPlaylists = ADDON_GetPlaylists;
    m_instanceData->toAddon.GetSongs = ADDON_GetSongs;
  }

  inline static bool ADDON_GetPlaylists(AddonInstance_MetadataProvider* instance,
                                        const std::string& strBaseDir,
                                        CFileItemList& items,
                                        const CDatabase::Filter& filter,
                                        const SortDescription& sortDescription,
                                        bool countOnly)
  {
    return instance->toAddon.addonInstance->GetPlaylists(strBaseDir, items, filter, sortDescription,
                                                         countOnly);
  }

  inline static bool ADDON_GetSongs(AddonInstance_MetadataProvider* instance,
                                    const std::string& strBaseDir,
                                    CFileItemList& items,
                                    int idGenre,
                                    int idArtist,
                                    int idAlbum,
                                    int idPlaylist,
                                    const SortDescription& sortDescription)
  {
    return instance->toAddon.addonInstance->GetSongs(strBaseDir, items, idGenre, idArtist, idAlbum,
                                                     idPlaylist, sortDescription);
  }

  AddonInstance_MetadataProvider* m_instanceData;
};

} /* namespace addon */
} /* namespace kodi */
