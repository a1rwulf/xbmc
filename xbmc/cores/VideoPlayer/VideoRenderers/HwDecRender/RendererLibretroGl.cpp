/*
 *      Copyright (C) 2007-2015 Team Kodi
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
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "RendererLibretroGl.h"

#include "cores/VideoPlayer/DVDCodecs/Video/DVDVideoCodec.h"
#include "cores/RetroPlayer/RetroGlRenderPicture.h"
#include "settings/Settings.h"
#include "settings/AdvancedSettings.h"
#include "utils/log.h"
#include "utils/GLUtils.h"
#include "windowing/WindowingFactory.h"

CRendererLibretroGl::CRendererLibretroGl()
{

}

CRendererLibretroGl::~CRendererLibretroGl()
{
    for (int i = 0; i < NUM_BUFFERS; ++i)
    {
        DeleteTexture(i);
    }
}

void CRendererLibretroGl::AddVideoPictureHW(DVDVideoPicture &picture, int index)
{
    LIBRETROGL::CRetroGlRenderPicture *libretrogl = picture.libretrogl;
    YUVBUFFER &buf = m_buffers[index];
    LIBRETROGL::CRetroGlRenderPicture *pic = libretrogl;

    /* TODO Check how to free this thing
    if (buf.hwDec)
        ((LIBRETROGL::CRetroGlRenderPicture*)buf.hwDec)->Release();
    */
    buf.hwDec = pic;
}

void CRendererLibretroGl::ReleaseBuffer(int idx)
{
    YUVBUFFER &buf = m_buffers[idx];
    /* TODO Check how to free this thing
    if (buf.hwDec)
        ((LIBRETROGL::CRetroGlRenderPicture*)buf.hwDec)->Release();
    */
    buf.hwDec = NULL;
}

CRenderInfo CRendererLibretroGl::GetRenderInfo()
{
    CRenderInfo info;
    info.formats = m_formats;
    //info.max_buffer_size = NUM_BUFFERS;
    info.max_buffer_size = 1;
    info.optimal_buffer_size = 1;
    return info;
}

bool CRendererLibretroGl::Supports(ERENDERFEATURE feature)
{
  if(feature == RENDERFEATURE_ROTATION)
    return true;

  return false;
}

bool CRendererLibretroGl::Supports(EINTERLACEMETHOD method)
{
    return false;
}

bool CRendererLibretroGl::Supports(ESCALINGMETHOD method)
{
    return false;
}

bool CRendererLibretroGl::LoadShadersHook()
{
    if (m_format == RENDER_FMT_LIBRETROGL)
    {
        CLog::Log(LOGNOTICE, "GL: Using LIBRETROGL render method");
        m_renderMethod = RENDER_LIBRETROGL;
        m_fullRange = false;
        return true;
    }
    return false;
}

bool CRendererLibretroGl::RenderHook(int idx)
{
    UpdateVideoFilter();
    RenderRGB(idx, m_currentField);

    YUVBUFFER &buf = m_buffers[idx];
    if (buf.hwDec)
    {
        //TODO check if this is needed
        //((LIBRETROGL::CLibretroGlRenderPicture*)buf.hwDec)->Sync();
    }
    return true;
}

bool CRendererLibretroGl::CreateTexture(int index)
{
    YV12Image &im     = m_buffers[index].image;
    YUVFIELDS &fields = m_buffers[index].fields;
    YUVPLANE  &plane  = fields[FIELD_FULL][0];

    DeleteTexture(index);

    memset(&im    , 0, sizeof(im));
    memset(&fields, 0, sizeof(fields));
    im.height = m_sourceHeight;
    im.width  = m_sourceWidth;

    plane.texwidth  = im.width;
    plane.texheight = im.height;

    plane.pixpertex_x = 1;
    plane.pixpertex_y = 1;

    plane.id = 1;

    return true;
}

bool CRendererLibretroGl::UploadTexture(int index)
{
    LIBRETROGL::CRetroGlRenderPicture *retro = (LIBRETROGL::CRetroGlRenderPicture*)m_buffers[index].hwDec;
    YUVFIELDS &fields = m_buffers[index].fields;
    YUVPLANE &plane = fields[FIELD_FULL][0];

    if (!retro /*|| !retro->valid*/)
    {
        return false;
    }

    plane.id = retro->texture[0];
    plane.rect = m_sourceRect;

    plane.texheight = retro->texHeight;
    plane.texwidth  = retro->texWidth;

    //Textures are upside down and mirrored by default
    //We configure the RenderManager with orientation 180 in RetroPlayer and swap x-coords here
    auto tmpx1 = plane.rect.x1;
    plane.rect.x1 = plane.rect.x2;
    plane.rect.x2 = tmpx1;

    if (m_textureTarget == GL_TEXTURE_2D)
    {
        plane.rect.y1 /= plane.texheight;
        plane.rect.y2 /= plane.texheight;
        plane.rect.x1 /= plane.texwidth;
        plane.rect.x2 /= plane.texwidth;
    }

    return true;
}

void CRendererLibretroGl::DeleteTexture(int index)
{
}
