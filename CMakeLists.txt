# Solution configuration

cmake_minimum_required(VERSION 3.21)
project(projects VERSION 1.0.0 LANGUAGES CXX)

set(MOD_ARCHIVE_DIR "" CACHE PATH "Absolute path where to save the mod archive.")
set(MOD_ARCHIVE_FOLDER "" CACHE STRING "Folder name of the mod archive.")

option(ENABLE_MOD_ARCHIVE "Enable mod archive target." ON)

if (NOT IS_DIRECTORY ${CMAKE_INSTALL_PREFIX})
  message(FATAL_ERROR "CMAKE_INSTALL_PREFIX either does not exist or is not a directory.")
elseif(NOT IS_ABSOLUTE ${CMAKE_INSTALL_PREFIX})
  message(FATAL_ERROR "CMAKE_INSTALL_PREFIX is not an absolute path.")
else()
  # Get the mod folder.
  cmake_path(GET CMAKE_INSTALL_PREFIX FILENAME MOD_FOLDER)
  # Get the game directory.
  cmake_path(GET CMAKE_INSTALL_PREFIX PARENT_PATH GAME_DIR)
endif()

# Mod directory is the install directory.
set(MOD_DIR ${CMAKE_INSTALL_PREFIX})

if (ENABLE_MOD_ARCHIVE)
  if (NOT IS_DIRECTORY ${MOD_ARCHIVE_DIR})
    message(FATAL_ERROR "MOD_ARCHIVE_DIR either does not exist or is not a directory.")
  elseif(NOT IS_ABSOLUTE ${MOD_ARCHIVE_DIR})
    message(FATAL_ERROR "MOD_ARCHIVE_DIR is not an absolute path.")
  endif()

  if (MOD_ARCHIVE_FOLDER STREQUAL "")
    message(FATAL_ERROR "MOD_ARCHIVE_FOLDER has no value.")
  endif()
endif()

# SDK directories definitions
set(CL_DLL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cl_dll)
set(COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/common)
set(DLLS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dlls)
set(ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/engine)
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(GAME_SHARED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/game_shared)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(PM_SHARED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pm_shared)
set(PUBLIC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/public)
set(UTILS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/utils)
set(MOD_FILES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mod_files)

# Mod configuration file
set(MOD_CONFIG_FILE_PATH ${COMMON_DIR}/mod_config.h)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/mod_config.h.in
  ${MOD_CONFIG_FILE_PATH}
  @ONLY
)

add_subdirectory(cl_dll)
add_subdirectory(dlls)

if (ENABLE_MOD_ARCHIVE)
  # Archive target definition
  set(MOD_ARCHIVE_TEMP_DIR ${MOD_ARCHIVE_DIR}/${MOD_ARCHIVE_FOLDER})
  set(MOD_ARCHIVE_MOD_DIR ${MOD_ARCHIVE_TEMP_DIR}/${MOD_FOLDER})
  set(MOD_ARCHIVE_COMPRESSED_FILE_NAME ${MOD_ARCHIVE_FOLDER}-${PROJECT_VERSION}.zip)
  set(MOD_ARCHIVE_COMPRESSED_FILE_PATH ${MOD_ARCHIVE_DIR}/${MOD_ARCHIVE_COMPRESSED_FILE_NAME})

  add_custom_target(mod_archive
    COMMAND ${CMAKE_COMMAND} -E echo Creating mod archive...
    # Copy Changelog.
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG.md ${MOD_ARCHIVE_TEMP_DIR}/CHANGELOG.md
    # Copy License.
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md ${MOD_ARCHIVE_TEMP_DIR}/LICENSE.md
    # Copy Installation instructions.
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/INSTALL.md ${MOD_ARCHIVE_TEMP_DIR}/INSTALL.md
    # Move mod files directory to a temporary archive folder.
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${MOD_FILES_DIR} ${MOD_ARCHIVE_MOD_DIR}
    # Copy client.dll to temporary archive folder.
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hl_cdll> ${MOD_ARCHIVE_MOD_DIR}/cl_dlls/$<TARGET_FILE_NAME:hl_cdll>
    # Copy hl.dll to temporary archive folder.
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hldll> ${MOD_ARCHIVE_MOD_DIR}/dlls/$<TARGET_FILE_NAME:hldll>
    # Compress and create the archive.
    COMMAND ${CMAKE_COMMAND} -E chdir ${MOD_ARCHIVE_DIR} tar "cfv" ${MOD_ARCHIVE_COMPRESSED_FILE_NAME} --format=zip ${MOD_ARCHIVE_FOLDER}
    # Remove the temporary archive folder.
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${MOD_ARCHIVE_TEMP_DIR}
    # Print compressed archive file location.
    COMMAND ${CMAKE_COMMAND} -E echo Mod archive saved to ${MOD_ARCHIVE_COMPRESSED_FILE_PATH}

    VERBATIM
    DEPENDS
      hl_cdll
      hldll
  )
endif()

# Installation
install(DIRECTORY mod_files/ DESTINATION .)
install(CODE "execute_process(COMMAND cmd /C \"echo Running mod setup...\")")
install(CODE "execute_process(COMMAND cmd /C \"call SETUP.bat\" WORKING_DIRECTORY \"${MOD_DIR}\")")
