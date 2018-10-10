/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

class CWinSystemGbm;

class CHdmiMonitor
{
public:
  CHdmiMonitor(CWinSystemGbm &winSys);
  ~CHdmiMonitor();

  void Start();
  void Stop();

  CWinSystemGbm& GetWinSystem() { return m_winSystem; }

private:
  static void FDEventCallback(int id, int fd, short revents, void *data);

  int m_fdMonitorId;
  struct udev *m_udev;
  struct udev_monitor* m_udevMonitor;
  CWinSystemGbm &m_winSystem;

  typedef struct MonitorData
  {
    struct udev_monitor *udev_monitor;
    class CHdmiMonitor *hdmi_monitor;
  } TMonitorData;

  TMonitorData m_data;
};
