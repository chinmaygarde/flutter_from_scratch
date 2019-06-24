cmake_minimum_required(VERSION 3.11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "Choose the type of build, options are: Debug, Release, or MinSizeRel." FORCE)
    message(STATUS "CMAKE_BUILD_TYPE not set, defaulting to MinSizeRel.")
endif()

project(flutter LANGUAGES CXX)

message(STATUS "Generator .............. ${CMAKE_GENERATOR}")
message(STATUS "Build Type ............. ${CMAKE_BUILD_TYPE}")

include_directories(
    ${CMAKE_SYSROOT}/opt/vc/include
    ${CMAKE_SYSROOT}/opt/vc/include/interface/vcos/pthreads
    ${CMAKE_SYSROOT}/opt/vc/include/interface/vmcs_host/linux
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../engine-prefix/src/engine/src/third_party/rapidjson/include
    ${CMAKE_CURRENT_SOURCE_DIR}/flutter/cpp_client_wrapper/include/flutter
    ${ENGINE_INCLUDE_DIR})

link_directories(
    ${CMAKE_SYSROOT}/opt/vc/lib
    ${ENGINE_LIBRARIES_DIR})

add_definitions(
    -DSTANDALONE -D_LINUX -DTARGET_POSIX -D_REENTRANT 
    -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 
    -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST 
    -fPIC -DPIC 
    -DUSE_RAPID_JSON
    -DGLFW_INCLUDE_ES2
    -DUSE_VCHIQ_ARM -DHAVE_LIBOPENMAX=2
    -DUSE_EXTERNAL_OMX -DOMX -DOMX_SKIP64BIT)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -U_FORTIFY_SOURCE -Wall -g -ftree-vectorize -pipe")


set(cxx_sources
    flutter/main.cc
    flutter/utils.cc
    flutter/cpp_client_wrapper/engine_method_result.cc
    flutter/cpp_client_wrapper/flutter_window_controller.cc
    flutter/cpp_client_wrapper/json_message_codec.cc
    flutter/cpp_client_wrapper/plugin_registrar.cc
    flutter/cpp_client_wrapper/standard_codec.cc
)

add_executable(flutter ${cxx_sources})
target_link_libraries(flutter ts brcmGLESv2 brcmEGL bcm_host flutter_engine pthread dl flutter_linux)

install(TARGETS flutter RUNTIME DESTINATION bin)
