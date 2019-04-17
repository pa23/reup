cmake_minimum_required(VERSION 3.10)

set(APP_NAME reup)
set(APP_VERSION "1.13.1")

project(${APP_NAME})

set(CMAKE_BUILD_TYPE RELEASE)
set(EXECUTABLE_OUTPUT_PATH "bin")

set(
    HEADERS
    src/identification.hpp
    src/constants.hpp
    src/configuration.hpp
    src/menu.hpp
    src/auxfunctions.hpp
    src/k2rei_swver.hpp
    src/md5.hpp
)

set(
    SOURCES
    src/main.cpp
    src/configuration.cpp
    src/menu.cpp
    src/auxfunctions.cpp
    src/k2rei_swver.cpp
    src/md5.cpp
)

if(MINGW)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_SHARED_LIBS OFF)
    set(BUILD_STATIC_LIBS ON)
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
else()
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_SHARED_LIBS ON)
    set(BUILD_STATIC_LIBS OFF)
    set(BUILD_SHARED_LIBS ON)
endif()

#set(Boost_COMPILER -mgw81)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID x64)
set(Boost_USE_MULTITHREADED ON)
#set(Boost_DEBUG ON)

find_package(Boost COMPONENTS system filesystem regex REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
    target_link_libraries(${PROJECT_NAME} Boost::system Boost::filesystem Boost::regex)
endif()
