/*
 *  Copyright (C) 2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DisplayUdevMonitor.h"

#include "ServiceBroker.h"
#include "cores/AudioEngine/Interfaces/AE.h"
#include "messaging/ApplicationMessenger.h"
#include "rendering/gles/RenderSystemGLES.h"
#include "utils/log.h"
#include "windowing/WinSystem.h"

#include "platform/linux/FDEventMonitor.h"

#include <libudev.h>

static struct udev* udev;
static struct udev_monitor* udevMonitor;

using namespace KODI::MESSAGING;

namespace KODI
{
namespace WINDOWING
{
namespace GBM
{

CDisplayUdevMonitor::CDisplayUdevMonitor() : m_fdMonitorId(0)
{
}

CDisplayUdevMonitor::~CDisplayUdevMonitor()
{
  Stop();
}

void CDisplayUdevMonitor::Start()
{
  int err;

  CLog::Log(LOGWARNING, "CDisplayUdevMonitor::Start");

  if (!udev)
  {
    udev = udev_new();
    if (!udev)
    {
      CLog::Log(LOGWARNING, "CDisplayUdevMonitor::Start - Unable to open udev handle");
      return;
    }

    udevMonitor = udev_monitor_new_from_netlink(udev, "udev");
    if (!udevMonitor)
    {
      CLog::Log(LOGERROR, "CDisplayUdevMonitor::Start - udev_monitor_new_from_netlink() failed");
      goto err_unref_udev;
    }

    err = udev_monitor_filter_add_match_subsystem_devtype(udevMonitor, "drm", NULL);
    if (err)
    {
      CLog::Log(
          LOGERROR,
          "CDisplayUdevMonitor::Start - udev_monitor_filter_add_match_subsystem_devtype() failed");
      goto err_unref_monitor;
    }

    err = udev_monitor_enable_receiving(udevMonitor);
    if (err)
    {
      CLog::Log(LOGERROR, "CDisplayUdevMonitor::Start - udev_monitor_enable_receiving() failed");
      goto err_unref_monitor;
    }

    g_fdEventMonitor.AddFD(CFDEventMonitor::MonitoredFD(udev_monitor_get_fd(udevMonitor), EPOLLIN,
                                                        FDEventCallback, udevMonitor),
                           m_fdMonitorId);
  }

  return;

err_unref_monitor:
  udev_monitor_unref(udevMonitor);
  udevMonitor = NULL;
err_unref_udev:
  udev_unref(udev);
  udev = NULL;
}

void CDisplayUdevMonitor::Stop()
{
  if (udev)
  {
    g_fdEventMonitor.RemoveFD(m_fdMonitorId);

    udev_monitor_unref(udevMonitor);
    udevMonitor = NULL;
    udev_unref(udev);
    udev = NULL;
  }
}

void CDisplayUdevMonitor::FDEventCallback(int id, int fd, short revents, void* data)
{
  struct udev_monitor* udevMonitor = (struct udev_monitor*)data;
  struct udev_device* device;

  while ((device = udev_monitor_receive_device(udevMonitor)) != NULL)
  {
    const char* action = udev_device_get_action(device);

    CLog::Log(LOGERROR, "CDisplayUdevMonitor - Action %s (\"%s\", \"%s\")", action,
              udev_device_get_syspath(device), udev_device_get_devpath(device));

    // There was a change of the HDMI device.
    // Reenumerate audio devices in case there is
    // a change in audio capabilities.
    CServiceBroker::GetActiveAE()->DeviceChange();

    udev_device_unref(device);
  }
}

} // namespace GBM
} // namespace WINDOWING
} // namespace KODI
