//
//  ODBPerson.h
//  kodi
//
//  Created by Lukas Obermann on 03.10.16.
//
//

#ifndef ODBPERSON_H
#define ODBPERSON_H

#include <odb/core.hxx>

#include "ODBArt.h"

#include <string>

#ifdef ODB_COMPILER
#pragma db model version(1, 1, open)
#endif

#pragma db object pointer(std::shared_ptr) \
                  table("person")
class CODBPerson
{
public:
  CODBPerson()
  {
    m_name = "";
    m_synced = false;
  };
  
#pragma db id auto
  unsigned long m_idPerson;
#pragma db type("VARCHAR(255)")
  std::string m_name;
  odb::lazy_shared_ptr<CODBArt> m_art;
  
  //Members not stored in the db, used for sync ...
#pragma db transient
  bool m_synced;
  
private:
  friend class odb::access;
  
#pragma db index member(m_name)
  
};

#endif /* ODBPERSON_H */
