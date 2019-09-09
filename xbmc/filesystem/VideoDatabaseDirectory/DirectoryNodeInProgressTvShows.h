/*
 *  Copyright (C) 2016-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "filesystem/MediaDirectory/DirectoryNode.h"

namespace XFILE
{
  namespace VIDEODATABASEDIRECTORY
  {
    class CDirectoryNodeInProgressTvShows : public XFILE::MEDIADIRECTORY::CDirectoryNode
    {
    public:
      CDirectoryNodeInProgressTvShows(const std::string& strEntryName, XFILE::MEDIADIRECTORY::CDirectoryNode* pParent, const std::string& strOrigin);
    protected:
      XFILE::MEDIADIRECTORY::NODE_TYPE GetChildType() const override;
      bool GetContent(CFileItemList& items) const override;
      std::string GetLocalizedName() const override;
    };
  }
}


