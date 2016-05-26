/*
 *      Copyright (C) 2012-2016 Team Kodi
 *      http://kodi.tv
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
 *  along with this Program; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "RetroPlayerVideo.h"
#include "PixelConverter.h"
#include "cores/VideoPlayer/DVDCodecs/Video/DVDVideoCodec.h"
#include "cores/VideoPlayer/VideoRenderers/RenderFlags.h"
#include "cores/VideoPlayer/VideoRenderers/RenderManager.h"
#include "utils/log.h"
#include "windowing/WindowingFactory.h"

#include <atomic> // TODO

using namespace GAME;

CRetroPlayerVideo::CRetroPlayerVideo(CDVDClock& clock, CRenderManager& renderManager, CProcessInfo& processInfo) :
  //CThread("RetroPlayerVideo"),
  m_clock(clock),
  m_renderManager(renderManager),
  m_processInfo(processInfo),
  m_framerate(0.0),
  m_bConfigured(false),
  m_droppedFrames(0)
{
  m_renderManager.PreInit();
}

CRetroPlayerVideo::~CRetroPlayerVideo()
{
  CloseStream();
  m_renderManager.UnInit();
}

bool CRetroPlayerVideo::OpenPixelStream(AVPixelFormat pixfmt, unsigned int width, unsigned int height, double framerate)
{
  CLog::Log(LOGINFO, "RetroPlayerVideo: Creating video stream with pixel format: %i, %dx%d", pixfmt, width, height);

  m_framerate = framerate;
  m_bConfigured = false;
  m_droppedFrames = 0;
  m_pixelConverter.reset(new CPixelConverter);
  if (m_pixelConverter->Open(pixfmt, AV_PIX_FMT_YUV420P, width, height))
    return true;

  m_pixelConverter.reset();

  return false;
}

bool CRetroPlayerVideo::OpenEncodedStream(AVCodecID codec)
{
  /* TODO
  if (!m_videoStream)
    m_videoStream = new CDemuxStreamVideo;
  else
    m_videoStream->changes++;

  // Stream
  m_videoStream->uniqueId = GAME_STREAM_VIDEO_ID;
  m_videoStream->codec = codec;
  m_videoStream->codec_fourcc = 0; // TODO
  m_videoStream->type = STREAM_VIDEO;
  m_videoStream->source = STREAM_SOURCE_DEMUX;
  m_videoStream->realtime = true;
  m_videoStream->disabled = false;

  // Video
  m_videoStream->pixfmt = pixfmt;
  m_videoStream->iHeight = height;
  m_videoStream->iWidth = width;
  */
  return false;
}

void CRetroPlayerVideo::AddData(const uint8_t* data, unsigned int size)
{
  DVDVideoPicture picture = { };

  if (GetPicture(data, size, picture))
  {
    if (!Configure(picture))
    {
      CLog::Log(LOGERROR, "RetroPlayerVideo: Failed to configure renderer");
      CloseStream();
    }
    else
    {
      SendPicture(picture);
    }
  }
}

void CRetroPlayerVideo::CloseStream()
{
  m_pixelConverter.reset();
}

bool CRetroPlayerVideo::Configure(DVDVideoPicture& picture)
{
  if (!m_bConfigured)
  {
    // Determine RenderManager flags
    unsigned int flags = CONF_FLAGS_YUVCOEF_BT601 | // color_matrix = 4
                         CONF_FLAGS_FULLSCREEN;     // Allow fullscreen

    const int orientation = picture.format == RENDER_FMT_LIBRETROGL ? 180 : 0; // (90 = 5, 180 = 2, 270 = 7) if we ever want to use RETRO_ENVIRONMENT_SET_ROTATION

    const int buffers = 1; // TODO

    m_bConfigured = m_renderManager.Configure(picture, static_cast<float>(m_framerate), flags, orientation, buffers);
  }

  return m_bConfigured;
}

bool CRetroPlayerVideo::GetPicture(const uint8_t* data, unsigned int size, DVDVideoPicture& picture)
{
  bool bHasPicture = false;

  int lateframes;
  double renderPts;
  int queued, discard;
  m_renderManager.GetStats(lateframes, renderPts, queued, discard);

  if (queued == 0)
  {
    if (data != (uint8_t*)-1)
    {
      if (m_pixelConverter && m_pixelConverter->Decode(data, size))
      {
        m_pixelConverter->GetPicture(picture);
        bHasPicture = true;
      }
    }
    else
    {
        m_retroglpic.texWidth = 640;
        m_retroglpic.texHeight = 480;

        picture.libretrogl = &m_retroglpic;
        picture.format = RENDER_FMT_LIBRETROGL;
        picture.dts            = DVD_NOPTS_VALUE;
        picture.pts            = DVD_NOPTS_VALUE;
        picture.iFlags         = 0; // *not* DVP_FLAG_ALLOCATED
        picture.color_matrix   = 4; // CONF_FLAGS_YUVCOEF_BT601
        picture.color_range    = 0; // *not* CONF_FLAGS_YUV_FULLRANGE
        picture.iWidth         = m_retroglpic.texWidth;
        picture.iHeight        = m_retroglpic.texHeight;
        picture.iDisplayWidth  = m_retroglpic.texWidth;
        picture.iDisplayHeight = m_retroglpic.texHeight;

        bHasPicture = true;
    }

  }

  return bHasPicture;
}

void CRetroPlayerVideo::SendPicture(DVDVideoPicture& picture)
{
  std::atomic_bool bAbortOutput(false); // TODO

  int index = m_renderManager.AddVideoPicture(picture);
  if (index < 0)
  {
    // Video device might not be done yet, drop the frame
    m_droppedFrames++;
  }
  else
  {
    m_renderManager.FlipPage(bAbortOutput);
  }
}

game_proc_address_t CRetroPlayerVideo::GetProcAddress(const char* sym)
{
  return glXGetProcAddress((const GLubyte*) sym);
}

uintptr_t CRetroPlayerVideo::GetCurrentFramebuffer()
{
  return (uintptr_t)m_fboId;
}

bool CRetroPlayerVideo::CreateGlxContext()
{
  GLXContext   glContext;

  m_Display = g_Windowing.GetDisplay();
  glContext = g_Windowing.GetGlxContext();
  m_Window = g_Windowing.GetWindow();

  // Get our window attribs.
  XWindowAttributes wndattribs;
  XGetWindowAttributes(m_Display, m_Window, &wndattribs);

  // Get visual Info
  XVisualInfo visInfo;
  visInfo.visualid = wndattribs.visual->visualid;
  int nvisuals = 0;
  XVisualInfo* visuals = XGetVisualInfo(m_Display, VisualIDMask, &visInfo, &nvisuals);
  if (nvisuals != 1)
  {
    CLog::Log(LOGERROR, "RetroPlayer::CreateGlxContext - could not find visual");
    return false;
  }
  visInfo = visuals[0];
  XFree(visuals);

  m_pixmap = XCreatePixmap(m_Display,
                           m_Window,
                           192,
                           108,
                           visInfo.depth);
  if (!m_pixmap)
  {
    CLog::Log(LOGERROR, "RetroPlayer::CreateGlxContext - Unable to create XPixmap");
    return false;
  }

  // create gl pixmap
  m_glPixmap = glXCreateGLXPixmap(m_Display, &visInfo, m_pixmap);

  if (!m_glPixmap)
  {
    CLog::Log(LOGINFO, "RetroPlayer::CreateGlxContext - Could not create glPixmap");
    return false;
  }

  m_glContext = glXCreateContext(m_Display, &visInfo, glContext, True);

  if (!glXMakeCurrent(m_Display, m_glPixmap, m_glContext))
  {
    CLog::Log(LOGINFO, "RetroPlayer::CreateGlxContext - Could not make Pixmap current");
    return false;
  }

  CLog::Log(LOGNOTICE, "RetroPlayer::CreateGlxContext - created context");
  return true;
}

bool CRetroPlayerVideo::CreateFramebuffer()
{
  glGenFramebuffers(1, &m_fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);

  // attach the texture to FBO color attachment point
  glFramebufferTexture2D(GL_FRAMEBUFFER,          // 1. fbo target: GL_FRAMEBUFFER
                         GL_COLOR_ATTACHMENT0,      // 2. attachment point
                         GL_TEXTURE_2D,             // 3. tex target: GL_TEXTURE_2D
                         m_retroglpic.texture[0],   // 4. tex ID
                         0);                        // 5. mipmap level: 0(base){

  CreateDepthbuffer();

  // check FBO status
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if(status != GL_FRAMEBUFFER_COMPLETE)
    return false;

  return true;
}

bool CRetroPlayerVideo::CreateTexture()
{
  glBindTexture(GL_TEXTURE_2D, 0);
  glGenTextures(1, &m_retroglpic.texture[0]);

  glBindTexture(GL_TEXTURE_2D, m_retroglpic.texture[0]);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0,
               GL_RGB, GL_UNSIGNED_BYTE, 0);

  return true;
}

bool CRetroPlayerVideo::CreateDepthbuffer()
{
  glGenRenderbuffers(1, &m_retroglpic.depth[0]);
  glBindRenderbuffer(GL_RENDERBUFFER, m_retroglpic.depth[0]);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 640, 480);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, m_retroglpic.depth[0]);
  return true;
}

void CRetroPlayerVideo::CreateHwRenderContext()
{
  CreateGlxContext();
  CreateTexture();
  if (!CreateFramebuffer())
    CLog::Log(LOGINFO, "RetroPlayerVideo: Could not create framebuffer object");
}
