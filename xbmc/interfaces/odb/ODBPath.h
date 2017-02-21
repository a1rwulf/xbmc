//
//  ODBPath.h
//  kodi
//
//  Created by Lukas Obermann on 03.10.16.
//
//

#ifndef ODBPATH_H
#define ODBPATH_H

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>

#include <string>

#include "ODBDate.h"

#ifdef ODB_COMPILER
#pragma db model version(1, 1, open)
#endif

#pragma db object pointer(std::shared_ptr) \
                  table("path")
class CODBPath
{
public:
  CODBPath()
  {
    m_path = "";
    m_content = "";
    m_scraper = "";
    m_hash = "";
    m_scanRecursive = 0;
    m_useFolderNames = false;
    m_settings = "";
    m_noUpdate = false;
    m_exclude =false;
    m_synced = false;
  };
  
#pragma db id auto
  unsigned long m_idPath;
#pragma db type("VARCHAR(255)")
  std::string m_path;
#pragma db type("VARCHAR(255)")
  std::string m_content;
#pragma db type("VARCHAR(255)")
  std::string m_scraper;
#pragma db type("VARCHAR(255)")
  std::string m_hash;
  int m_scanRecursive;
  bool m_useFolderNames;
  std::string m_settings;
  bool m_noUpdate;
  bool m_exclude;
  CODBDate m_dateAdded;
  odb::lazy_shared_ptr<CODBPath> m_parentPath;

  //Members not stored in the db, used for sync ...
#pragma db transient
  bool m_synced;
  
private:
  friend class odb::access;
  
#pragma db index member(m_path)
#pragma db index member(m_content)
#pragma db index member(m_scraper)
#pragma db index member(m_hash)
#pragma db index member(m_scanRecursive)
#pragma db index member(m_noUpdate)
#pragma db index member(m_exclude)
#pragma db index member(m_dateAdded)
#pragma db index member(m_parentPath)
};


#endif /* ODBPATH_H */
