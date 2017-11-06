/*
 *      Copyright (C) 2005-2017 Team XBMC
 *      http://xbmc.org
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "system.h"
#include "GUIUserMessages.h"
#include "Application.h"
#include "ServiceBroker.h"
#include "GUIDialogSubtitleSelect.h"
#include "LangInfo.h"
#include "addons/AddonManager.h"
#include "cores/IPlayer.h"
#include "dialogs/GUIDialogKaiToast.h"
#include "filesystem/AddonsDirectory.h"
#include "filesystem/File.h"
#include "filesystem/SpecialProtocol.h"
#include "filesystem/StackDirectory.h"
#include "guilib/GUIKeyboardFactory.h"
#include "guilib/LocalizeStrings.h"
#include "input/Key.h"
#include "settings/Settings.h"
#include "settings/lib/Setting.h"
#include "utils/JobManager.h"
#include "utils/LangCodeExpander.h"
#include "utils/log.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/Variant.h"
#include "URL.h"
#include "Util.h"
#include "video/VideoDatabase.h"
#include "filesystem/Directory.h"
#include "ServiceBroker.h"
#include "guilib/GUIComponent.h"
#include "dialogs/GUIDialogSelect.h"
#include "guilib/GUIWindowManager.h"

using namespace ADDON;
using namespace XFILE;

#define STRING_SUBTITLES_HEADER 287
#define STRING_TOAST_TEXT_ERROR 24109

CGUIDialogSubtitleSelect::CGUIDialogSubtitleSelect(void)
: CGUIDialog(WINDOW_DIALOG_SUBTITLE_SELECT, "")
{
  m_loadType = KEEP_IN_MEMORY;
}

CGUIDialogSubtitleSelect::~CGUIDialogSubtitleSelect(void)
{
}

bool CGUIDialogSubtitleSelect::OnMessage(CGUIMessage& message)
{
  return CGUIDialog::OnMessage(message);
}

void CGUIDialogSubtitleSelect::OnInitWindow()
{
  ShowSubtitleSelect();
    
  CGUIWindow::OnInitWindow();
}

bool CGUIDialogSubtitleSelect::ShowSubtitleSelect()
{
  //Only display when playing
  if (!g_application.GetAppPlayer().HasPlayer())
  {
    CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Info, g_localizeStrings.Get(STRING_SUBTITLES_HEADER), g_localizeStrings.Get(STRING_TOAST_TEXT_ERROR), TOAST_DISPLAY_TIME, false);
    return false;
  }
  
  CGUIDialogSelect* dialog = CServiceBroker::GetGUI()->GetWindowManager().GetWindow<CGUIDialogSelect>(WINDOW_DIALOG_SELECT);
  if (dialog == NULL)
    return false;
  
  CFileItemList options;
  
  int subtitleStreamCount = g_application.GetAppPlayer().GetSubtitleCount();
  int currentSubtitle = g_application.GetAppPlayer().GetSubtitle();
  if (!g_application.GetAppPlayer().GetSubtitleVisible())
    currentSubtitle = -1;
  
  //Add the Disabled Element
  {
    CFileItemPtr item(new CFileItem(g_localizeStrings.Get(1223).c_str()));
    item->SetProperty("value", -1);
    if (currentSubtitle == -1)
      item->Select(true);
    options.Add(item);
  }
  
  // cycle through each subtitle and add it to our entry list
  for (int i = 0; i < subtitleStreamCount; ++i)
  {
    SubtitleStreamInfo info;
    g_application.GetAppPlayer().GetSubtitleStreamInfo(i, info);
    
    std::string strItem;
    std::string strLanguage;
    
    if (!g_LangCodeExpander.Lookup(info.language, strLanguage))
      strLanguage = g_localizeStrings.Get(13205); // Unknown
    
    if (info.name.length() == 0)
      strItem = strLanguage;
    else
      strItem = StringUtils::Format("%s - %s", strLanguage.c_str(), info.name.c_str());
    
    CFileItemPtr item(new CFileItem(strItem.c_str()));
    item->SetProperty("value", i);
    
    if (i == currentSubtitle)
      item->Select(true);
    
    options.Add(item);
  }
  
  if (options.Size() < 2)
  {
    CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Info, g_localizeStrings.Get(STRING_SUBTITLES_HEADER), g_localizeStrings.Get(STRING_TOAST_TEXT_ERROR), TOAST_DISPLAY_TIME, false);
    return true;
  }
  
  dialog->Reset();
  dialog->SetHeading(g_localizeStrings.Get(STRING_SUBTITLES_HEADER).c_str());
  dialog->SetItems(options);
  dialog->SetMultiSelection(false);
  dialog->Open();
  
  if (!dialog->IsConfirmed())
    return true;
  
  int selected = dialog->GetSelectedItem() - 1;
  if (selected == currentSubtitle)
  {
    if (!g_application.GetAppPlayer().GetSubtitleVisible())
      g_application.GetAppPlayer().SetSubtitleVisible(true);
  }
  else if (selected == -1)
  {
    g_application.GetAppPlayer().SetSubtitleVisible(false);
  }
  else
  {
    g_application.GetAppPlayer().SetSubtitle(selected);
    g_application.GetAppPlayer().SetSubtitleVisible(true);
  }
  
  return true;
}
