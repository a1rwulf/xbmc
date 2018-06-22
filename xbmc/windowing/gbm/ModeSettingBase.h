/*
 *      Copyright (C) 2005-2018 Team XBMC
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <string>
#include <xf86drmMode.h>
#include "windowing/Resolution.h"

#include <vector>

class IModeSettingBase
{
public:
  IModeSettingBase() {};
  virtual ~IModeSettingBase() = default;
  virtual void FlipPage(struct gbm_bo *bo, bool rendered, bool videoLayer) = 0;
  virtual bool SetVideoMode(const RESOLUTION_INFO& res, struct gbm_bo *bo) = 0;
  virtual bool SetActive(bool active) = 0;
  virtual bool Init() = 0;
  virtual void Destroy() = 0;

  virtual std::string GetModule() const = 0;
  virtual std::string GetDevicePath() const = 0;
  virtual int GetFileDescriptor() const = 0;
  virtual struct plane* GetPrimaryPlane() const = 0;
  virtual struct plane* GetOverlayPlane() const = 0;
  virtual struct crtc* GetCrtc() const = 0;
  virtual RESOLUTION_INFO GetCurrentMode() const = 0;

  virtual std::vector<RESOLUTION_INFO> GetModes() = 0;
  virtual bool SetMode(const RESOLUTION_INFO& res) = 0;
  virtual void WaitVBlank() = 0;

  virtual bool AddProperty(struct drm_object *object, const char *name, uint64_t value) = 0;
  virtual bool SetProperty(struct drm_object *object, const char *name, uint64_t value) = 0;
};
