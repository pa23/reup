cmake_minimum_required(VERSION 2.8)

set(APP_NAME reup)
set(APP_VERSION "1.3.4")

project(${APP_NAME})

set(CMAKE_BUILD_TYPE RELEASE)
set(EXECUTABLE_OUTPUT_PATH "bin")

#aux_source_directory(src SRC_LIST)

set(
    HEADERS
    src/identification.hpp
    src/constants.hpp
    src/configuration.hpp
    src/menu.hpp
    src/auxfunctions.hpp
    src/k2rei_swver.hpp
)

set(
    SOURCES
    src/main.cpp
    src/configuration.cpp
    src/menu.cpp
    src/auxfunctions.cpp
    src/k2rei_swver.cpp
)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -W -pedantic")

if(MINGW)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_SHARED_LIBS OFF)
else()
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_SHARED_LIBS ON)
endif()
find_package(Boost COMPONENTS system filesystem regex REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()
if(MINGW)
    set(BUILD_STATIC_LIBS ON)
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
else()
    set(BUILD_STATIC_LIBS OFF)
    set(BUILD_SHARED_LIBS ON)
endif()

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(
    ${PROJECT_NAME}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_LIBRARIES}
)
