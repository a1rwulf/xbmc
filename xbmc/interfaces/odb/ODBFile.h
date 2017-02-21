//
//  ODBFile.h
//  kodi
//
//  Created by Lukas Obermann on 29.09.16.
//
//

#ifndef ODBFILE_H
#define ODBFILE_H

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>

#include <string>

#include "ODBPath.h"
#include "ODBDate.h"

#ifdef ODB_COMPILER
#pragma db model version(1, 1, open)
#endif

#pragma db object pointer(std::shared_ptr) \
                  table("file")
class CODBFile
{
public:
  CODBFile()
  {
    m_filename = "";
    m_playCount = 0;
    m_synced = false;
  };
  
#pragma db id auto
  unsigned long m_idFile;
  odb::lazy_shared_ptr<CODBPath> m_path;
#pragma db type("VARCHAR(255)")
  std::string m_filename;
  unsigned int m_playCount;
  CODBDate m_lastPlayed;
  CODBDate m_dateAdded;
  //Members not stored in the db, used for sync ...
#pragma db transient
  bool m_synced;
private:
    friend class odb::access;

#pragma db index member(m_path)
#pragma db index member(m_filename)
#pragma db index member(m_dateAdded)
};


#endif /* ODBFile_h */
