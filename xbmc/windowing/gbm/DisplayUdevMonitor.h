/*
 *  Copyright (C) 2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <string>
#include <vector>

namespace KODI
{
namespace WINDOWING
{
namespace GBM
{

class CDisplayUdevMonitor
{
public:
  CDisplayUdevMonitor();
  ~CDisplayUdevMonitor();

  void Start();
  void Stop();

private:
  static void FDEventCallback(int id, int fd, short revents, void* data);

  int m_fdMonitorId;
};

} // namespace GBM
} // namespace WINDOWING
} // namespace KODI