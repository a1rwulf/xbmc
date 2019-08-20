/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DirectoryNodeMusicVideosOverview.h"
#include "FileItem.h"
#include "guilib/LocalizeStrings.h"
#include "video/VideoDbUrl.h"
#include "utils/StringUtils.h"

using namespace XFILE::VIDEODATABASEDIRECTORY;

XFILE::MEDIADIRECTORY::Node MusicVideoChildren[] = {
                              { XFILE::MEDIADIRECTORY::NODE_TYPE_GENRE,             "genres",    135 },
                              { XFILE::MEDIADIRECTORY::NODE_TYPE_TITLE_MUSICVIDEOS, "titles",    10024 },
                              { XFILE::MEDIADIRECTORY::NODE_TYPE_YEAR,              "years",     652 },
                              { XFILE::MEDIADIRECTORY::NODE_TYPE_ACTOR,             "artists",   133 },
                              { XFILE::MEDIADIRECTORY::NODE_TYPE_MUSICVIDEOS_ALBUM, "albums",    132 },
                              { XFILE::MEDIADIRECTORY::NODE_TYPE_DIRECTOR,          "directors", 20348 },
                              { XFILE::MEDIADIRECTORY::NODE_TYPE_STUDIO,            "studios",   20388 },
                              { XFILE::MEDIADIRECTORY::NODE_TYPE_TAGS,              "tags",      20459 }
                            };

CDirectoryNodeMusicVideosOverview::CDirectoryNodeMusicVideosOverview(const std::string& strName, XFILE::MEDIADIRECTORY::CDirectoryNode* pParent, const std::string& strOrigin)
  : CDirectoryNode(XFILE::MEDIADIRECTORY::NODE_TYPE_MUSICVIDEOS_OVERVIEW, strName, pParent, strOrigin)
{

}

XFILE::MEDIADIRECTORY::NODE_TYPE CDirectoryNodeMusicVideosOverview::GetChildType() const
{
  for (const XFILE::MEDIADIRECTORY::Node& node : MusicVideoChildren)
    if (GetName() == node.id)
      return node.node;

  return XFILE::MEDIADIRECTORY::NODE_TYPE_NONE;
}

std::string CDirectoryNodeMusicVideosOverview::GetLocalizedName() const
{
  for (const XFILE::MEDIADIRECTORY::Node& node : MusicVideoChildren)
    if (GetName() == node.id)
      return g_localizeStrings.Get(node.label);
  return "";
}

bool CDirectoryNodeMusicVideosOverview::GetContent(CFileItemList& items) const
{
  CVideoDbUrl videoUrl;
  if (!videoUrl.FromString(BuildPath()))
    return false;

  for (const XFILE::MEDIADIRECTORY::Node& node : MusicVideoChildren)
  {
    CFileItemPtr pItem(new CFileItem(g_localizeStrings.Get(node.label)));

    CVideoDbUrl itemUrl = videoUrl;
    std::string strDir = StringUtils::Format("%s/", node.id.c_str());
    itemUrl.AppendPath(strDir);
    pItem->SetPath(itemUrl.ToString());

    pItem->m_bIsFolder = true;
    pItem->SetCanQueue(false);
    pItem->SetSpecialSort(SortSpecialOnTop);
    items.Add(pItem);
  }

  return true;
}

