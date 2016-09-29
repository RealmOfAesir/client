cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(RealmOfAesir CXX)
#set(CMAKE_CXX_STANDARD 17) #not supported yet :(
#set(CMAKE CXX STANDARD REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

include_directories("${PROJECT_SOURCE_DIR}")

file(GLOB PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(RealmOfAesir ${PROJECT_SOURCES})

include(FindPkgConfig)
include(FindOpenGL)
include(FindGLEW)

if(NOT OPENGL_GLU_FOUND)
  message(FATAL_ERROR "Either OpenGL or GLU not found")
endif()

if(NOT GLEW_FOUND)
  message(FATAL_ERROR "GLEW not found")
endif()

pkg_search_module(SDL2 REQUIRED sdl2>=2.0.4)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

include_directories("${SDL2_INCLUDE_DIRS}")
include_directories("${SDL2IMAGE_INCLUDE_DIRS}")
include_directories("${OPENGL_INCLUDE_DIR}")
include_directories("${GLEW_INCLUDE_DIRS}")

target_link_libraries(RealmOfAesir ${SDL2_LIBRARIES})
target_link_libraries(RealmOfAesir ${SDL2IMAGE_LIBRARIES})
target_link_libraries(RealmOfAesir ${OPENGL_LIBRARIES})
target_link_libraries(RealmOfAesir ${GLEW_LIBRARIES})


#post-build esque stuff
add_custom_command(
        TARGET RealmOfAesir POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/src/shaders/
                ${CMAKE_BINARY_DIR}/shaders)