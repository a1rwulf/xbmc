/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "PlayerOperations.h"
#include "Application.h"
#include "PlayListPlayer.h"
#include "guilib/GUIWindowManager.h"
#include "input/Key.h"
#include "GUIUserMessages.h"
#include "pictures/GUIWindowSlideShow.h"
#include "interfaces/builtins/Builtins.h"
#include "PartyModeManager.h"
#include "messaging/ApplicationMessenger.h"
#include "FileItem.h"
#include "VideoLibrary.h"
#include "video/VideoDatabase.h"
#include "AudioLibrary.h"
#include "GUIInfoManager.h"
#include "music/MusicDatabase.h"
#include "pvr/PVRManager.h"
#include "pvr/channels/PVRChannel.h"
#include "pvr/channels/PVRChannelGroupsContainer.h"
#include "pvr/epg/EpgInfoTag.h"
#include "pvr/recordings/PVRRecordings.h"
#include "cores/IPlayer.h"
#include "cores/playercorefactory/PlayerCoreFactory.h"
#include "SeekHandler.h"
#include "utils/Variant.h"
#include "Util.h"

using namespace JSONRPC;
using namespace PLAYLIST;
using namespace PVR;
using namespace KODI::MESSAGING;

JSONRPC_STATUS CPlayerOperations::GetActivePlayers(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  int activePlayers = GetActivePlayers();
  result = CVariant(CVariant::VariantTypeArray);

  std::string strPlayerType = "internal";
  if (activePlayers & External)
    strPlayerType = "external";
  else if (activePlayers & Remote)
    strPlayerType = "remote";

  if (activePlayers & Video)
  {
    CVariant video = CVariant(CVariant::VariantTypeObject);
    video["playerid"] = GetPlaylist(Video);
    video["type"] = "video";
    video["playertype"] = strPlayerType;
    result.append(video);
  }
  if (activePlayers & Audio)
  {
    CVariant audio = CVariant(CVariant::VariantTypeObject);
    audio["playerid"] = GetPlaylist(Audio);
    audio["type"] = "audio";
    audio["playertype"] = strPlayerType;
    result.append(audio);
  }
  if (activePlayers & Picture)
  {
    CVariant picture = CVariant(CVariant::VariantTypeObject);
    picture["playerid"] = GetPlaylist(Picture);
    picture["type"] = "picture";
    picture["playertype"] = "internal";
    result.append(picture);
  }

  return OK;
}

JSONRPC_STATUS CPlayerOperations::GetPlayers(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  const CPlayerCoreFactory &playerCoreFactory = CServiceBroker::GetPlayerCoreFactory();

  std::string media = parameterObject["media"].asString();
  result = CVariant(CVariant::VariantTypeArray);
  std::vector<std::string> players;

  if (media == "all")
  {
    playerCoreFactory.GetPlayers(players);
  }
  else
  {
    bool video = false;
    if (media == "video")
      video = true;
    playerCoreFactory.GetPlayers(players, true, video);
  }

  for (auto playername: players)
  {
    CVariant player(CVariant::VariantTypeObject);
    player["name"] = playername;

    player["playsvideo"] = playerCoreFactory.PlaysVideo(playername);
    player["playsaudio"] = playerCoreFactory.PlaysAudio(playername);
    player["type"] = playerCoreFactory.GetPlayerType(playername);

    result.push_back(player);
  }

  return OK;
}

JSONRPC_STATUS CPlayerOperations::GetProperties(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  PlayerType player = GetPlayer(parameterObject["playerid"]);

  CVariant properties = CVariant(CVariant::VariantTypeObject);
  for (unsigned int index = 0; index < parameterObject["properties"].size(); index++)
  {
    std::string propertyName = parameterObject["properties"][index].asString();
    CVariant property;
    JSONRPC_STATUS ret;
    if ((ret = GetPropertyValue(player, propertyName, property)) != OK)
      return ret;

    properties[propertyName] = property;
  }

  result = properties;

  return OK;
}

JSONRPC_STATUS CPlayerOperations::GetItem(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  PlayerType player = GetPlayer(parameterObject["playerid"]);
  CFileItemPtr fileItem;

  switch (player)
  {
    case Video:
    case Audio:
    {
      fileItem = CFileItemPtr(new CFileItem(g_application.CurrentFileItem()));
      if (IsPVRChannel())
      {
        CPVRChannelPtr currentChannel(CServiceBroker::GetPVRManager().GetPlayingChannel());
        if (currentChannel)
          fileItem = CFileItemPtr(new CFileItem(currentChannel));
      }
      else if (player == Video)
      {
        if (!CVideoLibrary::FillFileItem(fileItem->GetPath(), fileItem, parameterObject))
        {
          // Fallback to item details held by GUI but ensure path unchanged
          //! @todo  remove this once there is no route to playback that updates
          // GUI item without also updating app item e.g. start playback of a
          // non-library item via JSON 
          const CVideoInfoTag *currentVideoTag = CServiceBroker::GetGUI()->GetInfoManager().GetCurrentMovieTag();
          if (currentVideoTag != NULL)
          {
            std::string originalLabel = fileItem->GetLabel();
            std::string originalPath = fileItem->GetPath();
            fileItem->SetFromVideoInfoTag(*currentVideoTag);
            if (fileItem->GetLabel().empty())
              fileItem->SetLabel(originalLabel);
            fileItem->SetPath(originalPath);   // Ensure path unchanged
          }
        }
      }
      else
      {
        if (!CAudioLibrary::FillFileItem(fileItem->GetPath(), fileItem, parameterObject))
        {
          // Fallback to item details held by GUI but ensure path unchanged
          //! @todo  remove this once there is no route to playback that updates
          // GUI item without also updating app item e.g. start playback of a
          // non-library item via JSON 
          const MUSIC_INFO::CMusicInfoTag *currentMusicTag = CServiceBroker::GetGUI()->GetInfoManager().GetCurrentSongTag();
          if (currentMusicTag != NULL)
          {
            std::string originalLabel = fileItem->GetLabel();
            std::string originalPath = fileItem->GetPath();
            fileItem->SetFromMusicInfoTag(*currentMusicTag);
            if (fileItem->GetLabel().empty())
              fileItem->SetLabel(originalLabel);
            fileItem->SetPath(originalPath);   // Ensure path unchanged
          }
        }
      }

      if (IsPVRChannel())
        break;

      if (player == Video)
      {
        bool additionalInfo = false;
        for (CVariant::const_iterator_array itr = parameterObject["properties"].begin_array(); itr != parameterObject["properties"].end_array(); itr++)
        {
          std::string fieldValue = itr->asString();
          if (fieldValue == "cast" || fieldValue == "set" || fieldValue == "setid" || fieldValue == "showlink" || fieldValue == "resume" ||
             (fieldValue == "streamdetails" && !fileItem->GetVideoInfoTag()->m_streamDetails.HasItems()))
            additionalInfo = true;
        }

        CVideoDatabase videodatabase;
        if ((additionalInfo) &&
            videodatabase.Open())
        {
          if (additionalInfo)
          {
            switch (fileItem->GetVideoContentType())
            {
              case VIDEODB_CONTENT_MOVIES:
                videodatabase.GetMovieInfo("", *(fileItem->GetVideoInfoTag()), fileItem->GetVideoInfoTag()->m_iDbId);
                break;

              case VIDEODB_CONTENT_MUSICVIDEOS:
                videodatabase.GetMusicVideoInfo("", *(fileItem->GetVideoInfoTag()), fileItem->GetVideoInfoTag()->m_iDbId);
                break;

              case VIDEODB_CONTENT_EPISODES:
                videodatabase.GetEpisodeInfo("", *(fileItem->GetVideoInfoTag()), fileItem->GetVideoInfoTag()->m_iDbId);
                break;

              case VIDEODB_CONTENT_TVSHOWS:
              case VIDEODB_CONTENT_MOVIE_SETS:
              default:
                break;
            }
          }

          videodatabase.Close();
        }
      }
      else if (player == Audio)
      {
        if (fileItem->IsMusicDb())
        {
          CMusicDatabase musicdb;
          CFileItemList items;
          items.Add(fileItem);
          CAudioLibrary::GetAdditionalSongDetails(parameterObject, items, musicdb);
        }
      }
      break;
    }

    case Picture:
    {
      CGUIWindowSlideShow *slideshow = CServiceBroker::GetGUI()->GetWindowManager().GetWindow<CGUIWindowSlideShow>(WINDOW_SLIDESHOW);
      if (!slideshow)
        return FailedToExecute;

      CFileItemList slides;
      slideshow->GetSlideShowContents(slides);
      fileItem = slides[slideshow->CurrentSlide() - 1];
      break;
    }

    case None:
    default:
      return FailedToExecute;
  }

  HandleFileItem("id", !IsPVRChannel(), "item", fileItem, parameterObject, parameterObject["properties"], result, false);
  return OK;
}

JSONRPC_STATUS CPlayerOperations::PlayPause(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  CGUIWindowSlideShow *slideshow = NULL;
  switch (GetPlayer(parameterObject["playerid"]))
  {
    case Video:
    case Audio:
      if (!g_application.GetAppPlayer().CanPause())
        return FailedToExecute;

      if (parameterObject["play"].isString())
        CBuiltins::GetInstance().Execute("playercontrol(play)");
      else
      {
        if (parameterObject["play"].asBoolean())
        {
          if (g_application.GetAppPlayer().IsPausedPlayback())
            CApplicationMessenger::GetInstance().SendMsg(TMSG_MEDIA_PAUSE);
          else if (g_application.GetAppPlayer().GetPlaySpeed() != 1)
            g_application.GetAppPlayer().SetPlaySpeed(1);
        }
        else if (!g_application.GetAppPlayer().IsPausedPlayback())
          CApplicationMessenger::GetInstance().SendMsg(TMSG_MEDIA_PAUSE);
      }
      result["speed"] = g_application.GetAppPlayer().IsPausedPlayback() ? 0 : (int)lrint(g_application.GetAppPlayer().GetPlaySpeed());
      return OK;

    case Picture:
      slideshow = CServiceBroker::GetGUI()->GetWindowManager().GetWindow<CGUIWindowSlideShow>(WINDOW_SLIDESHOW);
      if (slideshow && slideshow->IsPlaying() &&
         (parameterObject["play"].isString() ||
         (parameterObject["play"].isBoolean() && parameterObject["play"].asBoolean() == slideshow->IsPaused())))
        SendSlideshowAction(ACTION_PAUSE);

      if (slideshow && slideshow->IsPlaying() && !slideshow->IsPaused())
        result["speed"] = slideshow->GetDirection();
      else
        result["speed"] = 0;
      return OK;

    case None:
    default:
      return FailedToExecute;
  }
}

JSONRPC_STATUS CPlayerOperations::Stop(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  switch (GetPlayer(parameterObject["playerid"]))
  {
    case Video:
    case Audio:
      CApplicationMessenger::GetInstance().SendMsg(TMSG_MEDIA_STOP, static_cast<int>(parameterObject["playerid"].asInteger()));
      return ACK;

    case Picture:
      SendSlideshowAction(ACTION_STOP);
      return ACK;

    case None:
    default:
      return FailedToExecute;
  }
}

JSONRPC_STATUS CPlayerOperations::SetSpeed(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  switch (GetPlayer(parameterObject["playerid"]))
  {
    case Video:
    case Audio:
      if (parameterObject["speed"].isInteger())
      {
        int speed = (int)parameterObject["speed"].asInteger();
        if (speed != 0)
        {
          // If the player is paused we first need to unpause
          if (g_application.GetAppPlayer().IsPausedPlayback())
            g_application.GetAppPlayer().Pause();
          g_application.GetAppPlayer().SetPlaySpeed(speed);
        }
        else
          g_application.GetAppPlayer().Pause();
      }
      else if (parameterObject["speed"].isString())
      {
        if (parameterObject["speed"].asString().compare("increment") == 0)
          CBuiltins::GetInstance().Execute("playercontrol(forward)");
        else
          CBuiltins::GetInstance().Execute("playercontrol(rewind)");
      }
      else
        return InvalidParams;

      result["speed"] = g_application.GetAppPlayer().IsPausedPlayback() ? 0 : (int)lrint(g_application.GetAppPlayer().GetPlaySpeed());
      return OK;

    case Picture:
    case None:
    default:
      return FailedToExecute;
  }
}

JSONRPC_STATUS CPlayerOperations::Seek(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  PlayerType player = GetPlayer(parameterObject["playerid"]);
  switch (player)
  {
    case Video:
    case Audio:
    {
      if (!g_application.GetAppPlayer().CanSeek())
        return FailedToExecute;

      const CVariant& value = parameterObject["value"];
      if (IsType(value, NumberValue) || value.isMember("percentage"))
        g_application.SeekPercentage(IsType(value, NumberValue) ? value.asFloat() : value["percentage"].asFloat());
      else if (value.isString() || value.isMember("step"))
      {
        std::string step = value.isString() ? value.asString() : value["step"].asString();
        if (step == "smallforward")
          CBuiltins::GetInstance().Execute("playercontrol(smallskipforward)");
        else if (step == "smallbackward")
          CBuiltins::GetInstance().Execute("playercontrol(smallskipbackward)");
        else if (step == "bigforward")
          CBuiltins::GetInstance().Execute("playercontrol(bigskipforward)");
        else if (step == "bigbackward")
          CBuiltins::GetInstance().Execute("playercontrol(bigskipbackward)");
        else
          return InvalidParams;
      }
      else if (value.isMember("seconds") && value.size() == 1)
        g_application.GetAppPlayer().GetSeekHandler().SeekSeconds(static_cast<int>(value["seconds"].asInteger()));
      else if (value.isObject())
        g_application.SeekTime(ParseTimeInSeconds(value.isMember("time") ? value["time"] : value));
      else
        return InvalidParams;

      GetPropertyValue(player, "percentage", result["percentage"]);
      GetPropertyValue(player, "time", result["time"]);
      GetPropertyValue(player, "totaltime", result["totaltime"]);
      return OK;
    }

    case Picture:
    case None:
    default:
      return FailedToExecute;
  }
}

JSONRPC_STATUS CPlayerOperations::Move(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  std::string direction = parameterObject["direction"].asString();
  switch (GetPlayer(parameterObject["playerid"]))
  {
    case Picture:
      if (direction == "left")
        SendSlideshowAction(ACTION_MOVE_LEFT);
      else if (direction == "right")
        SendSlideshowAction(ACTION_MOVE_RIGHT);
      else if (direction == "up")
        SendSlideshowAction(ACTION_MOVE_UP);
      else if (direction == "down")
        SendSlideshowAction(ACTION_MOVE_DOWN);
      else
        return InvalidParams;

      return ACK;

    case Video:
    case Audio:
      if (direction == "left" || direction == "up")
        CApplicationMessenger::GetInstance().SendMsg(TMSG_GUI_ACTION, WINDOW_INVALID, -1, static_cast<void*>(new CAction(ACTION_PREV_ITEM)));
      else if (direction == "right" || direction == "down")
        CApplicationMessenger::GetInstance().SendMsg(TMSG_GUI_ACTION, WINDOW_INVALID, -1, static_cast<void*>(new CAction(ACTION_NEXT_ITEM)));
      else
        return InvalidParams;

      return ACK;

    case None:
    default:
      return FailedToExecute;
  }
}

JSONRPC_STATUS CPlayerOperations::Zoom(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  CVariant zoom = parameterObject["zoom"];
  switch (GetPlayer(parameterObject["playerid"]))
  {
    case Picture:
      if (zoom.isInteger())
        SendSlideshowAction(ACTION_ZOOM_LEVEL_NORMAL + ((int)zoom.asInteger() - 1));
      else if (zoom.isString())
      {
        std::string strZoom = zoom.asString();
        if (strZoom == "in")
          SendSlideshowAction(ACTION_ZOOM_IN);
        else if (strZoom == "out")
          SendSlideshowAction(ACTION_ZOOM_OUT);
        else
          return InvalidParams;
      }
      else
        return InvalidParams;

      return ACK;

    case Video:
    case Audio:
    case None:
    default:
      return FailedToExecute;
  }
}

JSONRPC_STATUS CPlayerOperations::Rotate(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  switch (GetPlayer(parameterObject["playerid"]))
  {
    case Picture:
      if (parameterObject["value"].asString().compare("clockwise") == 0)
        SendSlideshowAction(ACTION_ROTATE_PICTURE_CW);
      else
        SendSlideshowAction(ACTION_ROTATE_PICTURE_CCW);
      return ACK;

    case Video:
    case Audio:
    case None:
    default:
      return FailedToExecute;
  }
}

JSONRPC_STATUS CPlayerOperations::Open(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  CVariant options = parameterObject["options"];
  CVariant optionShuffled = options["shuffled"];
  CVariant optionRepeat = options["repeat"];
  CVariant optionResume = options["resume"];
  CVariant optionPlayer = options["playername"];

  if (parameterObject["item"].isMember("playlistid"))
  {
    int playlistid = (int)parameterObject["item"]["playlistid"].asInteger();
    int playlistStartPosition = (int)parameterObject["item"]["position"].asInteger();

    if (playlistid < PLAYLIST_PICTURE)
    {
      // Apply the "shuffled" option if available
      if (optionShuffled.isBoolean())
        CServiceBroker::GetPlaylistPlayer().SetShuffle(playlistid, optionShuffled.asBoolean(), false);
      // Apply the "repeat" option if available
      if (!optionRepeat.isNull())
        CServiceBroker::GetPlaylistPlayer().SetRepeat(playlistid, (REPEAT_STATE)ParseRepeatState(optionRepeat), false);
      
      if (optionResume.isBoolean() && optionResume.asBoolean())
        CServiceBroker::GetPlaylistPlayer().SetSongResume(playlistid, playlistStartPosition, STARTOFFSET_RESUME);
      else if (optionResume.isObject())
        CServiceBroker::GetPlaylistPlayer().SetSongResume(playlistid, playlistStartPosition, (int)CUtil::ConvertSecsToMilliSecs(ParseTimeInSeconds(optionResume)));
      else if (optionResume.isInteger())
        CServiceBroker::GetPlaylistPlayer().SetSongResume(playlistid, playlistStartPosition, (int)CUtil::ConvertSecsToMilliSecs(optionResume.asInteger()));
      
    }

    switch (playlistid)
    {
      case PLAYLIST_MUSIC:
      case PLAYLIST_VIDEO:
        CApplicationMessenger::GetInstance().SendMsg(TMSG_MEDIA_PLAY, playlistid, playlistStartPosition);
        OnPlaylistChanged();
        break;

      case PLAYLIST_PICTURE:
      {
        std::string firstPicturePath;
        if (playlistStartPosition > 0)
        {
          CGUIWindowSlideShow *slideshow = CServiceBroker::GetGUI()->GetWindowManager().GetWindow<CGUIWindowSlideShow>(WINDOW_SLIDESHOW);
          if (slideshow != NULL)
          {
            CFileItemList list;
            slideshow->GetSlideShowContents(list);
            if (playlistStartPosition < list.Size())
              firstPicturePath = list.Get(playlistStartPosition)->GetPath();
          }
        }

        return StartSlideshow("", false, optionShuffled.isBoolean() && optionShuffled.asBoolean(), firstPicturePath);
        break;
      }
    }

    return ACK;
  }
  else if (parameterObject["item"].isMember("path"))
  {
    bool random = (optionShuffled.isBoolean() && optionShuffled.asBoolean()) ||
                  (!optionShuffled.isBoolean() && parameterObject["item"]["random"].asBoolean());
    return StartSlideshow(parameterObject["item"]["path"].asString(), parameterObject["item"]["recursive"].asBoolean(), random);
  }
  else if (parameterObject["item"].isObject() && parameterObject["item"].isMember("partymode"))
  {
    if (g_partyModeManager.IsEnabled())
      g_partyModeManager.Disable();
    CApplicationMessenger::GetInstance().SendMsg(TMSG_EXECUTE_BUILT_IN, -1, -1, nullptr, "playercontrol(partymode(" + parameterObject["item"]["partymode"].asString() + "))");
    return ACK;
  }
  else if (parameterObject["item"].isMember("channelid"))
  {
    if (!CServiceBroker::GetPVRManager().IsStarted())
      return FailedToExecute;

    CPVRChannelGroupsContainerPtr channelGroupContainer = CServiceBroker::GetPVRManager().ChannelGroups();
    if (!channelGroupContainer)
      return FailedToExecute;

    CPVRChannelPtr channel = channelGroupContainer->GetChannelById((int)parameterObject["item"]["channelid"].asInteger());
    if (channel == NULL)
      return InvalidParams;

    CFileItemList *l = new CFileItemList; //don't delete,
    l->Add(std::make_shared<CFileItem>(channel));
    CApplicationMessenger::GetInstance().PostMsg(TMSG_MEDIA_PLAY, -1, -1, static_cast<void*>(l));

    return ACK;
  }
  else if (parameterObject["item"].isMember("recordingid"))
  {
    if (!CServiceBroker::GetPVRManager().IsStarted())
      return FailedToExecute;

    CPVRRecordingsPtr recordingsContainer = CServiceBroker::GetPVRManager().Recordings();
    if (!recordingsContainer)
      return FailedToExecute;

    CFileItemPtr fileItem = recordingsContainer->GetById((int)parameterObject["item"]["recordingid"].asInteger());
    if (fileItem == NULL)
      return InvalidParams;

    CFileItemList *l = new CFileItemList; //don't delete,
    l->Add(std::make_shared<CFileItem>(*fileItem));
    CApplicationMessenger::GetInstance().PostMsg(TMSG_MEDIA_PLAY, -1, -1, static_cast<void*>(l));

    return ACK;
  }
  else
  {
    CFileItemList list;
    if (FillFileItemList(parameterObject["item"], list) && list.Size() > 0)
    {
      bool slideshow = true;
      for (int index = 0; index < list.Size(); index++)
      {
        if (!list[index]->IsPicture())
        {
          slideshow = false;
          break;
        }
      }

      if (slideshow)
      {
        CGUIWindowSlideShow *slideshow = CServiceBroker::GetGUI()->GetWindowManager().GetWindow<CGUIWindowSlideShow>(WINDOW_SLIDESHOW);
        if (!slideshow)
          return FailedToExecute;

        SendSlideshowAction(ACTION_STOP);
        slideshow->Reset();
        for (int index = 0; index < list.Size(); index++)
          slideshow->Add(list[index].get());

        return StartSlideshow("", false, optionShuffled.isBoolean() && optionShuffled.asBoolean());
      }
      else
      {
        std::string playername;
        // Handle the "playerid" option
        if (!optionPlayer.isNull())
        {
          if (optionPlayer.isString())
          {
            playername = optionPlayer.asString();

            if (playername != "default")
            {
              const CPlayerCoreFactory &playerCoreFactory = CServiceBroker::GetPlayerCoreFactory();

              // check if the there's actually a player with the given name
              if (playerCoreFactory.GetPlayerType(playername).empty())
                return InvalidParams;

              // check if the player can handle at least the first item in the list
              std::vector<std::string> possiblePlayers;
              playerCoreFactory.GetPlayers(*list.Get(0).get(), possiblePlayers);

              bool match = false;
              for (auto entry : possiblePlayers)
              {
                if (StringUtils::EqualsNoCase(entry, playername))
                {
                  match = true;
                  break;
                }
              }
              if (!match)
                return InvalidParams;
            }
          }
          else
            return InvalidParams;
        }

        // Handle "shuffled" option
        if (optionShuffled.isBoolean())
          list.SetProperty("shuffled", optionShuffled);
        // Handle "repeat" option
        if (!optionRepeat.isNull())
          list.SetProperty("repeat", ParseRepeatState(optionRepeat));
        // Handle "resume" option
        if (list.Size() == 1)
        {
          if (optionResume.isBoolean() && optionResume.asBoolean())
            list[0]->m_lStartOffset = STARTOFFSET_RESUME;
          else if (optionResume.isDouble())
            list[0]->SetProperty("StartPercent", optionResume);
          else if (optionResume.isObject())
            list[0]->m_lStartOffset = CUtil::ConvertSecsToMilliSecs(ParseTimeInSeconds(optionResume));
        }

        auto l = new CFileItemList(); //don't delete
        l->Copy(list);
        CApplicationMessenger::GetInstance().SendMsg(TMSG_MEDIA_PLAY, -1, -1, static_cast<void*>(l), playername);
      }

      return ACK;
    }
    else
      return InvalidParams;
  }

  return InvalidParams;
}

JSONRPC_STATUS CPlayerOperations::GoTo(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  CVariant to = parameterObject["to"];
  switch (GetPlayer(parameterObject["playerid"]))
  {
    case Video:
    case Audio:
      if (to.isString())
      {
        std::string strTo = to.asString();
        int actionID;
        if (strTo == "previous")
          actionID = ACTION_PREV_ITEM;
        else if (strTo == "next")
          actionID = ACTION_NEXT_ITEM;
        else
          return InvalidParams;

        CApplicationMessenger::GetInstance().SendMsg(TMSG_GUI_ACTION, WINDOW_INVALID, -1, static_cast<void*>(new CAction(actionID)));
      }
      else if (to.isInteger())
      {
        if (IsPVRChannel())
          CApplicationMessenger::GetInstance().SendMsg(TMSG_GUI_ACTION, WINDOW_INVALID, -1, static_cast<void*>(
            new CAction(ACTION_CHANNEL_SWITCH, static_cast<float>(to.asInteger()))));
        else
          CApplicationMessenger::GetInstance().SendMsg(TMSG_PLAYLISTPLAYER_PLAY, static_cast<int>(to.asInteger()));
      }
      else
        return InvalidParams;
      break;

    case Picture:
      if (to.isString())
      {
        std::string strTo = to.asString();
        int actionID;
        if (strTo == "previous")
          actionID = ACTION_PREV_PICTURE;
        else if (strTo == "next")
          actionID = ACTION_NEXT_PICTURE;
        else
          return InvalidParams;

        SendSlideshowAction(actionID);
      }
      else
        return FailedToExecute;
      break;

    case None:
    default:
      return FailedToExecute;
  }

  OnPlaylistChanged();
  return ACK;
}

JSONRPC_STATUS CPlayerOperations::SetShuffle(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  CGUIWindowSlideShow *slideshow = NULL;
  CVariant shuffle = parameterObject["shuffle"];
  switch (GetPlayer(parameterObject["playerid"]))
  {
    case Video:
    case Audio:
    {
      if (IsPVRChannel())
        return FailedToExecute;

      int playlistid = GetPlaylist(GetPlayer(parameterObject["playerid"]));
      if (CServiceBroker::GetPlaylistPlayer().IsShuffled(playlistid))
      {
        if ((shuffle.isBoolean() && !shuffle.asBoolean()) ||
            (shuffle.isString() && shuffle.asString() == "toggle"))
        {
          CApplicationMessenger::GetInstance().SendMsg(TMSG_PLAYLISTPLAYER_SHUFFLE, playlistid, 0);
          OnPlaylistChanged();
        }
      }
      else
      {
        if ((shuffle.isBoolean() && shuffle.asBoolean()) ||
            (shuffle.isString() && shuffle.asString() == "toggle"))
        {
          CApplicationMessenger::GetInstance().SendMsg(TMSG_PLAYLISTPLAYER_SHUFFLE, playlistid, 1);
          OnPlaylistChanged();
        }
      }
      break;
    }

    case Picture:
      slideshow = CServiceBroker::GetGUI()->GetWindowManager().GetWindow<CGUIWindowSlideShow>(WINDOW_SLIDESHOW);
      if (slideshow == NULL)
        return FailedToExecute;
      if (slideshow->IsShuffled())
      {
        if ((shuffle.isBoolean() && !shuffle.asBoolean()) ||
            (shuffle.isString() && shuffle.asString() == "toggle"))
          return FailedToExecute;
      }
      else
      {
        if ((shuffle.isBoolean() && shuffle.asBoolean()) ||
            (shuffle.isString() && shuffle.asString() == "toggle"))
          slideshow->Shuffle();
      }
      break;

    default:
      return FailedToExecute;
  }
  return ACK;
}

JSONRPC_STATUS CPlayerOperations::SetRepeat(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  switch (GetPlayer(parameterObject["playerid"]))
  {
    case Video:
    case Audio:
    {
      if (IsPVRChannel())
        return FailedToExecute;

      REPEAT_STATE repeat = REPEAT_NONE;
      int playlistid = GetPlaylist(GetPlayer(parameterObject["playerid"]));
      if (parameterObject["repeat"].asString() == "cycle")
      {
        REPEAT_STATE repeatPrev = CServiceBroker::GetPlaylistPlayer().GetRepeat(playlistid);
        if (repeatPrev == REPEAT_NONE)
          repeat = REPEAT_ALL;
        else if (repeatPrev == REPEAT_ALL)
          repeat = REPEAT_ONE;
        else
          repeat = REPEAT_NONE;
      }
      else
        repeat = (REPEAT_STATE)ParseRepeatState(parameterObject["repeat"]);

      CApplicationMessenger::GetInstance().SendMsg(TMSG_PLAYLISTPLAYER_REPEAT, playlistid, repeat);
      OnPlaylistChanged();
      break;
    }

    case Picture:
    default:
      return FailedToExecute;
  }

  return ACK;
}

JSONRPC_STATUS CPlayerOperations::SetPartymode(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  PlayerType player = GetPlayer(parameterObject["playerid"]);
  switch (player)
  {
    case Video:
    case Audio:
    {
      if (IsPVRChannel())
        return FailedToExecute;

      bool change = false;
      PartyModeContext context = PARTYMODECONTEXT_UNKNOWN;
      std::string strContext;
      if (player == Video)
      {
        context = PARTYMODECONTEXT_VIDEO;
        strContext = "video";
      }
      else if (player == Audio)
      {
        context = PARTYMODECONTEXT_MUSIC;
        strContext = "music";
      }

      bool toggle = parameterObject["partymode"].isString();
      if (g_partyModeManager.IsEnabled())
      {
        if (g_partyModeManager.GetType() != context)
          return InvalidParams;

        if (toggle || parameterObject["partymode"].asBoolean() == false)
          change = true;
      }
      else
      {
        if (toggle || parameterObject["partymode"].asBoolean() == true)
          change = true;
      }

      if (change)
        CApplicationMessenger::GetInstance().SendMsg(TMSG_EXECUTE_BUILT_IN, -1, -1, nullptr, "playercontrol(partymode(" + strContext + "))");
      break;
    }

    case Picture:
    default:
      return FailedToExecute;
  }

  return ACK;
}

JSONRPC_STATUS CPlayerOperations::SetAudioStream(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  switch (GetPlayer(parameterObject["playerid"]))
  {
    case Video:
      if (g_application.GetAppPlayer().HasPlayer())
      {
        int index = -1;
        if (parameterObject["stream"].isString())
        {
          std::string action = parameterObject["stream"].asString();
          if (action.compare("previous") == 0)
          {
            index = g_application.GetAppPlayer().GetAudioStream() - 1;
            if (index < 0)
              index = g_application.GetAppPlayer().GetAudioStreamCount() - 1;
          }
          else if (action.compare("next") == 0)
          {
            index = g_application.GetAppPlayer().GetAudioStream() + 1;
            if (index >= g_application.GetAppPlayer().GetAudioStreamCount())
              index = 0;
          }
          else
            return InvalidParams;
        }
        else if (parameterObject["stream"].isInteger())
          index = (int)parameterObject["stream"].asInteger();

        if (index < 0 || g_application.GetAppPlayer().GetAudioStreamCount() <= index)
          return InvalidParams;

        g_application.GetAppPlayer().SetAudioStream(index);
      }
      else
        return FailedToExecute;
      break;

    case Audio:
    case Picture:
    default:
      return FailedToExecute;
  }

  return ACK;
}

JSONRPC_STATUS CPlayerOperations::SetSubtitle(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  switch (GetPlayer(parameterObject["playerid"]))
  {
    case Video:
      if (g_application.GetAppPlayer().HasPlayer())
      {
        int index = -1;
        if (parameterObject["subtitle"].isString())
        {
          std::string action = parameterObject["subtitle"].asString();
          if (action.compare("previous") == 0)
          {
            index = g_application.GetAppPlayer().GetSubtitle() - 1;
            if (index < 0)
              index = g_application.GetAppPlayer().GetSubtitleCount() - 1;
          }
          else if (action.compare("next") == 0)
          {
            index = g_application.GetAppPlayer().GetSubtitle() + 1;
            if (index >= g_application.GetAppPlayer().GetSubtitleCount())
              index = 0;
          }
          else if (action.compare("off") == 0)
          {
            g_application.GetAppPlayer().SetSubtitleVisible(false);
            return ACK;
          }
          else if (action.compare("on") == 0)
          {
            g_application.GetAppPlayer().SetSubtitleVisible(true);
            return ACK;
          }
          else
            return InvalidParams;
        }
        else if (parameterObject["subtitle"].isInteger())
          index = (int)parameterObject["subtitle"].asInteger();

        if (index < 0 || g_application.GetAppPlayer().GetSubtitleCount() <= index)
          return InvalidParams;

        g_application.GetAppPlayer().SetSubtitle(index);

        // Check if we need to enable subtitles to be displayed
        if (parameterObject["enable"].asBoolean() && !g_application.GetAppPlayer().GetSubtitleVisible())
          g_application.GetAppPlayer().SetSubtitleVisible(true);
      }
      else
        return FailedToExecute;
      break;

    case Audio:
    case Picture:
    default:
      return FailedToExecute;
  }

  return ACK;
}

JSONRPC_STATUS CPlayerOperations::SetVideoStream(const std::string &method, ITransportLayer *transport, IClient *client, const CVariant &parameterObject, CVariant &result)
{
  switch (GetPlayer(parameterObject["playerid"]))
  {
  case Video:
  {
    int streamCount = g_application.GetAppPlayer().GetVideoStreamCount();
    if (streamCount > 0)
    {
      int index = g_application.GetAppPlayer().GetVideoStream();
      if (parameterObject["stream"].isString())
      {
        std::string action = parameterObject["stream"].asString();
        if (action.compare("previous") == 0)
        {
          index--;
          if (index < 0)
            index = streamCount - 1;
        }
        else if (action.compare("next") == 0)
        {
          index++;
          if (index >= streamCount)
            index = 0;
        }
        else
          return InvalidParams;
      }
      else if (parameterObject["stream"].isInteger())
        index = (int)parameterObject["stream"].asInteger();

      if (index < 0 || streamCount <= index)
        return InvalidParams;

      g_application.GetAppPlayer().SetVideoStream(index);
    }
    else
      return FailedToExecute;
    break;
  }
  case Audio:
  case Picture:
  default:
    return FailedToExecute;
  }

  return ACK;
}

int CPlayerOperations::GetActivePlayers()
{
  int activePlayers = 0;

  if (g_application.GetAppPlayer().IsPlayingVideo() || CServiceBroker::GetPVRManager().IsPlayingTV() || CServiceBroker::GetPVRManager().IsPlayingRecording())
    activePlayers |= Video;
  if (g_application.GetAppPlayer().IsPlayingAudio() || CServiceBroker::GetPVRManager().IsPlayingRadio())
    activePlayers |= Audio;
  if (CServiceBroker::GetGUI()->GetWindowManager().IsWindowActive(WINDOW_SLIDESHOW))
    activePlayers |= Picture;
  if (g_application.GetAppPlayer().IsExternalPlaying())
    activePlayers |= External;
  if (g_application.GetAppPlayer().IsRemotePlaying())
    activePlayers |= Remote;

  return activePlayers;
}

PlayerType CPlayerOperations::GetPlayer(const CVariant &player)
{
  int iPlayer = (int)player.asInteger();
  PlayerType playerID;

  switch (iPlayer)
  {
    case PLAYLIST_VIDEO:
      playerID = Video;
      break;

    case PLAYLIST_MUSIC:
      playerID = Audio;
      break;

    case PLAYLIST_PICTURE:
      playerID = Picture;
      break;

    default:
      playerID = None;
      break;
  }

  if (GetPlaylist(playerID) == iPlayer)
    return playerID;
  else
    return None;
}

int CPlayerOperations::GetPlaylist(PlayerType player)
{
  int playlist = CServiceBroker::GetPlaylistPlayer().GetCurrentPlaylist();
  if (playlist == PLAYLIST_NONE) // No active playlist, try guessing
    playlist = g_application.GetAppPlayer().GetPreferredPlaylist();

  switch (player)
  {
    case Video:
      return playlist == PLAYLIST_NONE ? PLAYLIST_VIDEO : playlist;

    case Audio:
      return playlist == PLAYLIST_NONE ? PLAYLIST_MUSIC : playlist;

    case Picture:
      return PLAYLIST_PICTURE;

    default:
      return playlist;
  }
}

JSONRPC_STATUS CPlayerOperations::StartSlideshow(const std::string& path, bool recursive, bool random, const std::string &firstPicturePath /* = "" */)
{
  int flags = 0;
  if (recursive)
    flags |= 1;
  if (random)
    flags |= 2;
  else
    flags |= 4;

  std::vector<std::string> params;
  params.push_back(path);
  if (!firstPicturePath.empty())
    params.push_back(firstPicturePath);

  // Reset screensaver when started from JSON only to avoid potential conflict with slideshow screensavers
  g_application.ResetScreenSaver();
  g_application.WakeUpScreenSaverAndDPMS();
  CGUIMessage msg(GUI_MSG_START_SLIDESHOW, 0, 0, flags);
  msg.SetStringParams(params);
  CApplicationMessenger::GetInstance().SendGUIMessage(msg, WINDOW_SLIDESHOW);

  return ACK;
}

void CPlayerOperations::SendSlideshowAction(int actionID)
{
  CApplicationMessenger::GetInstance().SendMsg(TMSG_GUI_ACTION, WINDOW_SLIDESHOW, -1, static_cast<void*>(new CAction(actionID)));
}

void CPlayerOperations::OnPlaylistChanged()
{
  CGUIMessage msg(GUI_MSG_PLAYLIST_CHANGED, 0, 0);
  CServiceBroker::GetGUI()->GetWindowManager().SendThreadMessage(msg);
}

JSONRPC_STATUS CPlayerOperations::GetPropertyValue(PlayerType player, const std::string &property, CVariant &result)
{
  if (player == None)
    return FailedToExecute;

  int playlist = GetPlaylist(player);

  if (property == "type")
  {
    switch (player)
    {
      case Video:
        result = "video";
        break;

      case Audio:
        result = "audio";
        break;

      case Picture:
        result = "picture";
        break;

      default:
        return FailedToExecute;
    }
  }
  else if (property == "partymode")
  {
    switch (player)
    {
      case Video:
      case Audio:
        if (IsPVRChannel())
        {
          result = false;
          break;
        }

        result = g_partyModeManager.IsEnabled();
        break;

      case Picture:
        result = false;
        break;

      default:
        return FailedToExecute;
    }
  }
  else if (property == "speed")
  {
    CGUIWindowSlideShow *slideshow = NULL;
    switch (player)
    {
      case Video:
      case Audio:
        result = g_application.GetAppPlayer().IsPausedPlayback() ? 0 : (int)lrint(g_application.GetAppPlayer().GetPlaySpeed());
        break;

      case Picture:
        slideshow = CServiceBroker::GetGUI()->GetWindowManager().GetWindow<CGUIWindowSlideShow>(WINDOW_SLIDESHOW);
        if (slideshow && slideshow->IsPlaying() && !slideshow->IsPaused())
          result = slideshow->GetDirection();
        else
          result = 0;
        break;

      default:
        return FailedToExecute;
    }
  }
  else if (property == "time")
  {
    switch (player)
    {
      case Video:
      case Audio:
      {
        int ms = 0;
        if (!IsPVRChannel())
          ms = (int)(g_application.GetTime() * 1000.0);
        else
        {
          CPVREpgInfoTagPtr epg(GetCurrentEpg());
          if (epg)
            ms = epg->Progress() * 1000;
        }

        MillisecondsToTimeObject(ms, result);
        break;
      }

      case Picture:
        MillisecondsToTimeObject(0, result);
        break;

      default:
        return FailedToExecute;
    }
  }
  else if (property == "percentage")
  {
    CGUIWindowSlideShow *slideshow = NULL;
    switch (player)
    {
      case Video:
      case Audio:
      {
        if (!IsPVRChannel())
          result = g_application.GetPercentage();
        else
        {
          CPVREpgInfoTagPtr epg(GetCurrentEpg());
          if (epg)
            result = epg->ProgressPercentage();
          else
            result = 0;
        }
        break;
      }

      case Picture:
        slideshow = CServiceBroker::GetGUI()->GetWindowManager().GetWindow<CGUIWindowSlideShow>(WINDOW_SLIDESHOW);
        if (slideshow && slideshow->NumSlides() > 0)
          result = (double)slideshow->CurrentSlide() / slideshow->NumSlides();
        else
          result = 0.0;
        break;

      default:
        return FailedToExecute;
    }
  }
  else if (property == "totaltime")
  {
    switch (player)
    {
      case Video:
      case Audio:
      {
        int ms = 0;
        if (!IsPVRChannel())
          ms = (int)(g_application.GetTotalTime() * 1000.0);
        else
        {
          CPVREpgInfoTagPtr epg(GetCurrentEpg());
          if (epg)
            ms = epg->GetDuration() * 1000;
        }

        MillisecondsToTimeObject(ms, result);
        break;
      }

      case Picture:
        MillisecondsToTimeObject(0, result);
        break;

      default:
        return FailedToExecute;
    }
  }
  else if (property == "playlistid")
  {
    result = playlist;
  }
  else if (property == "position")
  {
    CGUIWindowSlideShow *slideshow = NULL;
    switch (player)
    {
      case Video:
      case Audio: /* Return the position of current item if there is an active playlist */
        if (!IsPVRChannel() && CServiceBroker::GetPlaylistPlayer().GetCurrentPlaylist() == playlist)
          result = CServiceBroker::GetPlaylistPlayer().GetCurrentSong();
        else
          result = -1;
        break;

      case Picture:
        slideshow = CServiceBroker::GetGUI()->GetWindowManager().GetWindow<CGUIWindowSlideShow>(WINDOW_SLIDESHOW);
        if (slideshow && slideshow->IsPlaying())
          result = slideshow->CurrentSlide() - 1;
        else
          result = -1;
        break;

      default:
        result = -1;
        break;
    }
  }
  else if (property == "repeat")
  {
    switch (player)
    {
      case Video:
      case Audio:
        if (IsPVRChannel())
        {
          result = "off";
          break;
        }

        switch (CServiceBroker::GetPlaylistPlayer().GetRepeat(playlist))
        {
        case REPEAT_ONE:
          result = "one";
          break;
        case REPEAT_ALL:
          result = "all";
          break;
        default:
          result = "off";
          break;
        }
        break;

      case Picture:
      default:
        result = "off";
        break;
    }
  }
  else if (property == "shuffled")
  {
    CGUIWindowSlideShow *slideshow = NULL;
    switch (player)
    {
      case Video:
      case Audio:
        if (IsPVRChannel())
        {
          result = false;
          break;
        }

        result = CServiceBroker::GetPlaylistPlayer().IsShuffled(playlist);
        break;

      case Picture:
        slideshow = CServiceBroker::GetGUI()->GetWindowManager().GetWindow<CGUIWindowSlideShow>(WINDOW_SLIDESHOW);
        if (slideshow && slideshow->IsPlaying())
          result = slideshow->IsShuffled();
        else
          result = -1;
        break;

      default:
        result = -1;
        break;
    }
  }
  else if (property == "canseek")
  {
    switch (player)
    {
      case Video:
      case Audio:
        result = g_application.GetAppPlayer().CanSeek();
        break;

      case Picture:
      default:
        result = false;
        break;
    }
  }
  else if (property == "canchangespeed")
  {
    switch (player)
    {
      case Video:
      case Audio:
        result = !IsPVRChannel();
        break;

      case Picture:
      default:
        result = false;
        break;
    }
  }
  else if (property == "canmove")
  {
    switch (player)
    {
      case Picture:
        result = true;
        break;

      case Video:
      case Audio:
      default:
        result = false;
        break;
    }
  }
  else if (property == "canzoom")
  {
    switch (player)
    {
      case Picture:
        result = true;
        break;

      case Video:
      case Audio:
      default:
        result = false;
        break;
    }
  }
  else if (property == "canrotate")
  {
    switch (player)
    {
      case Picture:
        result = true;
        break;

      case Video:
      case Audio:
      default:
        result = false;
        break;
    }
  }
  else if (property == "canshuffle")
  {
    switch (player)
    {
      case Video:
      case Audio:
      case Picture:
        result = !IsPVRChannel();
        break;

      default:
        result = false;
        break;
    }
  }
  else if (property == "canrepeat")
  {
    switch (player)
    {
      case Video:
      case Audio:
        result = !IsPVRChannel();
        break;

      case Picture:
      default:
        result = false;
        break;
    }
  }
  else if (property == "currentaudiostream")
  {
    switch (player)
    {
      case Video:
      case Audio:
        if (g_application.GetAppPlayer().HasPlayer())
        {
          result = CVariant(CVariant::VariantTypeObject);
          int index = g_application.GetAppPlayer().GetAudioStream();
          if (index >= 0)
          {
            AudioStreamInfo info;
            g_application.GetAppPlayer().GetAudioStreamInfo(index, info);

            result["index"] = index;
            result["name"] = info.name;
            result["language"] = info.language;
            result["codec"] = info.codecName;
            result["bitrate"] = info.bitrate;
            result["channels"] = info.channels;
          }
        }
        else
          result = CVariant(CVariant::VariantTypeNull);
        break;

      case Picture:
      default:
        result = CVariant(CVariant::VariantTypeNull);
        break;
    }
  }
  else if (property == "audiostreams")
  {
    result = CVariant(CVariant::VariantTypeArray);
    switch (player)
    {
      case Video:
        if (g_application.GetAppPlayer().HasPlayer())
        {
          for (int index = 0; index < g_application.GetAppPlayer().GetAudioStreamCount(); index++)
          {
            AudioStreamInfo info;
            g_application.GetAppPlayer().GetAudioStreamInfo(index, info);

            CVariant audioStream(CVariant::VariantTypeObject);
            audioStream["index"] = index;
            audioStream["name"] = info.name;
            audioStream["language"] = info.language;
            audioStream["codec"] = info.codecName;
            audioStream["bitrate"] = info.bitrate;
            audioStream["channels"] = info.channels;

            result.append(audioStream);
          }
        }
        break;

      case Audio:
      case Picture:
      default:
        break;
    }
  }
  else if (property == "currentvideostream")
  {
    switch (player)
    {
    case Video:
    {
      int index = g_application.GetAppPlayer().GetVideoStream();
      if (index >= 0)
      {
        result = CVariant(CVariant::VariantTypeObject);
        VideoStreamInfo info;
        g_application.GetAppPlayer().GetVideoStreamInfo(index, info);

        result["index"] = index;
        result["name"] = info.name;
        result["language"] = info.language;
        result["codec"] = info.codecName;
        result["width"] = info.width;
        result["height"] = info.height;
      }
      else
        result = CVariant(CVariant::VariantTypeNull);
      break;
    }
    case Audio:
    case Picture:
    default:
      result = CVariant(CVariant::VariantTypeNull);
      break;
    }
  }
  else if (property == "videostreams")
  {
    result = CVariant(CVariant::VariantTypeArray);
    switch (player)
    {
    case Video:
    {
      int streamCount = g_application.GetAppPlayer().GetVideoStreamCount();
      if (streamCount >= 0)
      {
        for (int index = 0; index < streamCount; ++index)
        {
          VideoStreamInfo info;
          g_application.GetAppPlayer().GetVideoStreamInfo(index, info);

          CVariant videoStream(CVariant::VariantTypeObject);
          videoStream["index"] = index;
          videoStream["name"] = info.name;
          videoStream["language"] = info.language;
          videoStream["codec"] = info.codecName;
          videoStream["width"] = info.width;
          videoStream["height"] = info.height;

          result.append(videoStream);
        }
      }
      break;
    }
    case Audio:
    case Picture:
    default:
      break;
    }
  }
  else if (property == "subtitleenabled")
  {
    switch (player)
    {
      case Video:
        result = g_application.GetAppPlayer().GetSubtitleVisible();
        break;

      case Audio:
      case Picture:
      default:
        result = false;
        break;
    }
  }
  else if (property == "currentsubtitle")
  {
    switch (player)
    {
      case Video:
        if (g_application.GetAppPlayer().HasPlayer())
        {
          result = CVariant(CVariant::VariantTypeObject);
          int index = g_application.GetAppPlayer().GetSubtitle();
          if (index >= 0)
          {
            SubtitleStreamInfo info;
            g_application.GetAppPlayer().GetSubtitleStreamInfo(index, info);

            result["index"] = index;
            result["name"] = info.name;
            result["language"] = info.language;
          }
        }
        else
          result = CVariant(CVariant::VariantTypeNull);
        break;

      case Audio:
      case Picture:
      default:
        result = CVariant(CVariant::VariantTypeNull);
        break;
    }
  }
  else if (property == "subtitles")
  {
    result = CVariant(CVariant::VariantTypeArray);
    switch (player)
    {
      case Video:
        if (g_application.GetAppPlayer().HasPlayer())
        {
          for (int index = 0; index < g_application.GetAppPlayer().GetSubtitleCount(); index++)
          {
            SubtitleStreamInfo info;
            g_application.GetAppPlayer().GetSubtitleStreamInfo(index, info);

            CVariant subtitle(CVariant::VariantTypeObject);
            subtitle["index"] = index;
            subtitle["name"] = info.name;
            subtitle["language"] = info.language;

            result.append(subtitle);
          }
        }
        break;

      case Audio:
      case Picture:
      default:
        break;
    }
  }
  else if (property == "live")
    result = IsPVRChannel();
  else
    return InvalidParams;

  return OK;
}

int CPlayerOperations::ParseRepeatState(const CVariant &repeat)
{
  REPEAT_STATE state = REPEAT_NONE;
  std::string strState = repeat.asString();

  if (strState.compare("one") == 0)
    state = REPEAT_ONE;
  else if (strState.compare("all") == 0)
    state = REPEAT_ALL;

  return state;
}

double CPlayerOperations::ParseTimeInSeconds(const CVariant &time)
{
  double seconds = 0.0;
  if (time.isMember("hours"))
    seconds += time["hours"].asInteger() * 60 * 60;
  if (time.isMember("minutes"))
    seconds += time["minutes"].asInteger() * 60;
  if (time.isMember("seconds"))
    seconds += time["seconds"].asInteger();
  if (time.isMember("milliseconds"))
    seconds += time["milliseconds"].asDouble() / 1000.0;

  return seconds;
}

bool CPlayerOperations::IsPVRChannel()
{
  return CServiceBroker::GetPVRManager().IsPlayingTV() || CServiceBroker::GetPVRManager().IsPlayingRadio();
}

CPVREpgInfoTagPtr CPlayerOperations::GetCurrentEpg()
{
  if (!CServiceBroker::GetPVRManager().IsPlayingTV() && !CServiceBroker::GetPVRManager().IsPlayingRadio())
    return CPVREpgInfoTagPtr();

  CPVRChannelPtr currentChannel(CServiceBroker::GetPVRManager().GetPlayingChannel());
  if (!currentChannel)
    return CPVREpgInfoTagPtr();

  return currentChannel->GetEPGNow();
}
