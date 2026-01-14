# Install script for directory: C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/SDL3_ttf")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/msys64/ucrt64/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/harfbuzz" TYPE FILE FILES
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-aat-layout.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-aat.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-blob.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-buffer.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-common.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-cplusplus.hh"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-deprecated.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-draw.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-face.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-font.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-map.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-color.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-deprecated.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-font.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-layout.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-math.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-meta.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-metrics.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-name.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-shape.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-var.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-paint.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-set.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-shape-plan.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-shape.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-style.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-unicode.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-version.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ft.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-gdi.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/external/harfbuzz/src/hb-uniscribe.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL_ttf/build/external/harfbuzz-build/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
