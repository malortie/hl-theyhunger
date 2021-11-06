# Game/Server project configuration

set(TARGET_NAME hldll)

# Compile definitions
set(COMPILE_DEFS_BASE
  WIN32
  _WINDOWS
  QUIVER
  VOXEL
  QUAKE2
  VALVE_DLL
  CLIENT_WEAPONS
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
  /DEF:${CMAKE_CURRENT_SOURCE_DIR}/hl.def
  /SUBSYSTEM:WINDOWS
)

set(LINK_OPTS_RELEASE
  /OPT:ICF # Enable COMDAT Folding
  /OPT:REF # Optimize references
)

# Include directories
set(INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${ENGINE_DIR}
  ${COMMON_DIR}
  ${PM_SHARED_DIR}
  ${GAME_SHARED_DIR}
  ${PUBLIC_DIR}
)

# Header files
set(HEADER_FILES
  activity.h
  activitymap.h
  animation.h
  basemonster.h
  cbase.h
  cdll_dll.h
  client.h
  decals.h
  defaultai.h
  doors.h
  effects.h
  enginecallback.h
  explode.h
  extdll.h
  flyingmonster.h
  func_break.h
  gamerules.h
  hornet.h
  items.h
  monsterevent.h
  monsters.h
  nodes.h
  plane.h
  player.h
  saverestore.h
  schedule.h
  scripted.h
  scriptevent.h
  skill.h
  soundent.h
  spectator.h
  squadmonster.h
  talkmonster.h
  teamplay_gamerules.h
  trains.h
  util.h
  vector.h
  weapons.h
  ${ENGINE_DIR}/eiface.h
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
  aflock.cpp
  agrunt.cpp
  airtank.cpp
  animating.cpp
  animation.cpp
  apache.cpp
  barnacle.cpp
  barney.cpp
  bigmomma.cpp
  bloater.cpp
  bmodels.cpp
  bullsquid.cpp
  buttons.cpp
  cbase.cpp
  client.cpp
  combat.cpp
  controller.cpp
  crossbow.cpp
  crowbar.cpp
  defaultai.cpp
  doors.cpp
  effects.cpp
  egon.cpp
  explode.cpp
  flyingmonster.cpp
  func_break.cpp
  func_tank.cpp
  game.cpp
  gamerules.cpp
  gargantua.cpp
  gauss.cpp
  genericmonster.cpp
  ggrenade.cpp
  globals.cpp
  gman.cpp
  handgrenade.cpp
  hassassin.cpp
  headcrab.cpp
  healthkit.cpp
  hgrunt.cpp
  hornet.cpp
  hornetgun.cpp
  houndeye.cpp
  h_ai.cpp
  h_battery.cpp
  h_cine.cpp
  h_cycler.cpp
  h_export.cpp
  ichthyosaur.cpp
  islave.cpp
  items.cpp
  leech.cpp
  lights.cpp
  maprules.cpp
  monstermaker.cpp
  monsters.cpp
  monsterstate.cpp
  mortar.cpp
  mp5.cpp
  multiplay_gamerules.cpp
  nihilanth.cpp
  nodes.cpp
  observer.cpp
  osprey.cpp
  pathcorner.cpp
  plane.cpp
  plats.cpp
  player.cpp
  python.cpp
  rat.cpp
  roach.cpp
  rpg.cpp
  satchel.cpp
  schedule.cpp
  scientist.cpp
  scripted.cpp
  shotgun.cpp
  singleplay_gamerules.cpp
  skill.cpp
  sound.cpp
  soundent.cpp
  spectator.cpp
  squadmonster.cpp
  squeakgrenade.cpp
  subs.cpp
  talkmonster.cpp
  teamplay_gamerules.cpp
  tempmonster.cpp
  tentacle.cpp
  triggers.cpp
  tripmine.cpp
  turret.cpp
  util.cpp 
  weapons.cpp 
  world.cpp 
  wpn_shared/hl_wpn_glock.cpp
  xen.cpp
  zombie.cpp
  ${GAME_SHARED_DIR}/voice_gamemgr.cpp
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

# Game/Server target definition
add_library(${TARGET_NAME} MODULE 
  ${HEADER_FILES}
  ${SOURCE_FILES}
  ${PM_SHARED_SOURCE_FILES}
)

set_target_properties(${TARGET_NAME}
  PROPERTIES 
    OUTPUT_NAME hl
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

set(LAUNCH_OPT_DEBUG "-game ${MOD_FOLDER} -dev -console +sv_cheats 1")
set(LAUNCH_OPT_RELEASE "-game ${MOD_FOLDER} -console +sv_cheats 1")

# Configuration for launching from Visual Studio
set_target_properties(${TARGET_NAME}
  PROPERTIES 
    VS_DEBUGGER_COMMAND ${GAME_DIR}/hl.exe
    VS_DEBUGGER_COMMAND_ARGUMENTS $<IF:$<CONFIG:Debug>,${LAUNCH_OPT_DEBUG},${LAUNCH_OPT_RELEASE}>
    VS_DEBUGGER_WORKING_DIRECTORY ${GAME_DIR}
)

# Used to copy the DLL file to the dlls folder in mod directory.
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${TARGET_NAME}> ${MOD_DIR}/dlls/$<TARGET_FILE_NAME:${TARGET_NAME}>
  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PDB_FILE:${TARGET_NAME}> ${MOD_DIR}/dlls/$<TARGET_PDB_FILE_NAME:${TARGET_NAME}>
  VERBATIM
)

# Installation
install(TARGETS ${TARGET_NAME} DESTINATION dlls)
