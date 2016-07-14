/*
 *      Copyright (C) 2016 Team Kodi
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
#pragma once

#define GLX_GLXEXT_PROTOTYPES
#include "system_gl.h"

#include "RetroGlRenderPicture.h"
#include "games/addons/GameClientCallbacks.h"

#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class CProcessInfo;
class CRenderManager;
struct DVDVideoPicture;

namespace GAME
{
  class CRetroPlayerGL : public IGameRenderingCallback
  {
  public:
    CRetroPlayerGL(CRenderManager& m_renderManager, CProcessInfo& m_processInfo);

    virtual ~CRetroPlayerGL();

    // implementation of IGameRenderingCallback
    virtual bool Create() override;
    virtual void Destroy() override;
    virtual uintptr_t GetCurrentFramebuffer() override;
    virtual RetroGLProcAddress GetProcAddress(const char *sym) override;
    virtual void RenderFrame() override;

  private:
    bool Configure(DVDVideoPicture& picture);
    void GetPicture(DVDVideoPicture& picture);
    void SendPicture(DVDVideoPicture& picture);

    bool CreateGlxContext();
    bool CreateTexture();
    bool CreateFramebuffer();
    bool CreateDepthbuffer();

    // Construction parameters
    CRenderManager& m_renderManager;
    CProcessInfo&   m_processInfo;

    // Rendering properties
    bool m_bConfigured;
    Display *m_Display;
    Window m_Window;
    GLXContext m_glContext;
    GLXWindow m_glWindow;
    Pixmap    m_pixmap;
    GLXPixmap m_glPixmap;
    GLuint m_fboId;
    GLuint m_textureId;

    LIBRETROGL::CRetroGlRenderPicture m_retroglpic;
  };
}
