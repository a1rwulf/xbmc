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
#pragma once
#define GLX_GLXEXT_PROTOTYPES
#include "system_gl.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include "RetroGlRenderPicture.h"
#include "addons/kodi-addon-dev-kit/include/kodi/kodi_game_types.h"

#include "games/addons/GameClientCallbacks.h"
//#include "threads/Thread.h"

#include <memory>

class CDVDClock;
class CPixelConverter;
class CProcessInfo;
class CRenderManager;
struct DVDVideoPicture;

namespace GAME
{
  class CRetroPlayerVideo : public IGameVideoCallback
                            //protected CThread
  {
  public:
    CRetroPlayerVideo(CDVDClock& m_clock, CRenderManager& m_renderManager, CProcessInfo& m_processInfo);

    virtual ~CRetroPlayerVideo();

    // implementation of IRetroPlayerVideoCallback
    virtual bool OpenPixelStream(AVPixelFormat pixfmt, unsigned int width, unsigned int height, double framerate) override;
    virtual bool OpenEncodedStream(AVCodecID codec) override;
    virtual void AddData(const uint8_t* data, unsigned int size) override;
    virtual void CloseStream() override;

    uintptr_t GetCurrentFramebuffer();
    game_proc_address_t GetProcAddress(const char* sym);
    void CreateHwRenderContext();
    /*
  protected:
    // implementation of CThread
    virtual void Process(void);
    */

  private:
    bool Configure(DVDVideoPicture& picture);
    bool GetPicture(const uint8_t* data, unsigned int size, DVDVideoPicture& picture);
    void SendPicture(DVDVideoPicture& picture);

    bool CreateGlxContext();
    bool CreateFramebuffer();
    bool CreateTexture();
    bool CreateDepthbuffer();

    // Construction parameters
    CDVDClock&      m_clock;
    CRenderManager& m_renderManager;
    CProcessInfo&   m_processInfo;

    // Stream properties
    double       m_framerate;
    bool         m_bConfigured; // Need first picture to configure the render manager
    unsigned int m_droppedFrames;
    std::unique_ptr<CPixelConverter> m_pixelConverter;

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
