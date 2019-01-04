//
//  VideoCacheUpdater.cpp
//  kodi
//
//  Created by Lukas Obermann on 09.05.17.
//
//

#include "VideoCacheUpdater.h"

#include "utils/log.h"
#include "VideoDatabase.h"

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
  CLog::Log(LOGINFO, "CVideoCacheUpdater finished");

  return true;
}
