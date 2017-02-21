//
//  ODBSet.h
//  kodi
//
//  Created by Lukas Obermann on 03.10.16.
//
//

#ifndef ODBSET_H
#define ODBSET_H

#include <odb/core.hxx>
#include "ODBArt.h"

#include <map>
#include <string>

#ifdef ODB_COMPILER
#pragma db model version(1, 1, open)
#endif

#pragma db object pointer(std::shared_ptr) \
                  table("set")
class CODBSet
{
public:
  CODBSet()
  {
    m_name = "";
    m_overview = "";
    m_synced = false;
  };
  
#pragma db id auto
  unsigned long m_idSet;
#pragma db type("VARCHAR(255)")
  std::string m_name;
  std::string m_overview;
  std::vector< odb::lazy_shared_ptr<CODBArt> > m_artwork;
  
  //Members not stored in the db, used for sync ...
#pragma db transient
  bool m_synced;
  
private:
  friend class odb::access;
  
#pragma db index member(m_name)
};


#endif /* ODBSET_H */
