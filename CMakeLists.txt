# Copyright Florian Philipp 2014
#
# This file is part of FsSort.
#
# FsSort is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# FsSort is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with FsSort.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.6)
include(FindPkgConfig)

project(FsSort)

if(NOT CMAKE_BUILD_TYPE)
  message(WARNING "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(ADD_CFLAGS "-Wall ${EXT2FS_CFLAGS} ${COM_ERR_CFLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ADD_CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADD_CFLAGS}")

set(SRC
  src/caps.cpp
  src/fssort.cpp
  src/opts.cpp
  src/sort.cpp
  src/verbose.cpp
)
add_executable(fssort ${SRC})

pkg_check_modules(EXT2FS ext2fs)
pkg_check_modules(COM_ERR com_err)
link_directories(
  ${EXT2FS_LIBRARY_DIRS}
  ${COM_ERR_LIBRARY_DIRS}
)
include_directories(
  ${EXT2FS_INCLUDE_DIRS}
  ${COM_ERR_INCLUDE_DIRS}
)
target_link_libraries(fssort
  ${EXT2FS_LIBRARIES}
  ${COM_ERR_LIBRARIES}
  cap
)

install(TARGETS fssort
  DESTINATION bin
)
