/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "filesystem/MediaDirectory/DirectoryNode.h"

namespace XFILE
{
  namespace MUSICDATABASEDIRECTORY
  {
    class CDirectoryNodeSingles : public XFILE::MEDIADIRECTORY::CDirectoryNode
    {
    public:
      CDirectoryNodeSingles(const std::string& strName, XFILE::MEDIADIRECTORY::CDirectoryNode* pParent, const std::string& strOrigin);
    protected:
      bool GetContent(CFileItemList& items) const override;
    };
  }
}


