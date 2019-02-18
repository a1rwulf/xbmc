/*
 *  Copyright (C) 2016-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef ODBPLAYLIST_H
#define ODBPLAYLIST_H

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <string>
#include <vector>

#include "ODBArt.h"

class CODBSong;

PRAGMA_DB (model version(1, 1, open))

PRAGMA_DB (object pointer(std::shared_ptr) \
                  table("playlist")
                  session)
class CODBPlaylist {
public:
  CODBPlaylist() = default;
PRAGMA_DB (id auto)
    unsigned long m_idPlaylist;

    std::string m_name;
    std::string m_description;
    unsigned long m_updatedAt;

    std::vector< odb::lazy_ptr<CODBSong> > m_songs;
    std::vector< odb::lazy_shared_ptr<CODBArt> > m_artwork;

//Members not stored in the db, used for sync ...
PRAGMA_DB (transient)
    bool m_synced;

private:
    friend class odb::access;
};

#ifdef ODB_COMPILER
#include "ODBSong.h"
#endif

PRAGMA_DB (view object(CODBPlaylist) \
           object(CODBArt inner: CODBPlaylist::m_artwork)
               query(distinct))
struct ODBView_Playlist
{
  std::shared_ptr<CODBPlaylist> playlist;
};

#endif //ODBPLAYLIST_H
