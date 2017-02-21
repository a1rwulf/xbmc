//
//  ODBPersonLink.h
//  kodi
//
//  Created by Lukas Obermann on 04.10.16.
//
//

#ifndef ODBPERSONLINK_H
#define ODBPERSONLINK_H

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>

#include <string>

#include "ODBPerson.h"

#ifdef ODB_COMPILER
#pragma db model version(1, 1, open)
#endif

#pragma db object pointer(std::shared_ptr) \
                  table("person_link")
class CODBPersonLink
{
public:
  CODBPersonLink()
  {
    m_role = "";
    m_castOrder = 0;
    m_synced = false;
  };
  
#pragma db id auto
  unsigned long m_idPersonLink;
  odb::lazy_shared_ptr<CODBPerson> m_person;
  std::string m_role;
  int m_castOrder;
  
  //Members not stored in the db, used for sync ...
#pragma db transient
  bool m_synced;
  
private:
  friend class odb::access;
  
#pragma db index member(m_person)
  
};

#endif /* ODBPERSONLINK_H */
