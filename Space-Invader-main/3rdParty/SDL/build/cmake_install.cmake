# Install script for directory: C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/SDL3")
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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/sdl3.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/libSDL3.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/SDL3.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/SDL3.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/SDL3.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/SDL3.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/libSDL3_test.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3/SDL3headersTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3/SDL3headersTargets.cmake"
         "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/CMakeFiles/Export/35815d1d52a6ea1175d74784b559bdb6/SDL3headersTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3/SDL3headersTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3/SDL3headersTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3" TYPE FILE FILES "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/CMakeFiles/Export/35815d1d52a6ea1175d74784b559bdb6/SDL3headersTargets.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3/SDL3sharedTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3/SDL3sharedTargets.cmake"
         "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/CMakeFiles/Export/35815d1d52a6ea1175d74784b559bdb6/SDL3sharedTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3/SDL3sharedTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3/SDL3sharedTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3" TYPE FILE FILES "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/CMakeFiles/Export/35815d1d52a6ea1175d74784b559bdb6/SDL3sharedTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3" TYPE FILE FILES "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/CMakeFiles/Export/35815d1d52a6ea1175d74784b559bdb6/SDL3sharedTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3/SDL3testTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3/SDL3testTargets.cmake"
         "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/CMakeFiles/Export/35815d1d52a6ea1175d74784b559bdb6/SDL3testTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3/SDL3testTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3/SDL3testTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3" TYPE FILE FILES "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/CMakeFiles/Export/35815d1d52a6ea1175d74784b559bdb6/SDL3testTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3" TYPE FILE FILES "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/CMakeFiles/Export/35815d1d52a6ea1175d74784b559bdb6/SDL3testTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL3" TYPE FILE FILES
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/SDL3Config.cmake"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/SDL3ConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL3" TYPE FILE FILES
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_assert.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_asyncio.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_atomic.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_audio.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_begin_code.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_bits.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_blendmode.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_camera.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_clipboard.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_close_code.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_copying.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_cpuinfo.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_dialog.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_dlopennote.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_egl.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_endian.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_error.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_events.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_filesystem.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_gamepad.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_gpu.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_guid.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_haptic.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_hidapi.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_hints.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_init.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_intrin.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_iostream.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_joystick.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_keyboard.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_keycode.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_loadso.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_locale.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_log.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_main.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_main_impl.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_messagebox.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_metal.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_misc.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_mouse.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_mutex.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_oldnames.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_opengl.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_opengl_glext.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_opengles.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_opengles2.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_opengles2_gl2.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_opengles2_gl2ext.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_opengles2_gl2platform.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_opengles2_khrplatform.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_pen.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_pixels.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_platform.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_platform_defines.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_power.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_process.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_properties.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_rect.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_render.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_scancode.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_sensor.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_stdinc.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_storage.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_surface.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_system.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_thread.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_time.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_timer.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_touch.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_tray.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_version.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_video.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_vulkan.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/include-revision/SDL3/SDL_revision.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL3" TYPE FILE FILES
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_test.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_test_assert.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_test_common.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_test_compare.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_test_crc32.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_test_font.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_test_fuzzer.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_test_harness.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_test_log.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_test_md5.h"
    "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/include/SDL3/SDL_test_memory.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/licenses/SDL3" TYPE FILE FILES "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/LICENSE.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/test/cmake_install.cmake")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/Amine/Downloads/Space-Invader-main/Space-Invader-main/3rdParty/SDL/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
