#include "OffScreenModeSetting.h"

bool COffScreenModeSetting::Init()
{
  // Create a dummy 30HZ fullhd mode
  m_mode = new drmModeModeInfo;
  m_mode->hdisplay = 1920;
  m_mode->vdisplay = 1080;
  m_mode->vrefresh = 30;

  // Open the render node that is needed for off-screen rendering
  m_fd = open("/dev/dri/renderD128", O_RDWR);

  return m_fd != -1;
}

void COffScreenModeSetting::Destroy()
{
  close(m_fd);
  m_fd = -1;

  delete m_mode;
  m_mode = nullptr;
}

std::vector<RESOLUTION_INFO> COffScreenModeSetting::GetModes()
{
    std::vector<RESOLUTION_INFO> resolutions;
    RESOLUTION_INFO res;

    res.iWidth = res.iScreenWidth = m_mode->hdisplay;
    res.iHeight = res.iScreenHeight = m_mode->vdisplay;
    res.fRefreshRate = m_mode->vrefresh;
    res.iSubtitles = static_cast<int>(0.965 * res.iHeight);
    res.fPixelRatio = 1.0f;
    res.bFullScreen = true;
    res.strId = std::to_string(0);

    resolutions.push_back(res);
    return resolutions;
}

RESOLUTION_INFO COffScreenModeSetting::GetCurrentMode() const
{
  RESOLUTION_INFO res;
  res.iScreen = 0;
  res.iScreenWidth = 1920;
  res.iScreenHeight = 1080;
  res.iWidth = 1920;
  res.iHeight = 1080;
  res.fRefreshRate = 30.0f;
  res.iSubtitles = static_cast<int>(0.965 * res.iHeight);
  res.fPixelRatio = 1.0f;
  res.bFullScreen = true;
  res.strId = std::to_string(0);

  return res;
}
