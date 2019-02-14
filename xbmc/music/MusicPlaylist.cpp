/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "MusicPlaylist.h"
#include "music/tags/MusicInfoTag.h"
#include "settings/AdvancedSettings.h"
#include "settings/SettingsComponent.h"
#include "utils/StringUtils.h"
#include "utils/log.h"
#include "utils/XMLUtils.h"
#include "utils/MathUtils.h"
#include "FileItem.h"
#include "ServiceBroker.h"

#include <algorithm>

using namespace MUSIC_INFO;

CMusicPlaylist::CMusicPlaylist(const CFileItem& item)
{
  Reset();
  const CMusicInfoTag& tag = *item.GetMusicInfoTag();
  strPlaylist = tag.GetPlaylist();
  m_updatedAt.Reset();
}

bool CMusicPlaylist::operator<(const CMusicPlaylist &p) const
{
  if (strPlaylist < p.strPlaylist) return true;
  if (strPlaylist > p.strPlaylist) return false;

  return false;
}
