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
#include <odb/section.hxx>
#include <string>
#include <vector>
#include "ODBArt.h"

class CODBSong;

PRAGMA_DB (model version(1, 1, open))

PRAGMA_DB (object pointer(std::shared_ptr) \
                  table("playlist"))
class CODBPlaylist {
public:
  CODBPlaylist() = default;
PRAGMA_DB (id auto)
    unsigned long m_idPlaylist;

    std::string m_name;
    std::string m_description;
    unsigned long m_updatedAt;

PRAGMA_DB (section(playlist_songs))
    std::vector< odb::lazy_ptr<CODBSong> > m_songs;
PRAGMA_DB (section(playlist_art))
    std::vector< odb::lazy_shared_ptr<CODBArt> > m_artwork;

PRAGMA_DB (load(lazy) update(change))
    odb::section playlist_songs;
PRAGMA_DB (load(lazy) update(change))
    odb::section playlist_art;

//Members not stored in the db, used for sync ...
PRAGMA_DB (transient)
    bool m_synced;

private:
    friend class odb::access;
};

PRAGMA_DB (view object(CODBPlaylist) \
           object(CODBArt: CODBPlaylist::m_artwork) \
           table("playlist_songs" = "ps" inner: "ps.object_id = " + CODBPlaylist::m_idPlaylist) \
           query(distinct))
struct ODBView_Playlist
{
  std::shared_ptr<CODBPlaylist> playlist;
  PRAGMA_DB(column("(SELECT COUNT(object_id) FROM playlist_songs WHERE playlist_songs.object_id=" + CODBPlaylist::m_idPlaylist + ")"))
  unsigned int size;
};

PRAGMA_DB (view object(CODBPlaylist) \
           object(CODBArt: CODBPlaylist::m_artwork) \
           table("playlist_songs" = "ps" inner: "ps.object_id = " + CODBPlaylist::m_idPlaylist) \
           query(distinct))
struct ODBView_Playlist_Total
{
  PRAGMA_DB (column("COUNT(DISTINCT(" + CODBPlaylist::m_idPlaylist + "))"))
  unsigned int total;
};

PRAGMA_DB (view object(CODBPlaylist) \
                object(CODBArt: CODBPlaylist::m_artwork) \
                query(distinct))
struct ODBView_Playlist_Art
{
  PRAGMA_DB (column(CODBPlaylist::m_idPlaylist))
  unsigned long id;
  std::shared_ptr<CODBArt> art;
};

#ifdef ODB_COMPILER
#include "ODBSong.h"
#endif

#endif //ODBPLAYLIST_H
