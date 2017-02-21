//
//  ODBBookmark.h
//  kodi
//
//  Created by Lukas Obermann on 03.10.16.
//
//

#ifndef ODBBOOKMARK_H
#define ODBBOOKMARK_H

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>

#include <string>

#include "ODBFile.h"

#ifdef ODB_COMPILER
#pragma db model version(1, 1, open)
#endif

#pragma db object pointer(std::shared_ptr) \
                  table("bookmark")
class CODBBookmark
{
public:
  CODBBookmark()
  {
    m_timeInSeconds = 0;
    m_totalTimeInSeconds = 0;
    m_thumbNailImage = "";
    m_player = "";
    m_type = 0;
    m_synced = false;
  };
  
#pragma db id auto
  unsigned long m_idBookmark;
  odb::lazy_shared_ptr<CODBFile> m_file;
  double m_timeInSeconds;
  double m_totalTimeInSeconds;
  std::string m_thumbNailImage;
  std::string m_player;
  std::string m_playerState;
  int m_type;
  
  //Members not stored in the db, used for sync ...
#pragma db transient
  bool m_synced;
  
private:
  friend class odb::access;
  
#pragma db index member(m_file)
#pragma db index member(m_type)
  
};

#endif /* ODBBOOKMARK_H */
