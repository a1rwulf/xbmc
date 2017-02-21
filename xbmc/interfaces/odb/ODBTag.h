//
//  ODBTag.h
//  kodi
//
//  Created by Lukas Obermann on 07.10.16.
//
//

#ifndef ODBTAG_H
#define ODBTAG_H

#include <odb/core.hxx>

#include <string>

#ifdef ODB_COMPILER
#pragma db model version(1, 1, open)
#endif

#pragma db object pointer(std::shared_ptr) \
                  table("tag")
class CODBTag
{
public:
  CODBTag()
  {
    m_name = "";
    m_synced = false;
  };
  
#pragma db id auto
  unsigned long m_idTag;
#pragma db type("VARCHAR(255)")
  std::string m_name;
  
  //Members not stored in the db, used for sync ...
#pragma db transient
  bool m_synced;
  
private:
  friend class odb::access;

#pragma db index member(m_name)
};

#endif /* ODBTAG_H */
