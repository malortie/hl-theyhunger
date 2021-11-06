# Building The Code

## Supported Platforms

See [supported platforms](README.md#Supported-Platforms).

## Prerequisites

- CMake 3.21 or higher
- Visual Studio 2019

## Configuring CMake

The following variables must be configured.

- `CMAKE_INSTALL_PREFIX`: The install directory. Must be set to the mod directory in Steam directory. e.g.

  ```text
  C:\Program Files (x86)\steamapps\common\Half-Life\<MyMod>
  ```

- `ENABLE_MOD_ARCHIVE`: Whether or not to add the archive project.

  - `MOD_ARCHIVE_DIR`: The directory where the archive will be saved.

  - `MOD_ARCHIVE_FOLDER`: The name of the archive folder in which temporary files will be copied before compression. This folder will be created inside the directory specified by `MOD_ARCHIVE_DIR` and the compressed archive file will have the same name.

## First Time Build

Build the `INSTALL` project. This will build `hl_cdll` and `hldll` projects and will copy all mod files to the mod directory (Install directory) and run the setup script automatically.

## Launching From Visual Studio

1. Ensure Steam is running.
2. Right click on either `hl_cdll` or `hldll` and select Debug -> Start New Instance

## Projects

- `INSTALL`: Builds hl_cdll and hldll projects, copies all mod files to the mod directory, runs the setup script.
- `hl_cdll`: Game client project
- `hldll`: Game server project
- `mod_archive`: Creates a compressed archive of all mod data.
