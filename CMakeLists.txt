cmake_minimum_required(VERSION 3.6)
project(prime_watch_external)
set(NAME prime_watch_external)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp common.cpp)

if (WIN32 OR CYGWIN)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    message("Windows target")
    set(SOURCE_FILES ${SOURCE_FILES} windump.cpp)

    SET(BUILD_SHARED_LIBRARIES OFF)
    SET(CMAKE_EXE_LINKER_FLAGS "-static")
elseif (APPLE)
    message("Apple target")
    set(SOURCE_FILES ${SOURCE_FILES} unixdump.cpp dolphinpid.m)
    SET(CMAKE_EXE_LINKER_FLAGS "-framework Foundation -framework AppKit -w")
    set(CMAKE_CXX_FLAGS "-Wall")
    set(CMAKE_C_FLAGS "-x objective-c")
else ()
    message("unix target")
    set(SOURCE_FILES ${SOURCE_FILES} unixdump.cpp)
endif ()

if (APPLE)
    add_executable(
            ${NAME}
            MACOSX_BUNDLE
            ${HEADER_FILES}
            ${SOURCE_FILES}
    )
else ()
    add_executable(prime_watch_external ${SOURCE_FILES})
endif ()

if (WIN32)
    target_link_libraries(prime_watch_external wsock32 ws2_32)
endif ()