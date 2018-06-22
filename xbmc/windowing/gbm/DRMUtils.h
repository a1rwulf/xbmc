/*
 *      Copyright (C) 2005-2013 Team XBMC
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

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <vector>

#include "windowing/Resolution.h"
#include "GBMUtils.h"
#include "windowing/gbm/ModeSettingBase.h"

struct drm_object
{
  uint32_t id = 0;
  uint32_t type = 0;
  drmModeObjectPropertiesPtr props = nullptr;
  drmModePropertyRes **props_info = nullptr;
};

struct plane : drm_object
{
  drmModePlanePtr plane = nullptr;
  uint32_t format;
};

struct connector : drm_object
{
  drmModeConnectorPtr connector = nullptr;
};

struct encoder
{
  drmModeEncoder *encoder = nullptr;
};

struct crtc : drm_object
{
  drmModeCrtcPtr crtc = nullptr;
};

struct drm_fb
{
  struct gbm_bo *bo = nullptr;
  uint32_t fb_id;
};

class CDRMUtils : public IModeSettingBase
{
public:
  CDRMUtils();
  virtual ~CDRMUtils() = default;
  virtual void FlipPage(struct gbm_bo *bo, bool rendered, bool videoLayer) override {}
  virtual bool SetVideoMode(const RESOLUTION_INFO& res, struct gbm_bo *bo) override { return false; }
  virtual bool SetActive(bool active) override { return false; }
  virtual bool Init() override;
  virtual void Destroy() override;

  virtual std::string GetModule() const override { return m_module; }
  virtual std::string GetDevicePath() const override{ return m_device_path; }
  virtual int GetFileDescriptor() const override { return m_fd; }
  virtual struct plane* GetPrimaryPlane() const override { return m_primary_plane; }
  virtual struct plane* GetOverlayPlane() const override { return m_overlay_plane; }
  virtual struct crtc* GetCrtc() const override { return m_crtc; }
  
  virtual RESOLUTION_INFO GetCurrentMode() const override;
  virtual std::vector<RESOLUTION_INFO> GetModes() override;
  virtual bool SetMode(const RESOLUTION_INFO& res) override;
  virtual void WaitVBlank() override;

  virtual bool AddProperty(struct drm_object *object, const char *name, uint64_t value) override { return false; }
  virtual bool SetProperty(struct drm_object *object, const char *name, uint64_t value) override { return false; }

protected:
  bool OpenDrm();
  uint32_t GetPropertyId(struct drm_object *object, const char *name);
  drm_fb* DrmFbGetFromBo(struct gbm_bo *bo);

  int m_fd;
  struct connector *m_connector = nullptr;
  struct encoder *m_encoder = nullptr;
  struct crtc *m_crtc = nullptr;
  struct plane *m_primary_plane = nullptr;
  struct plane *m_overlay_plane = nullptr;
  drmModeModeInfo *m_mode = nullptr;

  int m_width = 0;
  int m_height = 0;

private:
  bool GetResources();
  bool FindConnector();
  bool FindEncoder();
  bool FindCrtc();
  bool FindPlanes();
  bool FindPreferredMode();
  bool RestoreOriginalMode();
  static void DrmFbDestroyCallback(struct gbm_bo *bo, void *data);
  RESOLUTION_INFO GetResolutionInfo(drmModeModeInfoPtr mode) const;

  int m_crtc_index;
  std::string m_module;
  std::string m_device_path;

  drmModeResPtr m_drm_resources = nullptr;
  drmModeCrtcPtr m_orig_crtc = nullptr;
};
