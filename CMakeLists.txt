cmake_minimum_required(VERSION 3.0.2)
project(NWNX-Unified-ApiGen)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(CMP_MSVC 1)
    add_definitions(-DCMP_MSVC)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    set(CMP_CLANG 1)
    add_definitions(-DCMP_CLANG)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Intel)
    set(CMP_INTEL 1)
    add_definitions(-DCMP_INTEL)
else() # If we can't detect a compiler, we presume GCC.
    set(CMP_GCC 1)
    add_definitions(-DCMP_GCC)
endif()

if(WIN32)
    set(OS_WINDOWS 1)
    add_definitions(-DOS_WINDOWS)
else() # If we can't detect an OS, we presume Linux.
    set(OS_LINUX 1)
    add_definitions(-DOS_LINUX)

    if(NOT UNIX)
        message("Attempting to build for an unknown platform. Presuming Linux.")
    endif()
endif()


if(CMP_MSVC)
    # C++17 support
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")

    # Multithreaded compilation
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

    # Disable warnings for usage of compliant functions.
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    # Release final gets a normal PDB.
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG:FULL")

    # RelWithDebInfo gets Edit-And-Continue support.
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Zi")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /DEBUG:FASTLINK")

    # Debug gets Edit-And-Continue support.
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG:FASTLINK")

    # Define targets
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DTAR_RELEASE_FINAL -DTAR_RELEASE")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DTAR_RELEASE -DTAR_DEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DTAR_DEBUG")
endif()

if(CMP_GCC OR CMP_CLANG)
    # C++17 support
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

    # Various warnings ...
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-security")

    # TODO: Define targets
endif()

set(ARTIFACTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Build-Artifacts")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ARTIFACTS_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ARTIFACTS_DIR}/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(External)
add_subdirectory(Utility)
add_subdirectory(Targets)
add_subdirectory(ApiGen)
