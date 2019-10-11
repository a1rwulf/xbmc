/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#ifdef BUILD_KODI_ADDON
#include "RecursiveMutex.h"
#include "Lockables.h"
#else
#include "platform/RecursiveMutex.h"
#include "threads/Lockables.h"
#endif



class CCriticalSection : public XbmcThreads::CountingLockable<XbmcThreads::CRecursiveMutex> {};
