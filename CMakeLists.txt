# Copyright (C) Sebastian Bialobrzecki <sbb@openmailbox.org>
# -- Names --
cmake_minimum_required(VERSION 3.0)
project(virt-htop)

# -- Requirements --
# Libvirt
set(LIBVIRT_LINK "-lvirt")

# Curses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})
set(CURSES_LINK_MENU "-lmenu")

# -- Directories --
set(DIR_ROOT ".")
set(DIR_SRC "${DIR_ROOT}/src")

# -- Sources --
set(SOURCES
./src/main.c
./src/utils.c
./src/virt.c
./src/arguments.c
./src/tui.c
./src/tui_node.c
./src/tui_domain.c)

# -- Targets --
add_executable(${PROJECT_NAME} ${SOURCES})

# -- Include --
target_include_directories(${PROJECT_NAME} PUBLIC ${DIR_SRC})

# -- Linking --
target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES} ${CURSES_LINK_MENU} ${LIBVIRT_LINK})

# -- Compiler flags --
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Werror)
