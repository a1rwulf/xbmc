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

#include "ModeSettingBase.h"

#ifdef TARGET_POSIX
#include "platform/linux/XTimeUtils.h"
#endif

class COffScreenModeSetting : public IModeSettingBase
{
public:
  COffScreenModeSetting() = default;
  virtual ~COffScreenModeSetting() = default;
  virtual void FlipPage(struct gbm_bo *bo, bool rendered, bool videoLayer) override {}
  virtual bool SetVideoMode(const RESOLUTION_INFO& res, struct gbm_bo *bo) override { return false; }
  virtual bool SetActive(bool active) override { return false; }
  virtual bool Init() override;
  virtual void Destroy() override;

  virtual std::string GetModule() const override { return ""; }
  virtual std::string GetDevicePath() const override{ return ""; }
  virtual int GetFileDescriptor() const override { return m_fd; }
  virtual struct plane* GetPrimaryPlane() const override { return nullptr; }
  virtual struct plane* GetOverlayPlane() const override { return nullptr; }
  virtual struct crtc* GetCrtc() const override { return nullptr; }
  virtual RESOLUTION_INFO GetCurrentMode() const override;

  virtual std::vector<RESOLUTION_INFO> GetModes() override;
  virtual bool SetMode(const RESOLUTION_INFO& res) override { return true; }
  virtual void WaitVBlank() override { Sleep(20); }

  virtual bool AddProperty(struct drm_object *object, const char *name, uint64_t value) override { return false; }
  virtual bool SetProperty(struct drm_object *object, const char *name, uint64_t value) override { return false; }

private:
  int m_fd = -1;
  drmModeModeInfo *m_mode = nullptr;
};
