//
//  ODBStreamDetails.h
//  kodi
//
//  Created by Lukas Obermann on 05.10.16.
//
//

#ifndef ODBSTREAMDETAILS_H
#define ODBSTREAMDETAILS_H

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>

#include <string>

#include "ODBFile.h"

#ifdef ODB_COMPILER
#pragma db model version(1, 1, open)
#endif

#pragma db object pointer(std::shared_ptr) \
                  table("stream_details")
class CODBStreamDetails
{
public:
  CODBStreamDetails()
  {
    m_streamType = 0;
    m_videoCodec = "";
    m_videoAspect = 0;
    m_videoWidth = 0;
    m_videoHeight = 0;
    m_audioCodec = "";
    m_audioChannels = 0;
    m_audioLanguage = "";
    m_subtitleLanguage = "";
    m_videoDuration = 0;
    m_stereoMode = "";
    m_videoLanguage = "";
    m_synced = false;
  };
  
#pragma db id auto
  unsigned long m_idStreamDetail;
  odb::lazy_shared_ptr<CODBFile> m_file;
  int m_streamType;
  std::string m_videoCodec;
  float m_videoAspect;
  int m_videoWidth;
  int m_videoHeight;
  std::string m_audioCodec;
  int m_audioChannels;
  std::string m_audioLanguage;
  std::string m_subtitleLanguage;
  int m_videoDuration;
  std::string m_stereoMode;
  std::string m_videoLanguage;
  
  //Members not stored in the db, used for sync ...
#pragma db transient
  bool m_synced;
  
private:
  friend class odb::access;
  
#pragma db index member(m_file)
#pragma db index member(m_streamType)
  
};

#endif /* ODBSTREAMDETAILS_H */
