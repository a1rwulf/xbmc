/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

/*!
 \file Playlist.h
\brief
*/

#include <map>
#include <vector>
#include "Artist.h"
#include "Song.h"
#include "XBDateTime.h"
#include "utils/ScraperUrl.h"

class TiXmlNode;
class CFileItem;

class CMusicPlaylist
{
public:
  explicit CMusicPlaylist(const CFileItem& item);
  CMusicPlaylist() = default;
  bool operator<(const CMusicPlaylist &a) const;

  void Reset()
  {
    idPlaylist = -1;
    strPlaylist.clear();
    strThumb.clear();
    m_updatedAt.Reset();
    songs.clear();
  }

  long idPlaylist = -1;
  std::string strPlaylist;
  std::string strThumb;
  CDateTime m_updatedAt;
  VECSONGS songs;
};

typedef std::vector<CMusicPlaylist> VECPLAYLISTS;
