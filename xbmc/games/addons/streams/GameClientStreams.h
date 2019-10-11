/*
 *  Copyright (C) 2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addons/kodi-addon-dev-kit/include/kodi/kodi_game_types.h"
#include "cores/RetroPlayer/streams/RetroPlayerStreamTypes.h"

#include <map>

namespace KODI
{
namespace RETRO
{
  class IStreamManager;
}

namespace GAME
{

class CGameClient;
class IGameClientStream;
class IRetroPlayerStream;

class CGameClientStreams
{
public:
  CGameClientStreams(CGameClient &gameClient);

  void Initialize(RETRO::IStreamManager& streamManager);
  void Deinitialize();

  IGameClientStream* CreateStream(const game_stream_properties &properties);
  bool OpenStream(IGameClientStream*stream, const game_stream_properties &properties);
  void CloseStream(IGameClientStream* stream);

  game_proc_address_t GetHwProcedureAddress(const char *sym);

private:
  // Utility functions
  std::unique_ptr<IGameClientStream> CreateStream(GAME_STREAM_TYPE streamType) const;

  // Construction parameters
  CGameClient& m_gameClient;

  // Initialization parameters
  RETRO::IStreamManager* m_streamManager = nullptr;

  // Stream parameters
  std::map<IGameClientStream*, RETRO::StreamPtr> m_streams;

  RETRO::StreamPtr m_mystream;
};

} // namespace GAME
} // namespace KODI
