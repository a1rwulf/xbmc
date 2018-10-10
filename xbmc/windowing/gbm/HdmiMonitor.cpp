/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include <libudev.h>
#include "HdmiMonitor.h"
#include "messaging/ApplicationMessenger.h"
#include "platform/linux/FDEventMonitor.h"
#include "rendering/gles/RenderSystemGLES.h"
#include "ServiceBroker.h"
#include "settings/DisplaySettings.h"
#include "utils/log.h"
#include "windowing/gbm/WinSystemGbm.h"
#include "windowing/GraphicContext.h"

using namespace KODI::MESSAGING;

CHdmiMonitor::CHdmiMonitor(CWinSystemGbm &winSys) :
  m_fdMonitorId(0),
  m_udev(NULL),
  m_udevMonitor(NULL),
  m_winSystem(winSys)
{
}

CHdmiMonitor::~CHdmiMonitor()
{
  Stop();
}

void CHdmiMonitor::Start()
{
  int err;

  CLog::Log(LOGWARNING, "CHdmiMonitor::Start");

  if (!m_udev)
  {
    m_udev = udev_new();
    if (!m_udev)
    {
      CLog::Log(LOGWARNING, "CHdmiMonitor::Start - Unable to open udev handle");
      return;
    }

    m_udevMonitor = udev_monitor_new_from_netlink(m_udev, "udev");
    if (!m_udevMonitor)
    {
      CLog::Log(LOGERROR, "CHdmiMonitor::Start - udev_monitor_new_from_netlink() failed");
      goto err_unref_udev;
    }

    err = udev_monitor_filter_add_match_subsystem_devtype(m_udevMonitor, "drm", NULL);
    if (err)
    {
      CLog::Log(LOGERROR, "CHdmiMonitor::Start - udev_monitor_filter_add_match_subsystem_devtype() failed");
      goto err_unref_monitor;
    }

    err = udev_monitor_enable_receiving(m_udevMonitor);
    if (err)
    {
      CLog::Log(LOGERROR, "CHdmiMonitor::Start - udev_monitor_enable_receiving() failed");
      goto err_unref_monitor;
    }

    m_data.udev_monitor = m_udevMonitor;
    m_data.hdmi_monitor = this;

    g_fdEventMonitor.AddFD(
        CFDEventMonitor::MonitoredFD(udev_monitor_get_fd(m_udevMonitor),
                                     EPOLLIN, FDEventCallback, &m_data),
        m_fdMonitorId);
  }

  return;

err_unref_monitor:
  udev_monitor_unref(m_udevMonitor);
  m_udevMonitor = NULL;
err_unref_udev:
  udev_unref(m_udev);
  m_udev = NULL;
}

void CHdmiMonitor::Stop()
{
  if (m_udev)
  {
    g_fdEventMonitor.RemoveFD(m_fdMonitorId);

    udev_monitor_unref(m_udevMonitor);
    m_udevMonitor = NULL;
    udev_unref(m_udev);
    m_udev = NULL;
  }
}

void CHdmiMonitor::FDEventCallback(int id, int fd, short revents, void *data)
{
  TMonitorData *monitorData = (TMonitorData*)data;
  struct udev_monitor *udevMonitor = (struct udev_monitor *)monitorData->udev_monitor;
  CHdmiMonitor *hdmiMonitor = (CHdmiMonitor*)monitorData->hdmi_monitor;
  struct udev_device *device;

  while ((device = udev_monitor_receive_device(udevMonitor)) != NULL)
  {
    const char* action = udev_device_get_action(device);

    CLog::Log(LOGNOTICE, "CHdmiMonitor - Action %s (\"%s\", \"%s\")", action,
                                                                            udev_device_get_syspath(device),
                                                                            udev_device_get_devpath(device));

    std::shared_ptr<CDRMUtils> tmp = hdmiMonitor->GetWinSystem().GetDrm();
    if (dynamic_cast<COffScreenModeSetting*>(tmp.get()) != nullptr)
      CApplicationMessenger::GetInstance().PostMsg(TMSG_RENDERER_REINIT);

    udev_device_unref(device);
  }
}
