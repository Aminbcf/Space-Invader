# Install script for directory: /home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/harfbuzz" TYPE FILE FILES
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-aat-layout.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-aat.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-blob.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-buffer.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-common.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-cplusplus.hh"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-deprecated.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-draw.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-face.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-font.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-map.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-color.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-deprecated.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-font.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-layout.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-math.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-meta.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-metrics.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-name.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-shape.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot-var.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ot.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-paint.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-set.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-shape-plan.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-shape.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-style.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-unicode.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-version.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb.h"
    "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/external/harfbuzz/src/hb-ft.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/amin/Desktop/ProjetC/AnotherOne/3rdParty/SDL_ttf/build/external/harfbuzz-build/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
