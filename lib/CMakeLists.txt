cmake_minimum_required(VERSION 3.5)
project(spotify-qt-lib LANGUAGES CXX VERSION 0.4)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional Qt implementations
if (LIB_QT_IMPL)
	add_subdirectory(qt)
endif ()

# Source files
file(GLOB MAIN_SRC "src/*.cpp")
file(GLOB SUB_SRC "src/*/*.cpp")
# Header files
file(GLOB MAIN_INC "include/lib/*.hpp")
file(GLOB SUB_INC "include/lib/*/*.hpp")
# Add all files
add_library(spotify-qt-lib STATIC ${MAIN_SRC} ${SUB_SRC} ${MAIN_INC} ${SUB_INC} ${LIB_QT_SOURCES})

# Main source folder has the headers
target_include_directories(spotify-qt-lib PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/include"
	${LIB_QT_HEADERS})

# Version macros
target_compile_definitions(spotify-qt-lib PUBLIC LIB_VERSION="v${PROJECT_VERSION}")

# Check if using GCC for crash handler support
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_definitions(spotify-qt-lib PRIVATE IS_GNU_CXX)
endif ()

# SQLite cache support
if (NOT DEFINED USE_DB_CACHE)
	set(USE_DB_CACHE ON)
endif ()

# TODO: On error, use bundled SQLite instead
if (USE_DB_CACHE)
	find_package(PkgConfig QUIET)
	if (PkgConfig_FOUND)
		pkg_check_modules(SQLITE sqlite3 QUIET)
		if (SQLITE_FOUND)
			target_link_directories(spotify-qt-lib PRIVATE "${SQLITE_LIBRARY_DIRS}")
			target_include_directories(spotify-qt-lib PRIVATE "${SQLITE_INCLUDE_DIRS}")
			target_link_libraries(spotify-qt-lib PRIVATE "${SQLITE_LIBRARIES}")
			message(STATUS "Using SQLite ${SQLITE_VERSION} (system)")
		else ()
			message(WARNING "SQLite error: sqlite3 not found")
		endif ()
	else ()
		message(WARNING "SQLite error: pkg-config not found")
	endif ()
endif ()

# Link optional libraries
if (LIB_QT_LIBRARIES)
	target_link_libraries(spotify-qt-lib PRIVATE ${LIB_QT_LIBRARIES})
endif ()

# Unit testing
if (USE_TESTS)
	add_subdirectory(test)
endif ()
