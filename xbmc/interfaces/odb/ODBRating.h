//
//  ODBRating.h
//  kodi
//
//  Created by Lukas Obermann on 03.10.16.
//
//

#ifndef ODBRATING_H
#define ODBRATING_H

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>

#include <string>

#ifdef ODB_COMPILER
#pragma db model version(1, 1, open)
#endif

#pragma db object pointer(std::shared_ptr) \
                  table("rating")
class CODBRating
{
public:
  CODBRating()
  {
    m_ratingType = "";
    m_rating = 0.0f;
    m_votes = 0;
    m_synced = false;
  };
  
#pragma db id auto
  unsigned long m_idRating;
#pragma db type("VARCHAR(255)")
  std::string m_ratingType;
  float m_rating;
  int m_votes;
  
  //Members not stored in the db, used for sync ...
#pragma db transient
  bool m_synced;
  
private:
  friend class odb::access;
  
#pragma db index member(m_ratingType)
#pragma db index member(m_rating)
  
};

#endif /* ODBRATING_H */
