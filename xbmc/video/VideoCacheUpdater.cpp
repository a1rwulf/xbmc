//
//  VideoCacheUpdater.cpp
//  kodi
//
//  Created by Lukas Obermann on 09.05.17.
//
//

#include "VideoCacheUpdater.h"

#include "music/MusicDatabase.h"
#include "music/MusicDatabaseCache.h"
#include "utils/log.h"
#include "VideoDatabase.h"

using namespace MUSICDBCACHE;

CVideoCacheUpdater::CVideoCacheUpdater()
{

}

CVideoCacheUpdater::~CVideoCacheUpdater()
{
}

bool CVideoCacheUpdater::DoWork()
{
  CLog::Log(LOGINFO, "CVideoCacheUpdater started");
  CVideoDatabase videodb;
  videodb.getCache().languageChange();
  CMusicDatabase musicdb;
  musicdb.getCache().languageChange();
  CLog::Log(LOGINFO, "CVideoCacheUpdater finished");

  return true;
}
