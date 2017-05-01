cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
project(RealmOfAesir CXX)
#set(CMAKE_CXX_STANDARD 17) #not supported yet :(
#set(CMAKE CXX STANDARD REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DELPP_THREAD_SAFE -Wall -Wno-unused-variable -Wno-long-long -Wfloat-equal -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "-g3 -ggdb -O0")
  set(CMAKE_CXX_FLAGS_RELEASE "-g -O2")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

include_directories("${PROJECT_SOURCE_DIR}")

file(GLOB PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB EASYLOGGING_SOURCE ${CMAKE_SOURCE_DIR}/external/common/external/easyloggingpp/src/easylogging++.cc)
add_executable(RealmOfAesir ${EASYLOGGING_SOURCE} ${PROJECT_SOURCES})

include(FindOpenGL)
include(FindGLEW)

if(NOT OPENGL_GLU_FOUND)
  message(FATAL_ERROR "Either OpenGL or GLU not found")
endif()

if(NOT GLEW_FOUND)
  message(FATAL_ERROR "GLEW not found")
endif()

#versions don't seem to do anything, but it serves as a central point for which version are necessary.
find_package(SDL2 REQUIRED >= 2.0.4)
find_package(SDL2_image REQUIRED >= 2.0.0)
find_package(GLM REQUIRED >= 0.9.7)

find_library(LIBROA_COMMON_LIBRARY RealmOfAesirCommon HINTS ${CMAKE_SOURCE_DIR}/external/common/bin)
find_library(LIBRDKAFKA_LIBRARY rdkafka HINTS ${CMAKE_SOURCE_DIR}/external/common/external/librdkafka/src)
find_library(LIBRDKAFKAPP_LIBRARY rdkafka++ HINTS ${CMAKE_SOURCE_DIR}/external/common/external/librdkafka/src-cpp)

include_directories("${SDL2_INCLUDE_DIR}")
include_directories("${SDL2_IMAGE_INCLUDE_DIRS}")
include_directories("${OPENGL_INCLUDE_DIR}")
include_directories("${GLEW_INCLUDE_DIRS}")
include_directories("${GLM_INCLUDE_DIRS}")
include_directories("${CMAKE_SOURCE_DIR}/external/common/external/librdkafka/src-cpp")
include_directories("${CMAKE_SOURCE_DIR}/external/common/external/librdkafka/src")
include_directories("${CMAKE_SOURCE_DIR}/external/common/external/easyloggingpp/src")

if(NOT SDL2_LIBRARY)
    message(FATAL_ERROR "sdl2 not found")
endif()

if(NOT SDL2_IMAGE_LIBRARIES)
    message(FATAL_ERROR "sdl2image not found")
endif()

if(NOT OPENGL_LIBRARIES)
    message(FATAL_ERROR "opengl not found")
endif()

if(NOT GLEW_LIBRARIES)
    message(FATAL_ERROR "glew not found")
endif()

if(NOT LIBROA_COMMON_LIBRARY)
    message(FATAL_ERROR "libroa not found")
endif()

if(NOT LIBRDKAFKAPP_LIBRARY)
    message(FATAL_ERROR "kafka++ not found")
endif()

if(NOT LIBRDKAFKA_LIBRARY)
    message(FATAL_ERROR "kafka not found")
endif()

target_link_libraries(RealmOfAesir ${SDL2_LIBRARY})
target_link_libraries(RealmOfAesir ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(RealmOfAesir ${OPENGL_LIBRARIES})
target_link_libraries(RealmOfAesir ${GLEW_LIBRARIES})
target_link_libraries(RealmOfAesir ${LIBROA_COMMON_LIBRARY})
target_link_libraries(RealmOfAesir ${LIBRDKAFKAPP_LIBRARY})
target_link_libraries(RealmOfAesir ${LIBRDKAFKA_LIBRARY})

#post-build esque stuff
add_custom_command(
        TARGET RealmOfAesir POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/src/shaders/
                ${CMAKE_BINARY_DIR}/shaders)

add_custom_command(
        TARGET RealmOfAesir POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/assets/
                ${CMAKE_BINARY_DIR}/assets)
