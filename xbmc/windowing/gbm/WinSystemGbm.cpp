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

#include "WinSystemGbm.h"
#include "ServiceBroker.h"
#include "settings/DisplaySettings.h"
#include "settings/Settings.h"
#include "settings/lib/Setting.h"
#include <string.h>

#include "OptionalsReg.h"
#include "platform/linux/OptionalsReg.h"
#include "windowing/GraphicContext.h"
#include "platform/linux/powermanagement/LinuxPowerSyscall.h"
#include "settings/DisplaySettings.h"
#include "utils/log.h"
#include "utils/StringUtils.h"
#include "DRMAtomic.h"
#include "DRMLegacy.h"
#include "messaging/ApplicationMessenger.h"
#ifdef TARGET_POSIX
#include "XTimeUtils.h"
#endif

CWinSystemGbm::CWinSystemGbm() :
  m_DRM(nullptr),
  m_GBM(new CGBMUtils),
  m_delayDispReset(false),
  m_libinput(new CLibInputHandler),
  m_offScreen(false),
  m_offscreen_fd(-1)
{
  std::string envSink;
  if (getenv("KODI_AE_SINK"))
    envSink = getenv("KODI_AE_SINK");
  if (StringUtils::EqualsNoCase(envSink, "ALSA"))
  {
    OPTIONALS::ALSARegister();
  }
  else if (StringUtils::EqualsNoCase(envSink, "PULSE"))
  {
    OPTIONALS::PulseAudioRegister();
  }
  else if (StringUtils::EqualsNoCase(envSink, "OSS"))
  {
    OPTIONALS::OSSRegister();
  }
  else if (StringUtils::EqualsNoCase(envSink, "SNDIO"))
  {
    OPTIONALS::SndioRegister();
  }
  else
  {
    if (!OPTIONALS::PulseAudioRegister())
    {
      if (!OPTIONALS::ALSARegister())
      {
        if (!OPTIONALS::SndioRegister())
        {
          OPTIONALS::OSSRegister();
        }
      }
    }
  }

  CLinuxPowerSyscall::Register();
  m_lirc.reset(OPTIONALS::LircRegister());
  m_libinput->Start();

  m_deviceMonitor.Start();
}

bool CWinSystemGbm::InitWindowSystem()
{
  m_DRM = std::make_shared<CDRMAtomic>();

  if (!m_DRM->InitDrm())
  {
    CLog::Log(LOGERROR, "CWinSystemGbm::%s - failed to initialize Atomic DRM", __FUNCTION__);
    m_DRM.reset();

    m_DRM = std::make_shared<CDRMLegacy>();

    if (!m_DRM->InitDrm())
    {
      CLog::Log(LOGERROR, "CWinSystemGbm::%s - failed to initialize Legacy DRM", __FUNCTION__);
      m_DRM.reset();
    }
  }

  // Create DRM GBM device if we could initialize DRM
  if (m_DRM)
  {
    if (!m_GBM->CreateDevice(m_DRM->m_fd))
    {
      m_GBM.reset();
      return false;
    }
    CLog::Log(LOGDEBUG, "CWinSystemGbm::%s - initialized DRM", __FUNCTION__);
    m_offScreen = false;
  }
  else // Most likely no screen is attached use off screen rendering
  {
    m_offscreen_fd = open("/dev/dri/renderD128", O_RDWR);
    if (!m_GBM->CreateDevice(m_offscreen_fd))
    {
      CLog::Log(LOGINFO, "CWinSystemGbm::%s - failed to initialize off screen rendering", __FUNCTION__);
      m_GBM.reset();
      return false;
    }
    CLog::Log(LOGINFO, "CWinSystemGbm::%s - initialized off screen rendering", __FUNCTION__);
    m_offScreen = true;
  }

  return CWinSystemBase::InitWindowSystem();
}

bool CWinSystemGbm::DestroyWindowSystem()
{
  if (m_GBM)
  {
    m_GBM->DestroySurface();
    m_GBM->DestroyDevice();
  }

  if (m_DRM)
    m_DRM->DestroyDrm();

  if (m_offscreen_fd != -1)
  {
    CLog::Log(LOGINFO, "CWinSystemGbm::%s - close off screen rendering file descriptor", __FUNCTION__);
    close(m_offscreen_fd);
  }

  CLog::Log(LOGDEBUG, "CWinSystemGbm::%s - deinitialized DRM", __FUNCTION__);
  return CWinSystemBase::DestroyWindowSystem();
}

bool CWinSystemGbm::CreateNewWindow(const std::string& name,
                                    bool fullScreen,
                                    RESOLUTION_INFO& res)
{
  //Notify other subsystems that we change resolution
  OnLostDevice();

  if(m_DRM && !m_DRM->SetMode(res))
  {
    CLog::Log(LOGERROR, "CWinSystemGbm::%s - failed to set DRM mode", __FUNCTION__);
    return false;
  }

  if(m_DRM && !m_GBM->CreateSurface(m_DRM->m_mode->hdisplay, m_DRM->m_mode->vdisplay))
  {
    CLog::Log(LOGERROR, "CWinSystemGbm::%s - failed to initialize GBM", __FUNCTION__);
    return false;
  }

  m_bFullScreen = fullScreen;

  CLog::Log(LOGDEBUG, "CWinSystemGbm::%s - initialized GBM", __FUNCTION__);
  return true;
}

bool CWinSystemGbm::DestroyWindow()
{
  if (m_GBM)
    m_GBM->DestroySurface();

  CLog::Log(LOGDEBUG, "CWinSystemGbm::%s - deinitialized GBM", __FUNCTION__);
  return true;
}

void CWinSystemGbm::UpdateResolutions()
{
  CWinSystemBase::UpdateResolutions();

  if (!m_DRM)
  {
    CLog::Log(LOGWARNING, "CWinSystemGbm::%s - no display connected use defaults", __FUNCTION__);
    UpdateDesktopResolution(CDisplaySettings::GetInstance().GetResolutionInfo(RES_DESKTOP),
                          0,
                          1920,
                          1080,
                          60);


    return;
  }

  UpdateDesktopResolution(CDisplaySettings::GetInstance().GetResolutionInfo(RES_DESKTOP),
                          0,
                          m_DRM->m_mode->hdisplay,
                          m_DRM->m_mode->vdisplay,
                          m_DRM->m_mode->vrefresh);

  auto resolutions = m_DRM->GetModes();
  if (resolutions.empty())
  {
    CLog::Log(LOGWARNING, "CWinSystemGbm::%s - Failed to get resolutions", __FUNCTION__);
  }
  else
  {
    CDisplaySettings::GetInstance().ClearCustomResolutions();

    for (auto &res : resolutions)
    {
      CServiceBroker::GetWinSystem()->GetGfxContext().ResetOverscan(res);
      CDisplaySettings::GetInstance().AddResolutionInfo(res);

      CLog::Log(LOGNOTICE, "Found resolution %dx%d for display %d with %dx%d%s @ %f Hz",
                res.iWidth,
                res.iHeight,
                res.iScreen,
                res.iScreenWidth,
                res.iScreenHeight,
                res.dwFlags & D3DPRESENTFLAG_INTERLACED ? "i" : "",
                res.fRefreshRate);
    }
  }

  CDisplaySettings::GetInstance().ApplyCalibrations();
}

bool CWinSystemGbm::ResizeWindow(int newWidth, int newHeight, int newLeft, int newTop)
{
  return true;
}

bool CWinSystemGbm::SetFullScreen(bool fullScreen, RESOLUTION_INFO& res, bool blankOtherDisplays)
{
  // If DRM isn't initialized (we are in offscreen rendering mode) we cannot set modes
  if (!m_DRM)
    return true;

  // Notify other subsystems that we will change resolution
  OnLostDevice();

  if(!m_DRM->SetMode(res))
  {
    CLog::Log(LOGERROR, "CWinSystemGbm::%s - failed to set DRM mode", __FUNCTION__);
    return false;
  }

  struct gbm_bo *bo = nullptr;

  if (!m_DRM->m_req)
  {
    bo = m_GBM->LockFrontBuffer();
  }

  auto result = m_DRM->SetVideoMode(res, bo);

  if (!m_DRM->m_req)
  {
    m_GBM->ReleaseBuffer();
  }

  int delay = CServiceBroker::GetSettings().GetInt("videoscreen.delayrefreshchange");
  if (delay > 0)
  {
    m_delayDispReset = true;
    m_dispResetTimer.Set(delay * 100);
  }

  return result;
}

void CWinSystemGbm::FlipPage(bool rendered, bool videoLayer)
{
  if (!m_DRM)
    return;

  struct gbm_bo *bo = m_GBM->LockFrontBuffer();

  m_DRM->FlipPage(bo, rendered, videoLayer);

  m_GBM->ReleaseBuffer();
}

void CWinSystemGbm::WaitVBlank()
{
  if (m_DRM)
    m_DRM->WaitVBlank();
  else
    Sleep(50);
}

bool CWinSystemGbm::UseLimitedColor()
{
  return CServiceBroker::GetSettings().GetBool(CSettings::SETTING_VIDEOSCREEN_LIMITEDRANGE);
}

bool CWinSystemGbm::Hide()
{
  bool ret = m_DRM->SetActive(false);
  FlipPage(false, false);
  return ret;
}

bool CWinSystemGbm::Show(bool raise)
{
  bool ret = m_DRM->SetActive(true);
  FlipPage(false, false);
  return ret;
}

void CWinSystemGbm::Register(IDispResource *resource)
{
  CSingleLock lock(m_resourceSection);
  m_resources.push_back(resource);
}

void CWinSystemGbm::Unregister(IDispResource *resource)
{
  CSingleLock lock(m_resourceSection);
  std::vector<IDispResource*>::iterator i = find(m_resources.begin(), m_resources.end(), resource);
  if (i != m_resources.end())
  {
    m_resources.erase(i);
  }
}

void CWinSystemGbm::OnLostDevice()
{
  CLog::Log(LOGDEBUG, "%s - notify display change event", __FUNCTION__);

  { CSingleLock lock(m_resourceSection);
    for (std::vector<IDispResource *>::iterator i = m_resources.begin(); i != m_resources.end(); ++i)
      (*i)->OnLostDisplay();
  }
}

bool CWinSystemGbm::UseOffScreenRendering()
{
  return m_offScreen;
}
