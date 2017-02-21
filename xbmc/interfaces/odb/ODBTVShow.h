//
//  ODBTVShow.h
//  kodi
//
//  Created by Lukas Obermann on 04.01.17.
//
//

#ifndef ODBTVSHOW_H
#define ODBTVSHOW_H

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/section.hxx>

#include <memory>
#include <string>
#include <vector>

#include "ODBRating.h"
#include "ODBGenre.h"
#include "ODBPersonLink.h"
#include "ODBStudio.h"
#include "ODBStreamDetails.h"
#include "ODBTag.h"
#include "ODBUniqueID.h"
#include "ODBArt.h"
#include "ODBDate.h"
#include "ODBSeason.h"
#include "ODBPath.h"

/*
 Cast
 Genre
 Studio
 Tag
 Director
 Rating
 UniqueID
 Season
 Art
 
 */

#ifdef ODB_COMPILER
#pragma db model version(1, 1, open)
#endif

#pragma db object pointer(std::shared_ptr) \
                  table("tvshow")

class CODBTVShow
{
public:
  CODBTVShow()
  {
    m_title = "";
    m_plot = "";
    m_status = "";
    m_thumbUrl = "";
    m_thumbUrl_spoof = "";
    m_originalTitle = "";
    m_episodeGuide = "";
    m_fanart = "";
    m_identID = "";
    m_mpaa = "";
    m_sortTitle = "";
    m_userrating = 0;
    m_runtime = 0;
    m_synced = false;
  };
  
#pragma db id auto
  unsigned long m_idTVShow;
#pragma db type("VARCHAR(255)")
  std::string m_title;
  std::string m_plot;
  std::string m_status;
  CODBDate m_premiered;
  std::string m_thumbUrl;
  std::string m_thumbUrl_spoof;
#pragma db type("VARCHAR(255)")
  std::string m_originalTitle;
  std::string m_episodeGuide;
  std::string m_fanart;
  std::string m_identID;
  std::string m_mpaa;
#pragma db type("VARCHAR(255)")
  std::string m_sortTitle;
  int m_userrating;
  int m_runtime;
  
#pragma db section(section_foreign)
  odb::lazy_shared_ptr<CODBRating> m_defaultRating;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBRating> > m_ratings;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBGenre> > m_genres;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBArt> > m_artwork;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBStudio> > m_studios;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBPersonLink> > m_actors;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBPersonLink> > m_directors;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBTag> > m_tags;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBUniqueID> > m_ids;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBSeason> > m_seasons;
#pragma db section(section_foreign)
  odb::lazy_shared_ptr<CODBUniqueID> m_defaultID;
#pragma db section(section_foreign)
  std::vector< odb::lazy_shared_ptr<CODBPath> > m_paths;
  
#pragma db load(lazy) update(change)
  odb::section section_foreign;
  
  //Members not stored in the db, used for sync ...
#pragma db transient
  bool m_synced;
  
private:
  friend class odb::access;
  
#pragma db index member(m_title)
#pragma db index member(m_originalTitle)
#pragma db index member(m_sortTitle)
#pragma db index member(m_userrating)
};

#pragma db view object(CODBTVShow) \
                object(CODBGenre = genre: CODBTVShow::m_genres) \
                object(CODBPersonLink = director_link: CODBTVShow::m_directors) \
                object(CODBPerson = director: director_link::m_person) \
                object(CODBPersonLink = actor_ink: CODBTVShow::m_actors) \
                object(CODBPerson = actor: actor_ink::m_person) \
                object(CODBStudio = studio: CODBTVShow::m_studios) \
                object(CODBTag = tag: CODBTVShow::m_tags) \
                object(CODBRating = defaultRating: CODBTVShow::m_defaultRating) \
                object(CODBPath = path: CODBTVShow::m_paths) \
                query(distinct)
struct ODBView_TVShow
{
  std::shared_ptr<CODBTVShow> show;
};

// ODBView_Season and ODBView_Episode are here to avoid forward declarations

#pragma db view object(CODBTVShow) \
                object(CODBSeason inner: CODBTVShow::m_seasons) \
                object(CODBEpisode inner: CODBSeason::m_episodes) \
                object(CODBFile: CODBEpisode::m_file) \
                query(distinct)
struct ODBView_Season
{
  std::shared_ptr<CODBTVShow> show;
  std::shared_ptr<CODBSeason> season;
  
#pragma db column("COUNT(DISTINCT " + CODBEpisode::m_idEpisode + ")")
  int episodesTotal;
  
#pragma db column("COUNT(" + CODBFile::m_playCount + ") AS playCount")
  int playCount;
};

#pragma db view object(CODBTVShow) \
                object(CODBSeason inner: CODBTVShow::m_seasons) \
                object(CODBEpisode inner: CODBSeason::m_episodes) \
                object(CODBGenre = genre: CODBTVShow::m_genres) \
                object(CODBPersonLink = director_link: CODBEpisode::m_directors) \
                object(CODBPerson = director: director_link::m_person) \
                object(CODBPersonLink = actor_ink: CODBEpisode::m_actors) \
                object(CODBPerson = actor: actor_ink::m_person) \
                object(CODBPersonLink = writingCredit_link: CODBEpisode::m_writingCredits) \
                object(CODBPerson = writingCredit: writingCredit_link::m_person) \
                object(CODBStudio = studio: CODBTVShow::m_studios) \
                object(CODBFile = fileView: CODBEpisode::m_file) \
                object(CODBPath = pathView: fileView::m_path) \
                object(CODBStreamDetails: CODBEpisode::m_file == CODBStreamDetails::m_file) \
                object(CODBRating = defaultRating: CODBEpisode::m_defaultRating) \
                object(CODBTag = tag: CODBTVShow::m_tags) \
                query(distinct)
struct ODBView_Episode
{
  std::shared_ptr<CODBEpisode> episode;
};

#pragma db view object(CODBTVShow) \
                object(CODBPath = path inner: CODBTVShow::m_paths) \
                query(distinct)
struct ODBView_TVShow_Path
{
  std::shared_ptr<CODBTVShow> show;
  std::shared_ptr<CODBPath> path;
};

#pragma db view object(CODBTVShow) \
                object(CODBUniqueID: CODBTVShow::m_ids) \
                query(distinct)
struct ODBView_TVShow_UID
{
  std::shared_ptr<CODBTVShow> show;
  std::shared_ptr<CODBUniqueID> uid;
};

#pragma db view object(CODBTVShow) \
                object(CODBSeason inner: CODBTVShow::m_seasons) \
                query(distinct)
struct ODBView_TVShow_Seasons
{
  std::shared_ptr<CODBTVShow> show;
  std::shared_ptr<CODBSeason> season;
};

#pragma db view object(CODBTVShow) \
                object(CODBSeason: CODBTVShow::m_seasons) \
                object(CODBEpisode: CODBSeason::m_episodes) \
                query(distinct)
struct ODBView_TVShow_Season_Episodes
{
  std::shared_ptr<CODBTVShow> show;
  std::shared_ptr<CODBSeason> season;
  std::shared_ptr<CODBEpisode> episode;
};

#pragma db view object(CODBTVShow) \
                object(CODBSeason: CODBTVShow::m_seasons) \
                object(CODBEpisode: CODBSeason::m_episodes) \
                object(CODBFile: CODBEpisode::m_file) \
                query(distinct)
struct ODBView_TVShow_Counts
{
#pragma db column(CODBTVShow::m_idTVShow)
  unsigned long m_idTVShow;
  
#pragma db column("MAX(" + CODBFile::m_lastPlayed.m_ulong_date + ")")
  unsigned long lastPlayedULong;
  
#pragma db column("NULLIF(COUNT(" + CODBSeason::m_season + "), 0)")
  unsigned int totalCount;
  
#pragma db column("COUNT(" + CODBFile::m_playCount + ")")
  unsigned int watchedCount;
  
#pragma db column("NULLIF(COUNT(DISTINCT(" + CODBSeason::m_season + ")), 0)")
  unsigned int totalSeasons;
  
#pragma db column("MAX(" + CODBFile::m_dateAdded.m_ulong_date + ")")
  unsigned long dateAddedULong;
};

#pragma db view object(CODBTVShow) \
                object(CODBArt: CODBTVShow::m_artwork) \
                query(distinct)
struct ODBView_TVShow_Art
{
  std::shared_ptr<CODBArt> art;
};

#pragma db view object(CODBTVShow) \
                object(CODBGenre = genre inner: CODBTVShow::m_genres) \
                object(CODBPath = path: CODBTVShow::m_paths) \
                query(distinct)
struct ODBView_TVShow_Genre
{
#pragma db column(genre::m_idGenre)
  unsigned long m_idGenre;
#pragma db column(genre::m_name)
  std::string m_name;
#pragma db column(path::m_path)
  std::string m_path;
};

#pragma db view object(CODBTVShow) \
                object(CODBStudio = studio inner: CODBTVShow::m_studios) \
                object(CODBPath = path: CODBTVShow::m_paths) \
                query(distinct)
struct ODBView_TVShow_Studio
{
#pragma db column(studio::m_idStudio)
  unsigned long m_idStudio;
#pragma db column(studio::m_name)
  std::string m_name;
#pragma db column(path::m_path)
  std::string m_path;
};

#pragma db view object(CODBTVShow) \
                object(CODBTag = tag inner: CODBTVShow::m_tags) \
                object(CODBPath = path: CODBTVShow::m_paths) \
                query(distinct)
struct ODBView_TVShow_Tag
{
#pragma db column(tag::m_idTag)
  unsigned long m_idTag;
#pragma db column(tag::m_name)
  std::string m_name;
#pragma db column(path::m_path)
  std::string m_path;
};

#pragma db view object(CODBTVShow) \
                object(CODBPersonLink = person_link inner: CODBTVShow::m_directors) \
                object(CODBPerson = person inner: person_link::m_person) \
                object(CODBPath = path: CODBTVShow::m_paths) \
                query(distinct)
struct ODBView_TVShow_Director
{
  std::shared_ptr<CODBPerson> person;
#pragma db column(path::m_path)
  std::string m_path;
};

#pragma db view object(CODBTVShow) \
                object(CODBPersonLink = person_link inner: CODBTVShow::m_actors) \
                object(CODBPerson = person inner: person_link::m_person) \
                object(CODBPath = path inner: CODBTVShow::m_paths) \
                query(distinct)
struct ODBView_TVShow_Actor
{
  std::shared_ptr<CODBPerson> person;
#pragma db column(path::m_path)
  std::string m_path;
};

#pragma db view object(CODBTVShow)
struct ODBView_TVShow_Count
{
#pragma db column("count(1)")
  std::size_t count;
};

#endif /* ODBTVSHOW_H */
