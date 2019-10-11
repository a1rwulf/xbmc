# List contains only add-on related headers not present in
# ./addons/kodi-addon-dev-kit/include/kodi
#

# Keep this in alphabetical order
set(CORE_ADDON_BINDINGS_FILES
    ${CORE_SOURCE_DIR}/xbmc/cores/AudioEngine/Utils/AEChannelData.h
    ${CORE_SOURCE_DIR}/xbmc/filesystem/IFileTypes.h
    ${CORE_SOURCE_DIR}/xbmc/input/actions/ActionIDs.h
    ${CORE_SOURCE_DIR}/xbmc/input/XBMC_vkeys.h
    ${CORE_SOURCE_DIR}/xbmc/FileItem.h
    ${CORE_SOURCE_DIR}/xbmc/addons/IAddon.h
    ${CORE_SOURCE_DIR}/xbmc/guilib/GUIListItem.h
    ${CORE_SOURCE_DIR}/xbmc/LockType.h
    ${CORE_SOURCE_DIR}/xbmc/pvr/PVRTypes.h
    ${CORE_SOURCE_DIR}/xbmc/threads/CriticalSection.h
    ${CORE_SOURCE_DIR}/xbmc/threads/Lockables.h
    ${CORE_SOURCE_DIR}/xbmc/utils/IArchivable.h
    ${CORE_SOURCE_DIR}/xbmc/utils/ISerializable.h
    ${CORE_SOURCE_DIR}/xbmc/utils/ISortable.h
    ${CORE_SOURCE_DIR}/xbmc/utils/SortUtils.h
    ${CORE_SOURCE_DIR}/xbmc/XBDateTime.h
    ${CORE_SOURCE_DIR}/xbmc/addons/AddonInfo.h
    ${CORE_SOURCE_DIR}/xbmc/addons/AddonVersion.h
    ${CORE_SOURCE_DIR}/xbmc/platform/linux/PlatformDefs.h
    ${CORE_SOURCE_DIR}/xbmc/threads/platform/RecursiveMutex.h
    ${CORE_SOURCE_DIR}/xbmc/utils/DatabaseUtils.h
    ${CORE_SOURCE_DIR}/xbmc/media/MediaType.h
    ${CORE_SOURCE_DIR}/xbmc/SortFileItem.h
    ${CORE_SOURCE_DIR}/xbmc/utils/LabelFormatter.h
    ${CORE_SOURCE_DIR}/xbmc/dbwrappers/Database.h
)

set(CORE_ADDON_BINDINGS_DIRS
    ${CORE_SOURCE_DIR}/xbmc/addons/kodi-addon-dev-kit/include/kodi/
    ${CORE_SOURCE_DIR}/xbmc/cores/VideoPlayer/Interface/Addon
)
