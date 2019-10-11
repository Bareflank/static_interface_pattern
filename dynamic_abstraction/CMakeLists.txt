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

add_executable(dynamic_abstraction a.cpp b.cpp main.cpp)
add_executable(dynamic_abstraction_ut a.cpp b.cpp unittest.cpp)
add_executable(dynamic_abstraction_perf a.cpp b.cpp performance.cpp)

target_compile_options(dynamic_abstraction PRIVATE -O3 -DNDEBUG)
target_compile_options(dynamic_abstraction_ut PRIVATE -g)
target_compile_options(dynamic_abstraction_perf PRIVATE -O3 -DNDEBUG)
