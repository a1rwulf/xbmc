//
//  ODBStacktimes.h
//  kodi
//
//  Created by Lukas Obermann on 16.02.17.
//
//

#ifndef ODBSTACKTIME_H
#define ODBSTACKTIME_H

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>

#include <string>

#include "ODBFile.h"

#ifdef ODB_COMPILER
#pragma db model version(1, 1, open)
#endif

#pragma db object pointer(std::shared_ptr) \
                  table("stacktimes")
class CODBStacktime
{
public:
  CODBStacktime()
  {
    m_times = "";
  };
  
#pragma db id auto
  unsigned long m_idStacktime;
  odb::lazy_shared_ptr<CODBFile> m_file;
  std::string m_times;
  
private:
  friend class odb::access;
  
#pragma db index member(m_file)
  
};


#endif /* ODBSTACKTIME_H */
