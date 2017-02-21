//
//  ODBUniqueID.h
//  kodi
//
//  Created by Lukas Obermann on 03.10.16.
//
//

#ifndef ODBUNIQUEID_H
#define ODBUNIQUEID_H

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>

#include <string>

#ifdef ODB_COMPILER
#pragma db model version(1, 1, open)
#endif

#pragma db object pointer(std::shared_ptr) \
                  table("uniqueid")
class CODBUniqueID
{
public:
  CODBUniqueID()
  {
    m_value = "";
    m_type = "";
    m_synced = false;
  };
  
#pragma db id auto
  unsigned long m_idUniqueID;
#pragma db type("VARCHAR(255)")
  std::string m_value;
#pragma db type("VARCHAR(255)")
  std::string m_type;
  
  //Members not stored in the db, used for sync ...
#pragma db transient
  bool m_synced;
  
private:
  friend class odb::access;
  
#pragma db index member(m_type)
#pragma db index member(m_value)
};

#endif /* ODBUNIQUEID_H */
