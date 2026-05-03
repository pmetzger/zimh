# Locate the PCRE library
#
# This module defines:
#
# ::
#
#   PCRE2::PCRE2, an imported target for pcre2
#   PCRE2_LIBRARIES, the name of the pcre2 library to link against
#   PCRE2_INCLUDE_DIRS, where to find the pcre2 headers
#   PCRE2_FOUND, if false, do not try to compile or link with pcre2
#   PCRE2_STATIC_LINKAGE, if true, define PCRE2_STATIC when compiling
#   PCRE2_VERSION_STRING, human-readable pcre2 version string
#
# Tweaks:
# 1. PCRE_PATH: A list of directories in which to search
# 2. PCRE_DIR: An environment variable pointing to an installed PCRE.
# 3. PCRE2_USE_STATIC_LIBS: Prefer static PCRE2 library names.
#
# "scooter me fecit"

find_path(PCRE2_INCLUDE_DIR pcre2.h
        HINTS
          ENV PCRE_DIR
        # path suffixes to search inside ENV{PCRE_DIR}
        PATHS ${PCRE_PATH}
        PATH_SUFFIXES
            include/pcre
            include/PCRE
            include
        )

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(LIB_PATH_SUFFIXES lib64 x64 amd64 x86_64-linux-gnu aarch64-linux-gnu lib)
else ()
  set(LIB_PATH_SUFFIXES x86)
endif ()

if (PCRE2_USE_STATIC_LIBS)
    set(PCRE2_NAMES
        pcre2-8-static
        pcre2-8)
    set(PCRE2_DEBUG_NAMES
        pcre2-8-staticd
        pcre2-8d)
else ()
    set(PCRE2_NAMES
        pcre2-8
        pcre2-8-static)
    set(PCRE2_DEBUG_NAMES
        pcre2-8d
        pcre2-8-staticd)
endif ()

if (PCRE2_USE_STATIC_LIBS AND NOT WIN32)
    set(PCRE2_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES
        ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX})
endif ()

find_library(PCRE2_LIBRARY_RELEASE
        NAMES
            ${PCRE2_NAMES}
        HINTS
            ENV PCRE_DIR
        PATH_SUFFIXES
            ${LIB_PATH_SUFFIXES}
        PATHS
            ${PCRE_PATH}
        )

find_library(PCRE2_LIBRARY_DEBUG
        NAMES
            ${PCRE2_DEBUG_NAMES}
        HINTS
            ENV PCRE_DIR
        PATH_SUFFIXES
            ${LIB_PATH_SUFFIXES}
        PATHS
            ${PCRE_PATH}
        )

if (DEFINED PCRE2_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES)
    set(CMAKE_FIND_LIBRARY_SUFFIXES
        ${PCRE2_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
    unset(PCRE2_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES)
endif ()

if (PCRE2_INCLUDE_DIR)
    if (EXISTS "${PCRE2_INCLUDE_DIR}/pcre2.h")
        file(STRINGS "${PCRE2_INCLUDE_DIR}/pcre2.h" PCRE2_VERSION_MAJOR_LINE REGEX "^#define[ \t]+PCRE2_MAJOR[ \t]+[0-9]+$")
        file(STRINGS "${PCRE2_INCLUDE_DIR}/pcre2.h" PCRE2_VERSION_MINOR_LINE REGEX "^#define[ \t]+PCRE2_MINOR[ \t]+[0-9]+$")
    endif ()

    string(REGEX REPLACE "^#define[ \t]+PCRE2?_MAJOR[ \t]+([0-9]+)$" "\\1" PCRE2_VERSION_MAJOR "${PCRE2_VERSION_MAJOR_LINE}")
    string(REGEX REPLACE "^#define[ \t]+PCRE2?_MINOR[ \t]+([0-9]+)$" "\\1" PCRE2_VERSION_MINOR "${PCRE2_VERSION_MINOR_LINE}")

    set(PCRE2_VERSION_STRING "${PCRE2_VERSION_MAJOR}.${PCRE2_VERSION_MINOR}")
    unset(PCRE2_VERSION_MAJOR_LINE)
    unset(PCRE2_VERSION_MINOR_LINE)
    unset(PCRE2_VERSION_MAJOR)
    unset(PCRE2_VERSION_MINOR)
endif ()

include(SelectLibraryConfigurations)

SELECT_LIBRARY_CONFIGURATIONS(PCRE2)

## message("== PCRE_INCLUDE_DIR ${PCRE_INCLUDE_DIR}")
## message("== PCRE2_LIBRARY ${PCRE2_LIBRARY}")
## message("== PCRE2_LIBRARIES ${PCRE2_LIBRARIES}")
## message("== PCRE2_LIBRARY_DEBUG ${PCRE2_LIBRARY_DEBUG}")
## message("== PCRE2_LIBRARY_RELEASE ${PCRE2_LIBRARY_RELEASE}")

set(PCRE2_INCLUDE_DIRS ${PCRE2_INCLUDE_DIR})
set(PCRE2_LIBRARIES ${PCRE2_LIBRARY})

set(PCRE2_STATIC_LINKAGE FALSE)
if (PCRE2_USE_STATIC_LIBS)
    set(PCRE2_STATIC_LINKAGE TRUE)
else ()
    set(PCRE2_SELECTED_LIBRARIES
        ${PCRE2_LIBRARY_RELEASE}
        ${PCRE2_LIBRARY_DEBUG})
    if (NOT PCRE2_SELECTED_LIBRARIES)
        set(PCRE2_SELECTED_LIBRARIES ${PCRE2_LIBRARY})
    endif ()

    foreach (PCRE2_SELECTED_LIBRARY IN LISTS PCRE2_SELECTED_LIBRARIES)
        get_filename_component(PCRE2_LIBRARY_NAME
                               "${PCRE2_SELECTED_LIBRARY}" NAME_WE)
        if (PCRE2_LIBRARY_NAME MATCHES "pcre2-8-staticd?$")
            set(PCRE2_STATIC_LINKAGE TRUE)
        endif ()
    endforeach ()

    unset(PCRE2_SELECTED_LIBRARIES)
    unset(PCRE2_SELECTED_LIBRARY)
    unset(PCRE2_LIBRARY_NAME)
endif ()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    PCRE2
    REQUIRED_VARS
        PCRE2_LIBRARY
        PCRE2_INCLUDE_DIR
    VERSION_VAR
        PCRE2_VERSION_STRING
)

if (PCRE2_FOUND AND NOT TARGET PCRE2::PCRE2)
    add_library(PCRE2::PCRE2 UNKNOWN IMPORTED)
    set_target_properties(PCRE2::PCRE2 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${PCRE2_INCLUDE_DIRS}")

    if (PCRE2_LIBRARY_RELEASE AND PCRE2_LIBRARY_DEBUG)
        set_target_properties(PCRE2::PCRE2 PROPERTIES
            IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
            IMPORTED_LOCATION "${PCRE2_LIBRARY_RELEASE}"
            IMPORTED_LOCATION_RELEASE "${PCRE2_LIBRARY_RELEASE}"
            IMPORTED_LOCATION_DEBUG "${PCRE2_LIBRARY_DEBUG}"
            MAP_IMPORTED_CONFIG_MINSIZEREL RELEASE
            MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)
    elseif (PCRE2_LIBRARY_RELEASE)
        set_target_properties(PCRE2::PCRE2 PROPERTIES
            IMPORTED_LOCATION "${PCRE2_LIBRARY_RELEASE}")
    elseif (PCRE2_LIBRARY_DEBUG)
        set_target_properties(PCRE2::PCRE2 PROPERTIES
            IMPORTED_LOCATION "${PCRE2_LIBRARY_DEBUG}")
    else ()
        set_target_properties(PCRE2::PCRE2 PROPERTIES
            IMPORTED_LOCATION "${PCRE2_LIBRARY}")
    endif ()

    if (PCRE2_STATIC_LINKAGE)
        set_property(TARGET PCRE2::PCRE2 APPEND PROPERTY
            INTERFACE_COMPILE_DEFINITIONS PCRE2_STATIC)
    endif ()
endif ()

unset(PCRE2_NAMES)
unset(PCRE2_DEBUG_NAMES)
