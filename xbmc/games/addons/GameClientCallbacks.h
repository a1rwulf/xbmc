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

#include "cores/AudioEngine/Utils/AEChannelData.h"
#include "addons/kodi-addon-dev-kit/include/kodi/kodi_game_types.h"

#include "libavcodec/avcodec.h"
#include "libavutil/pixfmt.h"

#include <stdint.h>

class CAEChannelInfo;

namespace GAME
{
  class IGameAudioCallback
  {
  public:
    virtual ~IGameAudioCallback() { }

    virtual unsigned int NormalizeSamplerate(unsigned int samplerate) const = 0;
    virtual bool OpenPCMStream(AEDataFormat format, unsigned int samplerate, const CAEChannelInfo& channelLayout) = 0;
    virtual bool OpenEncodedStream(AVCodecID codec, unsigned int samplerate, const CAEChannelInfo& channelLayout) = 0;
    virtual void AddData(const uint8_t* data, unsigned int size) = 0;
    virtual void CloseStream() = 0;
  };

  class IGameVideoCallback
  {
  public:
    virtual ~IGameVideoCallback() { }

    virtual bool OpenPixelStream(AVPixelFormat pixfmt, unsigned int width, unsigned int height, double framerate) = 0;
    virtual bool OpenEncodedStream(AVCodecID codec) = 0;
    virtual void AddData(const uint8_t* data, unsigned int size) = 0;
    virtual void CloseStream() = 0;
    virtual uintptr_t GetCurrentFramebuffer() = 0;
    virtual game_proc_address_t GetProcAddress(const char *sym) = 0;
    virtual void CreateHwRenderContext() = 0;
  };
}
