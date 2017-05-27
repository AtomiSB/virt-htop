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
set(DIR_SRC  "${DIR_ROOT}/src")
set(DIR_VIRT "${DIR_SRC}/virt")
set(DIR_TUI  "${DIR_SRC}/tui")

# -- Sources --
set(SOURCES
./src/main.c
./src/arguments.c
./src/utils.c
./src/virt/virt.c
./src/virt/virt_node.c
./src/virt/virt_domain.c
./src/tui/tui.c
./src/tui/tui_node.c
./src/tui/tui_domain.c)

# -- Targets --
add_executable(${PROJECT_NAME} ${SOURCES})

# -- Include --
target_include_directories(${PROJECT_NAME} PUBLIC ${DIR_SRC} ${DIR_VIRT} ${DIR_TUI})

# -- Linking --
target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES} ${CURSES_LINK_MENU} ${LIBVIRT_LINK})

# -- Compiler flags --
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Werror)
