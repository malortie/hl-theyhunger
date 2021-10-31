# Client project configuration

set(TARGET_NAME hl_cdll)

# Compile definitions
set(COMPILE_DEFS_BASE
  WIN32
  _WINDOWS
  CLIENT_DLL
  CLIENT_WEAPONS
  HL_DLL
  _CRT_SECURE_NO_WARNINGS
  _CRT_NONSTDC_NO_WARNINGS
)

set(COMPILE_DEFS_DEBUG
  _DEBUG
)

set(COMPILE_DEFS_RELEASE
  NDEBUG
)

# Compile options
set(COMPILE_OPTS_DEBUG
  /MTd # Runtime library
)

set(COMPILE_OPTS_RELEASE
  /GL # Whole program optimization
  /Gy # Function level linking
  /MT # Runtime library
  /Oi # Generate intrinsic functions
)

# Linker options
set(LINK_OPTS_BASE
  /DEBUG # Generate Debug info
  /SUBSYSTEM:WINDOWS
)

set(LINK_OPTS_RELEASE
  /OPT:ICF # Enable COMDAT Folding
  /OPT:REF # Optimize references
)

# Include directories
set(INCLUDE_DIRS 
  ${DLLS_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${PUBLIC_DIR}
  ${COMMON_DIR}
  ${PM_SHARED_DIR}
  ${ENGINE_DIR}
  ${UTILS_DIR}/vgui/include
  ${GAME_SHARED_DIR}
  ${EXTERNAL_DIR}
)

# Header files
set(HEADER_FILES 
  ammo.h
  ammohistory.h
  camera.h
  cl_dll.h
  cl_util.h
  com_weapons.h
  demo.h
  eventscripts.h
  ev_hldm.h
  GameStudioModelRenderer.h
  health.h
  hud.h
  hud_servers.h
  hud_servers_priv.h
  hud_spectator.h
  interpolation.h
  in_defs.h
  kbutton.h
  StudioModelRenderer.h
  tri.h
  util_vector.h
  vgui_ControlConfigPanel.h
  vgui_int.h
  vgui_SchemeManager.h
  vgui_ScorePanel.h
  vgui_ServerBrowser.h
  vgui_SpectatorPanel.h
  view.h
  wrect.h
  ${COMMON_DIR}/parsemsg.h
  ${GAME_SHARED_DIR}/vgui_scrollbar2.h
  ${GAME_SHARED_DIR}/vgui_slider2.h
  ${GAME_SHARED_DIR}/voice_banmgr.h
  ${GAME_SHARED_DIR}/voice_status.h
  ${PM_SHARED_DIR}/pm_debug.h
  ${PM_SHARED_DIR}/pm_defs.h
  ${PM_SHARED_DIR}/pm_info.h
  ${PM_SHARED_DIR}/pm_materials.h
  ${PM_SHARED_DIR}/pm_movevars.h
  ${PM_SHARED_DIR}/pm_shared.h
  ${MOD_CONFIG_FILE_PATH}
)

# Source files
set(SOURCE_FILES 
  ammo.cpp
  ammohistory.cpp
  ammo_secondary.cpp
  battery.cpp
  cdll_int.cpp
  com_weapons.cpp
  death.cpp
  demo.cpp
  entity.cpp
  events.cpp
  ev_common.cpp
  ev_hldm.cpp
  flashlight.cpp
  GameStudioModelRenderer.cpp
  geiger.cpp
  health.cpp
  hl/hl_baseentity.cpp
  hl/hl_events.cpp
  hl/hl_objects.cpp
  hl/hl_weapons.cpp
  hud.cpp
  hud_bench.cpp
  hud_benchtrace.cpp
  hud_msg.cpp
  hud_redraw.cpp
  hud_servers.cpp
  hud_spectator.cpp
  hud_update.cpp
  input.cpp
  inputw32.cpp
  interpolation.cpp
  in_camera.cpp
  menu.cpp
  message.cpp
  saytext.cpp
  statusbar.cpp
  status_icons.cpp
  StudioModelRenderer.cpp
  studio_util.cpp
  text_message.cpp
  train.cpp
  tri.cpp
  util.cpp
  vgui_ClassMenu.cpp
  vgui_ControlConfigPanel.cpp
  vgui_CustomObjects.cpp
  vgui_int.cpp
  vgui_MOTDWindow.cpp
  vgui_SchemeManager.cpp
  vgui_ScorePanel.cpp
  vgui_ServerBrowser.cpp
  vgui_SpectatorPanel.cpp
  vgui_TeamFortressViewport.cpp
  vgui_teammenu.cpp
  view.cpp
  voice_status.cpp
  ${COMMON_DIR}/parsemsg.cpp
  ${DLLS_DIR}/crossbow.cpp
  ${DLLS_DIR}/crowbar.cpp
  ${DLLS_DIR}/egon.cpp
  ${DLLS_DIR}/gauss.cpp
  ${DLLS_DIR}/handgrenade.cpp
  ${DLLS_DIR}/hornetgun.cpp
  ${DLLS_DIR}/mp5.cpp
  ${DLLS_DIR}/python.cpp
  ${DLLS_DIR}/rpg.cpp
  ${DLLS_DIR}/satchel.cpp
  ${DLLS_DIR}/shotgun.cpp
  ${DLLS_DIR}/squeakgrenade.cpp
  ${DLLS_DIR}/tripmine.cpp
  ${DLLS_DIR}/wpn_shared/hl_wpn_glock.cpp
  ${GAME_SHARED_DIR}/vgui_checkbutton2.cpp
  ${GAME_SHARED_DIR}/vgui_grid.cpp
  ${GAME_SHARED_DIR}/vgui_helpers.cpp
  ${GAME_SHARED_DIR}/vgui_listbox.cpp
  ${GAME_SHARED_DIR}/vgui_loadtga.cpp
  ${GAME_SHARED_DIR}/vgui_scrollbar2.cpp
  ${GAME_SHARED_DIR}/vgui_slider2.cpp
  ${GAME_SHARED_DIR}/voice_banmgr.cpp
  ${PUBLIC_DIR}/interface.cpp
)

set(PM_SHARED_SOURCE_FILES
  ${PM_SHARED_DIR}/pm_debug.c
  ${PM_SHARED_DIR}/pm_math.c
  ${PM_SHARED_DIR}/pm_shared.c
)

set_source_files_properties(
  ${PM_SHARED_SOURCE_FILES}
  PROPERTIES
    LANGUAGE C
)

# Visual Studio filters
source_group(TREE ${PROJECT_SOURCE_DIR} 
  PREFIX "Header Files" 
  FILES ${HEADER_FILES}
)

source_group(TREE ${PROJECT_SOURCE_DIR} 
  PREFIX "Source Files" 
  FILES ${SOURCE_FILES} ${PM_SHARED_SOURCE_FILES}
)

# Client target definition
add_library(${TARGET_NAME} MODULE
  ${HEADER_FILES}
  ${SOURCE_FILES}
  ${PM_SHARED_SOURCE_FILES}
)

set_target_properties(${TARGET_NAME}
  PROPERTIES 
    OUTPUT_NAME client
)

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRS})

target_compile_definitions(${TARGET_NAME} PRIVATE
  ${COMPILE_DEFS_BASE}
  $<IF:$<CONFIG:Debug>,${COMPILE_DEFS_DEBUG},${COMPILE_DEFS_RELEASE}>
)

target_compile_options(${TARGET_NAME} PRIVATE
  $<IF:$<CONFIG:Debug>,${COMPILE_OPTS_DEBUG},${COMPILE_OPTS_RELEASE}>
)

target_link_options(${TARGET_NAME} PRIVATE
  ${LINK_OPTS_BASE}
  $<$<NOT:$<CONFIG:Debug>>:${LINK_OPTS_RELEASE}>
)

target_link_libraries(${TARGET_NAME}
  wsock32.lib
  ${UTILS_DIR}/vgui/lib/win32_vc6/vgui.lib
  ${LIB_DIR}/public/game_controls.lib
  ${LIB_DIR}/public/sdl2.lib
)

set(LAUNCH_OPT_DEBUG "-game ${MOD_FOLDER} -dev -console +sv_cheats 1")
set(LAUNCH_OPT_RELEASE "-game ${MOD_FOLDER} -console +sv_cheats 1")

# Configuration for launching from Visual Studio
set_target_properties(${TARGET_NAME}
  PROPERTIES 
    VS_DEBUGGER_COMMAND ${GAME_DIR}/hl.exe
    VS_DEBUGGER_COMMAND_ARGUMENTS $<IF:$<CONFIG:Debug>,${LAUNCH_OPT_DEBUG},${LAUNCH_OPT_RELEASE}>
    VS_DEBUGGER_WORKING_DIRECTORY ${GAME_DIR}
)

# Used to copy the DLL file to the cl_dlls folder in mod directory.
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${TARGET_NAME}> ${MOD_DIR}/cl_dlls/$<TARGET_FILE_NAME:${TARGET_NAME}>
  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PDB_FILE:${TARGET_NAME}> ${MOD_DIR}/cl_dlls/$<TARGET_PDB_FILE_NAME:${TARGET_NAME}>
  VERBATIM
)

# Installation
install(TARGETS ${TARGET_NAME} DESTINATION cl_dlls)
