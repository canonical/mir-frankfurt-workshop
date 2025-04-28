cmake_minimum_required(VERSION 3.16)
project(mirly)

set(CMAKE_CXX_STANDARD 20)

# Find the MirAL library using pkg-config.
# This assumes that pkg-config is installed and configured correctly.
# You may need to adjust the path to pkg-config if it's not in your PATH.
find_package(PkgConfig REQUIRED)
pkg_check_modules(MIRAL miral REQUIRED)
pkg_check_modules(XKBCOMMON xkbcommon REQUIRED)

# Create an executable target for the mirly application.
add_executable(mirly
    src/main.cpp
)

# Include the MirAL headers and link against the MirAL library.
target_include_directories(mirly
    PUBLIC
    ${MIRAL_INCLUDE_DIRS} 
)

target_link_libraries(mirly
    ${MIRAL_LDFLAGS}
    ${XKBCOMMON_LIBRARIES}
)

install(PROGRAMS ${CMAKE_BINARY_DIR}/frame
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
