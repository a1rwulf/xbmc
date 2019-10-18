/*
 *  Copyright (C) 2005-2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#ifndef BUILD_KODI_ADDON
#include "utils/SortUtils.h"
#endif

namespace kodi
{
namespace addon
{

typedef enum
{
  SortOrder_None = 0,
  SortOrder_Ascending,
  SortOrder_Descending
} AddonSortOrder;

#ifndef BUILD_KODI_ADDON
inline SortOrder TransToKodiSortOrder(kodi::addon::AddonSortOrder order)
{
  switch (order)
  {
    case kodi::addon::SortOrder_Ascending:
      return SortOrder::SortOrderAscending;
    case kodi::addon::SortOrder_Descending:
      return SortOrder::SortOrderDescending;
    case kodi::addon::SortOrder_None:
    default:
      return SortOrder::SortOrderNone;
  }
}

inline kodi::addon::AddonSortOrder TransToAddonSortOrder(SortOrder order)
{
  switch (order)
  {
    case SortOrder::SortOrderAscending:
      return kodi::addon::SortOrder_Ascending;
    case SortOrder::SortOrderDescending:
      return kodi::addon::SortOrder_Descending;
    case SortOrder::SortOrderNone:
    default:
      return kodi::addon::SortOrder_None;
  }
}
#endif

typedef enum
{
  SortAttribute_None = 0x0,
  SortAttribute_IgnoreArticle = 0x1,
  SortAttribute_IgnoreFolders = 0x2,
  SortAttribute_UseArtistSortName = 0x4,
  SortAttribute_IgnoreLabel = 0x8
} AddonSortAttribute;

#ifndef BUILD_KODI_ADDON
inline SortAttribute TransToKodiSortAttribute(kodi::addon::AddonSortAttribute attribute)
{
  int ret = SortAttribute::SortAttributeNone;
  if (attribute & kodi::addon::SortAttribute_IgnoreArticle)
    ret |= SortAttribute::SortAttributeIgnoreArticle;
  if (attribute & kodi::addon::SortAttribute_IgnoreFolders)
    ret |= SortAttribute::SortAttributeIgnoreFolders;
  if (attribute & kodi::addon::SortAttribute_UseArtistSortName)
    ret |= SortAttribute::SortAttributeUseArtistSortName;
  if (attribute & kodi::addon::SortAttribute_IgnoreLabel)
    ret |= SortAttribute::SortAttributeIgnoreLabel;
  return static_cast<SortAttribute>(ret);
}

inline kodi::addon::AddonSortAttribute TransToAddonSortAttribute(SortAttribute attribute)
{
  int ret = SortAttribute::SortAttributeNone;
  if (attribute & SortAttribute::SortAttributeIgnoreArticle)
    ret |= kodi::addon::SortAttribute_IgnoreArticle;
  if (attribute & SortAttribute::SortAttributeIgnoreFolders)
    ret |= kodi::addon::SortAttribute_IgnoreFolders;
  if (attribute & SortAttribute::SortAttributeUseArtistSortName)
    ret |= kodi::addon::SortAttribute_UseArtistSortName;
  if (attribute & SortAttribute::SortAttributeIgnoreLabel)
    ret |= kodi::addon::SortAttribute_IgnoreLabel;
  return static_cast<kodi::addon::AddonSortAttribute>(ret);
}
#endif

typedef enum
{
  SortSpecialNone = 0,
  SortSpecialOnTop = 1,
  SortSpecialOnBottom = 2
} SortSpecial;

///
/// \defgroup List_of_sort_methods List of sort methods
/// \addtogroup List_of_sort_methods
///
/// \brief These ID's can be used with the \ref built_in_functions_6 "Container.SetSortMethod(id)" function
/// \note The on field named part with <b>String</b> shows the string used on
/// GUI to set this sort type.
///
///@{
typedef enum
{
  /// __0__  :
  SortBy_None = 0,
  /// __1__  : Sort by Name                       <em>(String: <b><c>Label</c></b>)</em>
  SortBy_Label,
  /// __2__  : Sort by Date                       <em>(String: <b><c>Date</c></b>)</em>
  SortBy_Date,
  /// __3__  : Sort by Size                       <em>(String: <b><c>Size</c></b>)</em>
  SortBy_Size,
  /// __4__  : Sort by filename                   <em>(String: <b><c>File</c></b>)</em>
  SortBy_File,
  /// __5__  : Sort by path                       <em>(String: <b><c>Path</c></b>)</em>
  SortBy_Path,
  /// __6__  : Sort by drive type                 <em>(String: <b><c>DriveType</c></b>)</em>
  SortBy_DriveType,
  /// __7__  : Sort by title                      <em>(String: <b><c>Title</c></b>)</em>
  SortBy_Title,
  /// __8__  : Sort by track number               <em>(String: <b><c>TrackNumber</c></b>)</em>
  SortBy_TrackNumber,
  /// __9__  : Sort by time                       <em>(String: <b><c>Time</c></b>)</em>
  SortBy_Time,
  /// __10__ : Sort by artist                     <em>(String: <b><c>Artist</c></b>)</em>
  SortBy_Artist,
  /// __11__ : Sort by first artist then year     <em>(String: <b><c>ArtistYear</c></b>)</em>
  SortBy_ArtistThenYear,
  /// __12__ : Sort by album                      <em>(String: <b><c>Album</c></b>)</em>
  SortBy_Album,
  /// __13__ : Sort by album type                 <em>(String: <b><c>AlbumType</c></b>)</em>
  SortBy_AlbumType,
  /// __14__ : Sort by genre                      <em>(String: <b><c>Genre</c></b>)</em>
  SortBy_Genre,
  /// __15__ : Sort by country                     <em>(String: <b><c>Country</c></b>)</em>
  SortBy_Country,
  /// __16__ : Sort by year                       <em>(String: <b><c>Year</c></b>)</em>
  SortBy_Year,
  /// __17__ : Sort by rating                     <em>(String: <b><c>Rating</c></b>)</em>
  SortBy_Rating,
  /// __18__ : Sort by user rating                <em>(String: <b><c>UserRating</c></b>)</em>
  SortBy_UserRating,
  /// __19__ : Sort by votes                      <em>(String: <b><c>Votes</c></b>)</em>
  SortBy_Votes,
  /// __20__ : Sort by top 250                    <em>(String: <b><c>Top250</c></b>)</em>
  SortBy_Top250,
  /// __21__ : Sort by program count              <em>(String: <b><c>ProgramCount</c></b>)</em>
  SortBy_ProgramCount,
  /// __22__ : Sort by playlist order             <em>(String: <b><c>Playlist</c></b>)</em>
  SortBy_PlaylistOrder,
  /// __23__ : Sort by episode number             <em>(String: <b><c>Episode</c></b>)</em>
  SortBy_EpisodeNumber,
  /// __24__ : Sort by season                     <em>(String: <b><c>Season</c></b>)</em>
  SortBy_Season,
  /// __25__ : Sort by number of episodes         <em>(String: <b><c>TotalEpisodes</c></b>)</em>
  SortBy_NumberOfEpisodes,
  /// __26__ : Sort by number of watched episodes <em>(String: <b><c>WatchedEpisodes</c></b>)</em>
  SortBy_NumberOfWatchedEpisodes,
  /// __27__ : Sort by TV show status             <em>(String: <b><c>TvShowStatus</c></b>)</em>
  SortBy_TvShowStatus,
  /// __28__ : Sort by TV show title              <em>(String: <b><c>TvShowTitle</c></b>)</em>
  SortBy_TvShowTitle,
  /// __29__ : Sort by sort title                 <em>(String: <b><c>SortTitle</c></b>)</em>
  SortBy_SortTitle,
  /// __30__ : Sort by production code            <em>(String: <b><c>ProductionCode</c></b>)</em>
  SortBy_ProductionCode,
  /// __31__ : Sort by MPAA                       <em>(String: <b><c>MPAA</c></b>)</em>
  SortBy_MPAA,
  /// __32__ : Sort by video resolution           <em>(String: <b><c>VideoResolution</c></b>)</em>
  SortBy_VideoResolution,
  /// __33__ : Sort by video codec                <em>(String: <b><c>VideoCodec</c></b>)</em>
  SortBy_VideoCodec,
  /// __34__ : Sort by video aspect ratio         <em>(String: <b><c>VideoAspectRatio</c></b>)</em>
  SortBy_VideoAspectRatio,
  /// __35__ : Sort by audio channels             <em>(String: <b><c>AudioChannels</c></b>)</em>
  SortBy_AudioChannels,
  /// __36__ : Sort by audio codec                <em>(String: <b><c>AudioCodec</c></b>)</em>
  SortBy_AudioCodec,
  /// __37__ : Sort by audio language             <em>(String: <b><c>AudioLanguage</c></b>)</em>
  SortBy_AudioLanguage,
  /// __38__ : Sort by subtitle language          <em>(String: <b><c>SubtitleLanguage</c></b>)</em>
  SortBy_SubtitleLanguage,
  /// __39__ : Sort by studio                     <em>(String: <b><c>Studio</c></b>)</em>
  SortBy_Studio,
  /// __40__ : Sort by date added                 <em>(String: <b><c>DateAdded</c></b>)</em>
  SortBy_DateAdded,
  /// __41__ : Sort by last played                <em>(String: <b><c>LastPlayed</c></b>)</em>
  SortBy_LastPlayed,
  /// __42__ : Sort by playcount                  <em>(String: <b><c>PlayCount</c></b>)</em>
  SortBy_Playcount,
  /// __43__ : Sort by listener                   <em>(String: <b><c>Listeners</c></b>)</em>
  SortBy_Listeners,
  /// __44__ : Sort by bitrate                    <em>(String: <b><c>Bitrate</c></b>)</em>
  SortBy_Bitrate,
  /// __45__ : Sort by random                     <em>(String: <b><c>Random</c></b>)</em>
  SortBy_Random,
  /// __46__ : Sort by channel                    <em>(String: <b><c>Channel</c></b>)</em>
  SortBy_Channel,
  /// __47__ : Sort by channel number             <em>(String: <b><c>ChannelNumber</c></b>)</em>
  SortBy_ChannelNumber,
  /// __48__ : Sort by date taken                 <em>(String: <b><c>DateTaken</c></b>)</em>
  SortBy_DateTaken,
  /// __49__ : Sort by relevance
  SortBy_Relevance,
  /// __50__ : Sort by installation date          <en>(String: <b><c>installdate</c></b>)</em>
  SortBy_InstallDate,
  /// __51__ : Sort by last updated               <en>(String: <b><c>lastupdated</c></b>)</em>
  SortBy_LastUpdated,
  /// __52__ : Sort by last used                  <em>(String: <b><c>lastused</c></b>)</em>
  SortBy_LastUsed,
} AddonSortBy;
///@}

#ifndef BUILD_KODI_ADDON
inline SortBy TransToKodiSortBy(kodi::addon::AddonSortBy sortBy)
{
  switch (sortBy)
  {
    case kodi::addon::SortBy_Label:
      return SortBy::SortByLabel;
    case kodi::addon::SortBy_Date:
      return SortBy::SortByDate;
    case kodi::addon::SortBy_Size:
      return SortBy::SortBySize;
    case kodi::addon::SortBy_File:
      return SortBy::SortByFile;
    case kodi::addon::SortBy_Path:
      return SortBy::SortByPath;
    case kodi::addon::SortBy_DriveType:
      return SortBy::SortByDriveType;
    case kodi::addon::SortBy_Title:
      return SortBy::SortByTitle;
    case kodi::addon::SortBy_TrackNumber:
      return SortBy::SortByTrackNumber;
    case kodi::addon::SortBy_Time:
      return SortBy::SortByTime;
    case kodi::addon::SortBy_Artist:
      return SortBy::SortByArtist;
    case kodi::addon::SortBy_ArtistThenYear:
      return SortBy::SortByArtistThenYear;
    case kodi::addon::SortBy_Album:
      return SortBy::SortByAlbum;
    case kodi::addon::SortBy_AlbumType:
      return SortBy::SortByAlbumType;
    case kodi::addon::SortBy_Genre:
      return SortBy::SortByGenre;
    case kodi::addon::SortBy_Country:
      return SortBy::SortByTitle;
    case kodi::addon::SortBy_Year:
      return SortBy::SortByYear;
    case kodi::addon::SortBy_Rating:
      return SortBy::SortByRating;
    case kodi::addon::SortBy_UserRating:
      return SortBy::SortByUserRating;
    case kodi::addon::SortBy_Votes:
      return SortBy::SortByVotes;
    case kodi::addon::SortBy_Top250:
      return SortBy::SortByTop250;
    case kodi::addon::SortBy_ProgramCount:
      return SortBy::SortByProgramCount;
    case kodi::addon::SortBy_PlaylistOrder:
      return SortBy::SortByPlaylistOrder;
    case kodi::addon::SortBy_EpisodeNumber:
      return SortBy::SortByEpisodeNumber;
    case kodi::addon::SortBy_Season:
      return SortBy::SortBySeason;
    case kodi::addon::SortBy_NumberOfEpisodes:
      return SortBy::SortByNumberOfEpisodes;
    case kodi::addon::SortBy_NumberOfWatchedEpisodes:
      return SortBy::SortByNumberOfWatchedEpisodes;
    case kodi::addon::SortBy_TvShowStatus:
      return SortBy::SortByTvShowStatus;
    case kodi::addon::SortBy_TvShowTitle:
      return SortBy::SortByTvShowTitle;
    case kodi::addon::SortBy_SortTitle:
      return SortBy::SortBySortTitle;
    case kodi::addon::SortBy_ProductionCode:
      return SortBy::SortByProductionCode;
    case kodi::addon::SortBy_MPAA:
      return SortBy::SortByMPAA;
    case kodi::addon::SortBy_VideoResolution:
      return SortBy::SortByVideoResolution;
    case kodi::addon::SortBy_VideoCodec:
      return SortBy::SortByVideoCodec;
    case kodi::addon::SortBy_VideoAspectRatio:
      return SortBy::SortByVideoAspectRatio;
    case kodi::addon::SortBy_AudioChannels:
      return SortBy::SortByAudioChannels;
    case kodi::addon::SortBy_AudioCodec:
      return SortBy::SortByAudioCodec;
    case kodi::addon::SortBy_AudioLanguage:
      return SortBy::SortByAudioLanguage;
    case kodi::addon::SortBy_SubtitleLanguage:
      return SortBy::SortBySubtitleLanguage;
    case kodi::addon::SortBy_Studio:
      return SortBy::SortByStudio;
    case kodi::addon::SortBy_DateAdded:
      return SortBy::SortByDateAdded;
    case kodi::addon::SortBy_LastPlayed:
      return SortBy::SortByLastPlayed;
    case kodi::addon::SortBy_Playcount:
      return SortBy::SortByPlaycount;
    case kodi::addon::SortBy_Listeners:
      return SortBy::SortByListeners;
    case kodi::addon::SortBy_Bitrate:
      return SortBy::SortByBitrate;
    case kodi::addon::SortBy_Random:
      return SortBy::SortByRandom;
    case kodi::addon::SortBy_Channel:
      return SortBy::SortByChannel;
    case kodi::addon::SortBy_ChannelNumber:
      return SortBy::SortByChannelNumber;
    case kodi::addon::SortBy_DateTaken:
      return SortBy::SortByDateTaken;
    case kodi::addon::SortBy_Relevance:
      return SortBy::SortByRelevance;
    case kodi::addon::SortBy_InstallDate:
      return SortBy::SortByInstallDate;
    case kodi::addon::SortBy_LastUpdated:
      return SortBy::SortByLastUpdated;
    case kodi::addon::SortBy_LastUsed:
      return SortBy::SortByLastUsed;
    case kodi::addon::SortBy_None:
    default:
      return SortBy::SortByNone;
  }
}

inline kodi::addon::AddonSortBy TransToAddonSortBy(SortBy sortBy)
{
  switch (sortBy)
  {
    case SortBy::SortByLabel:
      return kodi::addon::SortBy_Label;
    case SortBy::SortByDate:
      return kodi::addon::SortBy_Date;
    case SortBy::SortBySize:
      return kodi::addon::SortBy_Size;
    case SortBy::SortByFile:
      return kodi::addon::SortBy_File;
    case SortBy::SortByPath:
      return kodi::addon::SortBy_Path;
    case SortBy::SortByDriveType:
      return kodi::addon::SortBy_DriveType;
    case SortBy::SortByTitle:
      return kodi::addon::SortBy_Title;
    case SortBy::SortByTrackNumber:
      return kodi::addon::SortBy_TrackNumber;
    case SortBy::SortByTime:
      return kodi::addon::SortBy_Time;
    case SortBy::SortByArtist:
      return kodi::addon::SortBy_Artist;
    case SortBy::SortByArtistThenYear:
      return kodi::addon::SortBy_ArtistThenYear;
    case SortBy::SortByAlbum:
      return kodi::addon::SortBy_Album;
    case SortBy::SortByAlbumType:
      return kodi::addon::SortBy_AlbumType;
    case SortBy::SortByGenre:
      return kodi::addon::SortBy_Genre;
    case SortBy::SortByCountry:
      return kodi::addon::SortBy_Title;
    case SortBy::SortByYear:
      return kodi::addon::SortBy_Year;
    case SortBy::SortByRating:
      return kodi::addon::SortBy_Rating;
    case SortBy::SortByUserRating:
      return kodi::addon::SortBy_UserRating;
    case SortBy::SortByVotes:
      return kodi::addon::SortBy_Votes;
    case SortBy::SortByTop250:
      return kodi::addon::SortBy_Top250;
    case SortBy::SortByProgramCount:
      return kodi::addon::SortBy_ProgramCount;
    case SortBy::SortByPlaylistOrder:
      return kodi::addon::SortBy_PlaylistOrder;
    case SortBy::SortByEpisodeNumber:
      return kodi::addon::SortBy_EpisodeNumber;
    case SortBy::SortBySeason:
      return kodi::addon::SortBy_Season;
    case SortBy::SortByNumberOfEpisodes:
      return kodi::addon::SortBy_NumberOfEpisodes;
    case SortBy::SortByNumberOfWatchedEpisodes:
      return kodi::addon::SortBy_NumberOfWatchedEpisodes;
    case SortBy::SortByTvShowStatus:
      return kodi::addon::SortBy_TvShowStatus;
    case SortBy::SortByTvShowTitle:
      return kodi::addon::SortBy_TvShowTitle;
    case SortBy::SortBySortTitle:
      return kodi::addon::SortBy_SortTitle;
    case SortBy::SortByProductionCode:
      return kodi::addon::SortBy_ProductionCode;
    case SortBy::SortByMPAA:
      return kodi::addon::SortBy_MPAA;
    case SortBy::SortByVideoResolution:
      return kodi::addon::SortBy_VideoResolution;
    case SortBy::SortByVideoCodec:
      return kodi::addon::SortBy_VideoCodec;
    case SortBy::SortByVideoAspectRatio:
      return kodi::addon::SortBy_VideoAspectRatio;
    case SortBy::SortByAudioChannels:
      return kodi::addon::SortBy_AudioChannels;
    case SortBy::SortByAudioCodec:
      return kodi::addon::SortBy_AudioCodec;
    case SortBy::SortByAudioLanguage:
      return kodi::addon::SortBy_AudioLanguage;
    case SortBy::SortBySubtitleLanguage:
      return kodi::addon::SortBy_SubtitleLanguage;
    case SortBy::SortByStudio:
      return kodi::addon::SortBy_Studio;
    case SortBy::SortByDateAdded:
      return kodi::addon::SortBy_DateAdded;
    case SortBy::SortByLastPlayed:
      return kodi::addon::SortBy_LastPlayed;
    case SortBy::SortByPlaycount:
      return kodi::addon::SortBy_Playcount;
    case SortBy::SortByListeners:
      return kodi::addon::SortBy_Listeners;
    case SortBy::SortByBitrate:
      return kodi::addon::SortBy_Bitrate;
    case SortBy::SortByRandom:
      return kodi::addon::SortBy_Random;
    case SortBy::SortByChannel:
      return kodi::addon::SortBy_Channel;
    case SortBy::SortByChannelNumber:
      return kodi::addon::SortBy_ChannelNumber;
    case SortBy::SortByDateTaken:
      return kodi::addon::SortBy_DateTaken;
    case SortBy::SortByRelevance:
      return kodi::addon::SortBy_Relevance;
    case SortBy::SortByInstallDate:
      return kodi::addon::SortBy_InstallDate;
    case SortBy::SortByLastUpdated:
      return kodi::addon::SortBy_LastUpdated;
    case SortBy::SortByLastUsed:
      return kodi::addon::SortBy_LastUsed;
    case SortBy::SortByNone:
    default:
      return kodi::addon::SortBy_None;
  }
}
#endif

typedef struct AddonSortDescription
{
  AddonSortBy sortBy = SortBy_None;
  AddonSortOrder sortOrder = SortOrder_Ascending;
  AddonSortAttribute sortAttributes = SortAttribute_None;
  int limitStart = 0;
  int limitEnd = -1;
} AddonSortDescription;

} // namespace addon
} // namespace kodi
