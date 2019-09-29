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
 : IAddonInstanceHandler(ADDON_INSTANCE_SCREENSAVER, addonBase)
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

} /* namespace ADDON */
