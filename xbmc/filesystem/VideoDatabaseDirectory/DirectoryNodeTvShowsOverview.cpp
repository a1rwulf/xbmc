/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DirectoryNodeTvShowsOverview.h"
#include "FileItem.h"
#include "guilib/LocalizeStrings.h"
#include "video/VideoDbUrl.h"
#include "utils/StringUtils.h"

using namespace XFILE::VIDEODATABASEDIRECTORY;

XFILE::MEDIADIRECTORY::Node TvShowChildren[] = {
                          { XFILE::MEDIADIRECTORY::NODE_TYPE_GENRE,         "genres",   135 },
                          { XFILE::MEDIADIRECTORY::NODE_TYPE_TITLE_TVSHOWS, "titles",   10024 },
                          { XFILE::MEDIADIRECTORY::NODE_TYPE_YEAR,          "years",    652 },
                          { XFILE::MEDIADIRECTORY::NODE_TYPE_ACTOR,         "actors",   344 },
                          { XFILE::MEDIADIRECTORY::NODE_TYPE_STUDIO,        "studios",  20388 },
                          { XFILE::MEDIADIRECTORY::NODE_TYPE_TAGS,          "tags",     20459 }
                        };

CDirectoryNodeTvShowsOverview::CDirectoryNodeTvShowsOverview(const std::string& strName, XFILE::MEDIADIRECTORY::CDirectoryNode* pParent, const std::string& strOrigin)
  : CDirectoryNode(XFILE::MEDIADIRECTORY::NODE_TYPE_TVSHOWS_OVERVIEW, strName, pParent, strOrigin)
{

}

XFILE::MEDIADIRECTORY::NODE_TYPE CDirectoryNodeTvShowsOverview::GetChildType() const
{
  if (GetName()=="0")
    return XFILE::MEDIADIRECTORY::NODE_TYPE_EPISODES;

  for (const XFILE::MEDIADIRECTORY::Node& node : TvShowChildren)
    if (GetName() == node.id)
      return node.node;

  return XFILE::MEDIADIRECTORY::NODE_TYPE_NONE;
}

std::string CDirectoryNodeTvShowsOverview::GetLocalizedName() const
{
  for (const XFILE::MEDIADIRECTORY::Node& node : TvShowChildren)
    if (GetName() == node.id)
      return g_localizeStrings.Get(node.label);
  return "";
}

bool CDirectoryNodeTvShowsOverview::GetContent(CFileItemList& items) const
{
  CVideoDbUrl videoUrl;
  if (!videoUrl.FromString(BuildPath()))
    return false;

  for (const XFILE::MEDIADIRECTORY::Node& node : TvShowChildren)
  {
    CFileItemPtr pItem(new CFileItem(g_localizeStrings.Get(node.label)));

    CVideoDbUrl itemUrl = videoUrl;
    std::string strDir = StringUtils::Format("%s/", node.id.c_str());
    itemUrl.AppendPath(strDir);
    pItem->SetPath(itemUrl.ToString());

    pItem->m_bIsFolder = true;
    pItem->SetCanQueue(false);
    items.Add(pItem);
  }

  return true;
}
