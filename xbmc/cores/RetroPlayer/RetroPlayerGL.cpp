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

#include "RetroPlayerGL.h"
#include "cores/VideoPlayer/DVDCodecs/Video/DVDVideoCodec.h"
#include "cores/VideoPlayer/VideoRenderers/RenderFlags.h" // for RENDER_FMT_LIBRETROGL
#include "cores/VideoPlayer/VideoRenderers/RenderFormats.h" // for CONF_FLAGS_FULLSCREEN
#include "cores/VideoPlayer/VideoRenderers/RenderManager.h"
#include "cores/VideoPlayer/DVDClock.h" // for DVD_NOPTS_VALUE
#include "utils/log.h"
#include "windowing/WindowingFactory.h"

#include <atomic> // TODO

using namespace GAME;

CRetroPlayerGL::CRetroPlayerGL(CRenderManager& renderManager, CProcessInfo& processInfo) :
  m_renderManager(renderManager),
  m_processInfo(processInfo),
  m_bConfigured(false),
  m_Display(nullptr),
  m_Window(0),
  m_glContext(nullptr),
  m_glWindow(0),
  m_pixmap(0),
  m_glPixmap(0),
  m_fboId(0),
  m_textureId(0)
{
}

CRetroPlayerGL::~CRetroPlayerGL()
{
  Destroy();
}

bool CRetroPlayerGL::Create()
{
  if (!CreateGlxContext())
    return false;

  if (!CreateTexture())
    return false;

  if (!CreateFramebuffer())
  {
    CLog::Log(LOGINFO, "RetroPlayerGL: Could not create framebuffer object");
    return false;
  }

  return true;
}

void CRetroPlayerGL::Destroy()
{
  //! @todo
}

uintptr_t CRetroPlayerGL::GetCurrentFramebuffer()
{
  return static_cast<uintptr_t>(m_fboId);
}

RetroGLProcAddress CRetroPlayerGL::GetProcAddress(const char* sym)
{
  return glXGetProcAddress(reinterpret_cast<const GLubyte*>(sym));
}

void CRetroPlayerGL::RenderFrame()
{
  DVDVideoPicture picture = { };
  GetPicture(picture);

  if (!Configure(picture))
  {
    CLog::Log(LOGERROR, "RetroPlayerGL: Failed to configure renderer");
  }
  else
  {
    SendPicture(picture);
  }
}

bool CRetroPlayerGL::Configure(DVDVideoPicture& picture)
{
  if (!m_bConfigured)
  {
    const float framerate = 60; //! @todo Get from game client

    // Determine RenderManager flags
    unsigned int flags = CONF_FLAGS_FULLSCREEN;     // Allow fullscreen

    const int orientation = 180; //! @todo Move to game.libretro. See note at https://github.com/a1rwulf/xbmc/commit/dc1ea03#commitcomment-18250984

    const int buffers = 1; //! @todo

    m_bConfigured = m_renderManager.Configure(picture, framerate, flags, orientation, buffers);
  }

  return m_bConfigured;
}

void CRetroPlayerGL::GetPicture(DVDVideoPicture& picture)
{
  m_retroglpic.texWidth = 640;
  m_retroglpic.texHeight = 480;

  picture.libretrogl     = &m_retroglpic;
  picture.format         = RENDER_FMT_LIBRETROGL;
  picture.dts            = DVD_NOPTS_VALUE;
  picture.pts            = DVD_NOPTS_VALUE;
  picture.iWidth         = m_retroglpic.texWidth;
  picture.iHeight        = m_retroglpic.texHeight;
  picture.iDisplayWidth  = m_retroglpic.texWidth;
  picture.iDisplayHeight = m_retroglpic.texHeight;
}

void CRetroPlayerGL::SendPicture(DVDVideoPicture& picture)
{
  std::atomic_bool bAbortOutput(false); // TODO

  int index = m_renderManager.AddVideoPicture(picture);
  if (index < 0)
  {
    // Video device might not be done yet, drop the frame
  }
  else
  {
    m_renderManager.FlipPage(bAbortOutput);
  }
}

bool CRetroPlayerGL::CreateGlxContext()
{
  GLXContext glContext;

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
    CLog::Log(LOGERROR, "RetroPlayerGL::CreateGlxContext - could not find visual");
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
    CLog::Log(LOGERROR, "RetroPlayerGL::CreateGlxContext - Unable to create XPixmap");
    return false;
  }

  // create gl pixmap
  m_glPixmap = glXCreateGLXPixmap(m_Display, &visInfo, m_pixmap);

  if (!m_glPixmap)
  {
    CLog::Log(LOGINFO, "RetroPlayerGL::CreateGlxContext - Could not create glPixmap");
    return false;
  }

  m_glContext = glXCreateContext(m_Display, &visInfo, glContext, True);

  if (!glXMakeCurrent(m_Display, m_glPixmap, m_glContext))
  {
    CLog::Log(LOGINFO, "RetroPlayerGL::CreateGlxContext - Could not make Pixmap current");
    return false;
  }

  CLog::Log(LOGNOTICE, "RetroPlayerGL::CreateGlxContext - created context");
  return true;
}

bool CRetroPlayerGL::CreateTexture()
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

bool CRetroPlayerGL::CreateFramebuffer()
{
  glGenFramebuffers(1, &m_fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);

  // attach the texture to FBO color attachment point
  glFramebufferTexture2D(GL_FRAMEBUFFER,            // 1. fbo target: GL_FRAMEBUFFER
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

bool CRetroPlayerGL::CreateDepthbuffer()
{
  glGenRenderbuffers(1, &m_retroglpic.depth[0]);
  glBindRenderbuffer(GL_RENDERBUFFER, m_retroglpic.depth[0]);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 640, 480);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, m_retroglpic.depth[0]);
  return true;
}
