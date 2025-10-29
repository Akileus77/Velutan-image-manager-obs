cmake_minimum_required(VERSION 3.10)

# Project definition
project(velutan-image-manager VERSION 1.1.0 LANGUAGES CXX)

# The Velutan Image Manager is a native OBS Studio plugin written in C++17.  It
# depends on the OBS SDK (libobs and the frontend API) and Qt Widgets for the
# graphical user interface.  The CMake configuration below is kept simple
# because this project is intended as a starting point: you may need to
# customise the find_package calls depending on where OBS and Qt are installed
# on your system.

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the OBS library.  On a typical OBS build environment the CMake
# configuration files will be installed so that find_package can locate them.
# find_package(obs REQUIRED)

# Manual OBS configuration
set(OBS_BUILD_DIR "C:/Users/Akika/Downloads/obs-studio/build-sdk")
set(OBS_SOURCE_DIR "C:/Users/Akika/Downloads/obs-studio")

set(OBS_INCLUDE_DIRS
    "${OBS_SOURCE_DIR}/libobs"
    "${OBS_SOURCE_DIR}/frontend/api"
    "${OBS_BUILD_DIR}/config"
)

set(OBS_LIBRARIES
    "${OBS_BUILD_DIR}/libobs/Release/obs.lib"
    "${OBS_BUILD_DIR}/frontend/api/Release/obs-frontend-api.lib"
)

# Find Qt6 Widgets.  Modern OBS uses Qt6.  The Widgets component is
# required for QDockWidget, QDialog and other UI classes.
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Collect sources and headers.  Headers are included here for IDEs and
# generators; they are not compiled by themselves but help tools like
# Visual Studio understand the project structure.
set(VELUTAN_SOURCES
    src/module.cpp
    src/dock_widget.cpp
    src/setup_dialog.cpp
    src/obs_integration.cpp
    src/persistence.cpp
    src/asset_library.cpp
    src/ui/AssetList.cpp
    src/ui/HeaderBar.cpp
    src/ui/Toast.cpp
    src/ui/TutorialCard.cpp
    src/ui/PinnedSourcesDialog.cpp
    src/ui/GridSettingsDialog.cpp
)

set(VELUTAN_HEADERS
    src/dock_widget.hpp
    src/setup_dialog.hpp
    src/obs_integration.hpp
    src/persistence.hpp
    src/asset_library.hpp
    src/ui/AssetList.hpp
    src/ui/HeaderBar.hpp
    src/ui/Toast.hpp
    src/ui/TutorialCard.hpp
    src/ui/PinnedSourcesDialog.hpp
    src/ui/GridSettingsDialog.hpp
)

# Enable Qt MOC (Meta Object Compiler) for Qt classes with Q_OBJECT
set(CMAKE_AUTOMOC ON)

# Define the plugin target.  OBS expects plugins to be built as shared
# libraries (MODULE on CMake) without the usual 'lib' prefix on Windows.
add_library(velutan-image-manager MODULE
    ${VELUTAN_SOURCES}
    ${VELUTAN_HEADERS}
)

# Include directories.  We add the OBS include directories and Qt include
# directories, as well as the project's source directory so that headers
# resolve correctly when included with quotes.
target_include_directories(velutan-image-manager PRIVATE
    ${OBS_INCLUDE_DIRS}
    src
)

# Link against OBS and Qt.  The OBS package supplies imported targets for
# libobs and the frontend API.  Qt6::Widgets is provided by the Qt6 package.
target_link_libraries(velutan-image-manager PRIVATE
    ${OBS_LIBRARIES}
    Qt6::Widgets
)

# On Windows the plugin should be placed in a subdirectory of the OBS
# installation.  For macOS and Linux see the README for installation paths.
# This install rule is only provided as an example and may need adjustment.
install(TARGETS velutan-image-manager
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib/obs-plugins
    ARCHIVE DESTINATION lib/obs-plugins
)

# Install data files such as localisation files and the default asset
# catalogue.  OBS expects localisation files under a 'locale' directory
# adjacent to the plugin binary.  The default library is copied into the
# plugin's data folder so that it can be used if the user does not have
# their own catalogue yet.
install(DIRECTORY data/
    DESTINATION share/obs/obs-plugins/velutan-image-manager/data
)