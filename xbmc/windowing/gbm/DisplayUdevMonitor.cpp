/*
 *      Copyright (C) 2018 Team Kodi
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

#include <libudev.h>
#include "DisplayUdevMonitor.h"
#include "messaging/ApplicationMessenger.h"
#include "platform/linux/FDEventMonitor.h"
#include "utils/log.h"
#include "ServiceBroker.h"
#include "windowing/WinSystem.h"
#include "rendering/gles/RenderSystemGLES.h"

using namespace KODI::MESSAGING;

CDisplayUdevMonitor::CDisplayUdevMonitor() :
  m_fdMonitorId(0),
  m_udev(NULL),
  m_udevMonitor(NULL)
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

  if (!m_udev)
  {
    m_udev = udev_new();
    if (!m_udev)
    {
      CLog::Log(LOGWARNING, "CDisplayUdevMonitor::Start - Unable to open udev handle");
      return;
    }

    m_udevMonitor = udev_monitor_new_from_netlink(m_udev, "udev");
    if (!m_udevMonitor)
    {
      CLog::Log(LOGERROR, "CDisplayUdevMonitor::Start - udev_monitor_new_from_netlink() failed");
      goto err_unref_udev;
    }

    err = udev_monitor_filter_add_match_subsystem_devtype(m_udevMonitor, "drm", NULL);
    if (err)
    {
      CLog::Log(LOGERROR, "CDisplayUdevMonitor::Start - udev_monitor_filter_add_match_subsystem_devtype() failed");
      goto err_unref_monitor;
    }

    err = udev_monitor_enable_receiving(m_udevMonitor);
    if (err)
    {
      CLog::Log(LOGERROR, "CDisplayUdevMonitor::Start - udev_monitor_enable_receiving() failed");
      goto err_unref_monitor;
    }

    g_fdEventMonitor.AddFD(
        CFDEventMonitor::MonitoredFD(udev_monitor_get_fd(m_udevMonitor),
                                     EPOLLIN, FDEventCallback, m_udevMonitor),
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

void CDisplayUdevMonitor::Stop()
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

void CDisplayUdevMonitor::FDEventCallback(int id, int fd, short revents, void *data)
{
  struct udev_monitor *udevMonitor = (struct udev_monitor *)data;
  struct udev_device *device;

  while ((device = udev_monitor_receive_device(udevMonitor)) != NULL)
  {
    const char* action = udev_device_get_action(device);
    //const char* soundInitialized = udev_device_get_property_value(device, "SOUND_INITIALIZED");

    CLog::Log(LOGERROR, "CDisplayUdevMonitor - Action %s (\"%s\", \"%s\")", action,
                                                                            udev_device_get_syspath(device),
                                                                            udev_device_get_devpath(device));

    CApplicationMessenger::GetInstance().PostMsg(TMSG_RENDERER_REINIT);

    udev_device_unref(device);
  }
}
