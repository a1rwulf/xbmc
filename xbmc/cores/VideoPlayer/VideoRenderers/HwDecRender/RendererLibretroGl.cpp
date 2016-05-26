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
    info.max_buffer_size = NUM_BUFFERS;
    info.optimal_buffer_size = 1;
    return info;
}

bool CRendererLibretroGl::Supports(ERENDERFEATURE feature)
{
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

    YV12Image &im     = m_buffers[index].image;
    YUVFIELDS &fields = m_buffers[index].fields;
    YUVPLANE &plane = fields[FIELD_FULL][0];

    if (!retro /*|| !retro->valid*/)
    {
        return false;
    }

    plane.id = retro->texture[0];

    // in stereoscopic mode sourceRect may only
    // be a part of the source video surface
    plane.rect = m_sourceRect;

    // clip rect
    /*
    if (retro->crop.x1 > plane.rect.x1)
      plane.rect.x1 = retro->crop.x1;
    if (retro->crop.x2 < plane.rect.x2)
      plane.rect.x2 = retro->crop.x2;
    if (retro->crop.y1 > plane.rect.y1)
      plane.rect.y1 = retro->crop.y1;
    if (retro->crop.y2 < plane.rect.y2)
      plane.rect.y2 = retro->crop.y2;
    */

    plane.texheight = retro->texHeight;
    plane.texwidth  = retro->texWidth;

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