/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DirectoryNode.h"
#include "utils/URIUtils.h"
#include "QueryParams.h"
#include "DirectoryNodeRoot.h"
#include "DirectoryNodeOverview.h"
#include "DirectoryNodeGrouped.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeArtist.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeAlbum.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeSong.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyAdded.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyAddedSong.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyPlayed.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyPlayedSong.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeTop100.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeSongTop100.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumTop100.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumTop100Song.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumCompilations.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumCompilationsSongs.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeYearAlbum.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeYearSong.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodeSingles.h"
#include "filesystem/MusicDatabaseDirectory/DirectoryNodePlaylist.h"
#include "filesystem/VideoDatabaseDirectory/DirectoryNodeTitleMovies.h"
#include "filesystem/VideoDatabaseDirectory/DirectoryNodeTitleTvShows.h"
#include "filesystem/VideoDatabaseDirectory/DirectoryNodeMoviesOverview.h"
#include "filesystem/VideoDatabaseDirectory/DirectoryNodeTvShowsOverview.h"
#include "filesystem/VideoDatabaseDirectory/DirectoryNodeSeasons.h"
#include "filesystem/VideoDatabaseDirectory/DirectoryNodeEpisodes.h"
#include "filesystem/VideoDatabaseDirectory/DirectoryNodeInProgressTvShows.h"
#include "filesystem/VideoDatabaseDirectory/DirectoryNodeRecentlyAddedMovies.h"
#include "filesystem/VideoDatabaseDirectory/DirectoryNodeRecentlyAddedEpisodes.h"
#include "filesystem/VideoDatabaseDirectory/DirectoryNodeMusicVideosOverview.h"
#include "filesystem/VideoDatabaseDirectory/DirectoryNodeRecentlyAddedMusicVideos.h"
#include "filesystem/VideoDatabaseDirectory/DirectoryNodeTitleMusicVideos.h"
#include "URL.h"
#include "FileItem.h"
#include "utils/StringUtils.h"

using namespace XFILE::MEDIADIRECTORY;

//  Constructor is protected use ParseURL()
CDirectoryNode::CDirectoryNode(NODE_TYPE Type, const std::string& strName, CDirectoryNode* pParent, const std::string& strOrigin)
{
  m_Type=Type;
  m_strName=strName;
  m_pParent=pParent;
  m_origin = strOrigin;
}

CDirectoryNode::~CDirectoryNode()
{
  delete m_pParent;
}

//  Parses a given path and returns the current node of the path
CDirectoryNode* CDirectoryNode::ParseURL(const std::string& strPath)
{
  CURL url(strPath);

  std::string strDirectory=url.GetFileName();
  URIUtils::RemoveSlashAtEnd(strDirectory);

  std::vector<std::string> Path = StringUtils::Split(strDirectory, '/');
  Path.insert(Path.begin(), "");

  CDirectoryNode* pNode = nullptr;
  CDirectoryNode* pParent = nullptr;
  NODE_TYPE NodeType = NODE_TYPE_ROOT;

  for (int i=0; i < static_cast<int>(Path.size()); ++i)
  {
    pNode = CreateNode(NodeType, Path[i], pParent, url.GetProtocol());
    NodeType = pNode ? pNode->GetChildType() : NODE_TYPE_NONE;
    pParent = pNode;
  }

  // Add all the additional URL options to the last node
  if (pNode)
    pNode->AddOptions(url.GetOptions());

  return pNode;
}

//  returns the database ids of the path,
void CDirectoryNode::GetDatabaseInfo(const std::string& strPath, CQueryParams& params)
{
  std::unique_ptr<CDirectoryNode> pNode(CDirectoryNode::ParseURL(strPath));

  if (!pNode.get())
    return;

  pNode->CollectQueryParams(params);
}

//  Create a node object
CDirectoryNode* CDirectoryNode::CreateNode(NODE_TYPE Type, const std::string& strName, CDirectoryNode* pParent, const std::string& strOrigin)
{
  switch (Type)
  {
  case NODE_TYPE_ROOT:
    return new CDirectoryNodeRoot(strName, pParent, strOrigin);
  case NODE_TYPE_OVERVIEW:
    return new CDirectoryNodeOverview(strName, pParent, strOrigin);
  case NODE_TYPE_GENRE:
  case NODE_TYPE_SOURCE:
  case NODE_TYPE_ROLE:
  case NODE_TYPE_YEAR:
  case NODE_TYPE_COUNTRY:
  case NODE_TYPE_SETS:
  case NODE_TYPE_TAGS:
  case NODE_TYPE_ACTOR:
  case NODE_TYPE_DIRECTOR:
  case NODE_TYPE_STUDIO:
  case NODE_TYPE_MUSICVIDEOS_ALBUM:
    return new CDirectoryNodeGrouped(Type, strName, pParent, strOrigin);
  case NODE_TYPE_ARTIST:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeArtist(strName, pParent, strOrigin);
  case NODE_TYPE_ALBUM:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeAlbum(strName, pParent, strOrigin);
  case NODE_TYPE_SONG:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeSong(strName, pParent, strOrigin);
  case NODE_TYPE_SINGLES:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeSingles(strName, pParent, strOrigin);
  case NODE_TYPE_TOP100:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeTop100(strName, pParent, strOrigin);
  case NODE_TYPE_ALBUM_TOP100:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeAlbumTop100(strName, pParent, strOrigin);
  case NODE_TYPE_ALBUM_TOP100_SONGS:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeAlbumTop100Song(strName, pParent, strOrigin);
  case NODE_TYPE_SONG_TOP100:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeSongTop100(strName, pParent, strOrigin);
  case NODE_TYPE_ALBUM_RECENTLY_ADDED:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeAlbumRecentlyAdded(strName, pParent, strOrigin);
  case NODE_TYPE_ALBUM_RECENTLY_ADDED_SONGS:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeAlbumRecentlyAddedSong(strName, pParent, strOrigin);
  case NODE_TYPE_ALBUM_RECENTLY_PLAYED:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeAlbumRecentlyPlayed(strName, pParent, strOrigin);
  case NODE_TYPE_ALBUM_RECENTLY_PLAYED_SONGS:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeAlbumRecentlyPlayedSong(strName, pParent, strOrigin);
  case NODE_TYPE_ALBUM_COMPILATIONS:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeAlbumCompilations(strName, pParent, strOrigin);
  case NODE_TYPE_ALBUM_COMPILATIONS_SONGS:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeAlbumCompilationsSongs(strName, pParent, strOrigin);
  case NODE_TYPE_YEAR_ALBUM:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeYearAlbum(strName, pParent, strOrigin);
  case NODE_TYPE_YEAR_SONG:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodeYearSong(strName, pParent, strOrigin);
  case NODE_TYPE_PLAYLIST:
    return new XFILE::MUSICDATABASEDIRECTORY::CDirectoryNodePlaylist(strName, pParent, strOrigin);
  case NODE_TYPE_TITLE_MOVIES:
    return new XFILE::VIDEODATABASEDIRECTORY::CDirectoryNodeTitleMovies(strName, pParent, strOrigin);
  case NODE_TYPE_TITLE_TVSHOWS:
    return new XFILE::VIDEODATABASEDIRECTORY::CDirectoryNodeTitleTvShows(strName, pParent, strOrigin);
  case NODE_TYPE_MOVIES_OVERVIEW:
    return new XFILE::VIDEODATABASEDIRECTORY::CDirectoryNodeMoviesOverview(strName, pParent, strOrigin);
  case NODE_TYPE_TVSHOWS_OVERVIEW:
    return new XFILE::VIDEODATABASEDIRECTORY::CDirectoryNodeTvShowsOverview(strName, pParent, strOrigin);
  case NODE_TYPE_SEASONS:
    return new XFILE::VIDEODATABASEDIRECTORY::CDirectoryNodeSeasons(strName, pParent, strOrigin);
  case NODE_TYPE_EPISODES:
    return new XFILE::VIDEODATABASEDIRECTORY::CDirectoryNodeEpisodes(strName, pParent, strOrigin);
  case NODE_TYPE_RECENTLY_ADDED_MOVIES:
    return new XFILE::VIDEODATABASEDIRECTORY::CDirectoryNodeRecentlyAddedMovies(strName,pParent, strOrigin);
  case NODE_TYPE_RECENTLY_ADDED_EPISODES:
    return new XFILE::VIDEODATABASEDIRECTORY::CDirectoryNodeRecentlyAddedEpisodes(strName,pParent, strOrigin);
  case NODE_TYPE_MUSICVIDEOS_OVERVIEW:
    return new XFILE::VIDEODATABASEDIRECTORY::CDirectoryNodeMusicVideosOverview(strName,pParent, strOrigin);
  case NODE_TYPE_RECENTLY_ADDED_MUSICVIDEOS:
    return new XFILE::VIDEODATABASEDIRECTORY::CDirectoryNodeRecentlyAddedMusicVideos(strName,pParent, strOrigin);
  case NODE_TYPE_INPROGRESS_TVSHOWS:
    return new XFILE::VIDEODATABASEDIRECTORY::CDirectoryNodeInProgressTvShows(strName,pParent, strOrigin);
  case NODE_TYPE_TITLE_MUSICVIDEOS:
    return new XFILE::VIDEODATABASEDIRECTORY::CDirectoryNodeTitleMusicVideos(strName,pParent, strOrigin);
  default:
    break;
  }

  return nullptr;
}

//  Current node name
const std::string& CDirectoryNode::GetName() const
{
  return m_strName;
}

int CDirectoryNode::GetID() const
{
  return atoi(m_strName.c_str());
}

std::string CDirectoryNode::GetLocalizedName() const
{
  return "";
}

//  Current node type
NODE_TYPE CDirectoryNode::GetType() const
{
  return m_Type;
}

//  Return the parent directory node or NULL, if there is no
CDirectoryNode* CDirectoryNode::GetParent() const
{
  return m_pParent;
}

void CDirectoryNode::RemoveParent()
{
  m_pParent = nullptr;
}

//  should be overloaded by a derived class
//  to get the content of a node. Will be called
//  by GetChilds() of a parent node
bool CDirectoryNode::GetContent(CFileItemList& items) const
{
  return false;
}

//  Creates a musicdb url
std::string CDirectoryNode::BuildPath() const
{
  std::vector<std::string> array;

  if (!m_strName.empty())
    array.insert(array.begin(), m_strName);

  CDirectoryNode* pParent=m_pParent;
  while (pParent != nullptr)
  {
    const std::string& strNodeName=pParent->GetName();
    if (!strNodeName.empty())
      array.insert(array.begin(), strNodeName);

    pParent=pParent->GetParent();
  }
  //! @todo a1rwulf - check how to differ between video and music
  std::string strPath = m_origin.empty() ? "videodb://" : m_origin + "://";
  for (int i = 0; i < static_cast<int>(array.size()); ++i)
    strPath+=array[i]+"/";

  std::string options = m_options.GetOptionsString();
  if (!options.empty())
    strPath += "?" + options;

  return strPath;
}

void CDirectoryNode::AddOptions(const std::string &options)
{
  if (options.empty())
    return;

  m_options.AddOptions(options);
}

//  Collects Query params from this and all parent nodes. If a NODE_TYPE can
//  be used as a database parameter, it will be added to the
//  params object.
void CDirectoryNode::CollectQueryParams(CQueryParams& params) const
{
  params.SetQueryParam(m_Type, m_strName);

  CDirectoryNode* pParent=m_pParent;
  while (pParent != nullptr)
  {
    params.SetQueryParam(pParent->GetType(), pParent->GetName());
    pParent=pParent->GetParent();
  }
}

//  Should be overloaded by a derived class.
//  Returns the NODE_TYPE of the child nodes.
NODE_TYPE CDirectoryNode::GetChildType() const
{
  return NODE_TYPE_NONE;
}

//  Get the child fileitems of this node
bool CDirectoryNode::GetChilds(CFileItemList& items)
{
  if (CanCache() && items.Load())
    return true;

  std::unique_ptr<CDirectoryNode> pNode(CDirectoryNode::CreateNode(GetChildType(), "", this, m_origin));

  bool bSuccess=false;
  if (pNode.get())
  {
    pNode->m_options = m_options;
    bSuccess=pNode->GetContent(items);
    if (bSuccess)
    {
      if (CanCache())
        items.SetCacheToDisc(CFileItemList::CACHE_ALWAYS);
    }
    else
      items.Clear();

    pNode->RemoveParent();
  }

  return bSuccess;
}

bool CDirectoryNode::CanCache() const
{
  // JM: No need to cache these views, as caching is added in the mediawindow baseclass for anything that takes
  //     longer than a second
  return false;
}
