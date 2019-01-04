cmake_minimum_required(VERSION 3.11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "Choose the type of build, options are: Debug, Release, or MinSizeRel." FORCE)
    message(STATUS "CMAKE_BUILD_TYPE not set, defaulting to MinSizeRel.")
endif()

project(hello_pi LANGUAGES CXX)

message(STATUS "Generator .............. ${CMAKE_GENERATOR}")
message(STATUS "Build Type ............. ${CMAKE_BUILD_TYPE}")

include_directories(
    ${CMAKE_SYSROOT}/opt/vc/include
    ${CMAKE_SYSROOT}/opt/vc/include/interface/vcos/pthreads
    ${CMAKE_SYSROOT}/opt/vc/include/interface/vmcs_host/linux
    ${ENGINE_INCLUDE_DIR})

link_directories(
    ${CMAKE_SYSROOT}/opt/vc/lib
    ${ENGINE_LIBRARIES_DIR})

add_definitions(
    -DSTANDALONE -D_LINUX -DTARGET_POSIX -D_REENTRANT 
    -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 
    -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST 
    -fPIC -DPIC 
    -DUSE_VCHIQ_ARM -DHAVE_LIBOPENMAX=2
    -DUSE_EXTERNAL_OMX -DOMX -DOMX_SKIP64BIT)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -U_FORTIFY_SOURCE -Wall -g -ftree-vectorize -pipe")


set(cxx_sources
    flutter/flutter_application.cc
    flutter/main.cc
    flutter/pi_display.cc
    flutter/utils.cc
)

add_executable(flutter ${cxx_sources})
target_link_libraries(flutter brcmGLESv2 brcmEGL bcm_host flutter_engine pthread dl)

install(TARGETS flutter RUNTIME DESTINATION bin)
