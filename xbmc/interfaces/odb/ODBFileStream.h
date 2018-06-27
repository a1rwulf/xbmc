/*
*      Copyright (C) 2017 Team Kodi
*      https://kodi.tv
*
*  This Program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2, or (at your option)
*  any later version.
*
*  This Program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with XBMC; see the file COPYING.  If not, see
*  <http://www.gnu.org/licenses/>.
*
*/

#ifndef ODBFILESTREAM_H
#define ODBFILESTREAM_H

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/section.hxx>

#include <string>

#include "ODBFile.h"
#include "ODBLanguage.h"

PRAGMA_DB (model version(1, 1, open))

PRAGMA_DB (object pointer(std::shared_ptr) \
                  table("filestream"))
class CODBFileStream
{
public:
  CODBFileStream()
  {
    m_name = "";
    m_synced = false;
  };
  
PRAGMA_DB (id auto)
  unsigned long m_idFileStream;
  unsigned long m_stream;
PRAGMA_DB (type("VARCHAR(255)"))
  std::string m_name;
PRAGMA_DB (type("VARCHAR(16)"))
  std::string m_type;
PRAGMA_DB (section(section_foreign))
  odb::lazy_shared_ptr<CODBFile> m_file;
PRAGMA_DB (section(section_foreign))
  odb::lazy_shared_ptr<CODBLanguage> m_language;
  
PRAGMA_DB (load(lazy) update(change))
  odb::section section_foreign;

  //Members not stored in the db, used for sync ...
PRAGMA_DB (transient)
  bool m_synced;
  
private:
  friend class odb::access;
  
PRAGMA_DB (index member(m_name))
PRAGMA_DB (index member(m_type))
PRAGMA_DB (index member(m_file))
PRAGMA_DB (index member(m_language))
};

#endif /* ODBFILESTREAM_H */
