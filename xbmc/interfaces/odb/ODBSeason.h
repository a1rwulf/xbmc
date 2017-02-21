//
//  ODBSeason.h
//  kodi
//
//  Created by Lukas Obermann on 04.01.17.
//
//

#ifndef ODBSEASON_H
#define ODBSEASON_H

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/section.hxx>

#include <memory>
#include <string>
#include <vector>

#include "ODBEpisode.h"
#include "ODBArt.h"

#ifdef ODB_COMPILER
#pragma db model version(1, 1, open)
#endif

#pragma db object pointer(std::shared_ptr) \
                  table("season")
class CODBSeason
{
public:
  CODBSeason()
  {
    m_name = "";
    m_season = -1;
    m_userrating = 0;
    m_synced = false;
  }
  
#pragma db id auto
  unsigned long m_idSeason;
#pragma db type("VARCHAR(255)")
  std::string m_name;
  int m_season;
  int m_userrating;
  CODBDate m_firstAired;
  
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBEpisode> > m_episodes;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBArt> > m_artwork;
  
#pragma db load(lazy) update(change)
  odb::section section_foreign;
  
  //Members not stored in the db, used for sync ...
#pragma db transient
  bool m_synced;
  
private:
  friend class odb::access;
  
#pragma db index member(m_name)
};

#pragma db view object(CODBSeason) \
                object(CODBEpisode: CODBSeason::m_episodes) \
                object(CODBFile: CODBEpisode::m_file) \
                query(distinct)
struct ODBView_Season_Episodes
{
  std::shared_ptr<CODBSeason> season;
  std::shared_ptr<CODBEpisode> episode;
};

#pragma db view object(CODBSeason) \
                object(CODBArt: CODBSeason::m_artwork) \
                query(distinct)
struct ODBView_Season_Art
{
  std::shared_ptr<CODBArt> art;
};


#endif /* ODBSEASON_H */
