#
# Bareflank Hypervisor
# Copyright (C) 2019 Assured Information Security, Inc.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

cmake_minimum_required(VERSION 3.13)
project(static_abstract_interface_pattern CXX)

include(ExternalProject)

# ------------------------------------------------------------------------------
# flags
# ------------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--strip-all")

# ------------------------------------------------------------------------------
# source
# ------------------------------------------------------------------------------

add_subdirectory(bad)
add_subdirectory(dynamic_abstraction)
add_subdirectory(static_abstraction)
