//
//  VideoCacheUpdater.hpp
//  kodi
//
//  Created by Lukas Obermann on 09.05.17.
//
//

#ifndef VIDEOCACHEUPDATE_H
#define VIDEOCACHEUPDATE_H

#include "utils/JobManager.h"

class CVideoCacheUpdater : public CJob
{
public:
  CVideoCacheUpdater();
  virtual ~CVideoCacheUpdater();
  
  /*!
   \brief Work function that extracts thumb.
   */
  virtual bool DoWork();
  
  virtual const char* GetType() const
  {
    return kJobTypeMediaFlags;
  }
};

#endif /* VIDEOCACHEUPDATE_H */
