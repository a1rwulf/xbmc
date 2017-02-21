//
//  ODBEpisode.h
//  kodi
//
//  Created by Lukas Obermann on 04.01.17.
//
//

#ifndef ODBEPISODE_H
#define ODBEPISODE_H

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/section.hxx>

#include <memory>
#include <string>
#include <vector>

#include "ODBFile.h"
#include "ODBRating.h"
#include "ODBDate.h"
#include "ODBBookmark.h"
#include "ODBPersonLink.h"
#include "ODBUniqueID.h"
#include "ODBArt.h"
#include "ODBStreamDetails.h"

#ifdef ODB_COMPILER
#pragma db model version(1, 1, open)
#endif

#pragma db object pointer(std::shared_ptr) \
                  table("episode")

class CODBEpisode
{
public:
  CODBEpisode()
  {
    m_title = "";
    m_plot = "";
    m_thumbUrl = "";
    m_thumbUrl_spoofed = "";
    m_runtime = 0;
    m_productionCode = "";
    m_episode = -1;
    m_originalTitle = "";
    m_sortSeason = -1;
    m_sortEpisode = -1;
    m_identId = 0;
    m_userrating = 0;
    m_idShow = 0;
  }
  
#pragma db id auto
  unsigned long m_idEpisode;
#pragma db type("VARCHAR(255)")
  std::string m_title;
  std::string m_plot;
  CODBDate m_aired;
  std::string m_thumbUrl;
  std::string m_thumbUrl_spoofed;
  int m_runtime;
  std::string m_productionCode;
  int m_episode;
  std::string m_originalTitle;
  int m_sortSeason;
  int m_sortEpisode;
  int m_identId;
  int m_userrating;
  
  //Temporary Show ID as used in CVideoInfoScanner:1264
  //See if this can be refactored away when odb is fully integrated
  unsigned long m_idShow;
  
#pragma db section(section_foreign)
  odb::lazy_shared_ptr<CODBFile> m_file;
#pragma db section(section_foreign)
  odb::lazy_shared_ptr<CODBRating> m_defaultRating;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBRating> > m_ratings;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBPersonLink> > m_credits;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBPersonLink> > m_directors;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBPersonLink> > m_writingCredits;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBArt> > m_artwork;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBPersonLink> > m_actors;
#pragma db section(section_foreign)
  odb::lazy_shared_ptr<CODBPath> m_basepath;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBUniqueID> > m_ids;
#pragma db section(section_foreign)
  odb::lazy_shared_ptr<CODBUniqueID> m_defaultID;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBBookmark> > m_bookmarks;
#pragma db section(section_foreign)
  odb::lazy_shared_ptr<CODBBookmark> m_resumeBookmark;
  
#pragma db load(lazy) update(change)
  odb::section section_foreign;
  
  //Members not stored in the db, used for sync ...
#pragma db transient
  bool m_synced;
  
private:
  friend class odb::access;
  
#pragma db index member(m_title)
#pragma db index member(m_sortSeason)
#pragma db index member(m_sortEpisode)
#pragma db index member(m_identId)
#pragma db index member(m_userrating)
};

#pragma db view object(CODBEpisode) \
                object(CODBArt: CODBEpisode::m_artwork) \
                query(distinct)
struct ODBView_Episode_Art
{
  std::shared_ptr<CODBArt> art;
};

#endif /* ODBEPISODE_H */
