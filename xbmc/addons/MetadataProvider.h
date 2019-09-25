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

  bool Start();
  void Stop();
  void Render();

private:
  AddonInstance_Metadataprovider m_struct;
};

} /* namespace ADDON */
