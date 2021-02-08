cmake_minimum_required(VERSION 3.4.1)
project(libogg)

message('CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}')

# set(OGG_HEADERS
#     ${CMAKE_CURRENT_BINARY_DIR}/include/ogg/config_types.h
#     include/ogg/ogg.h
#     include/ogg/os_types.h
# )

# set(OGG_SOURCES
#     src/bitwise.c
#     src/framing.c
#     src/crctable.h
# )

# add_library(ogg ${OGG_HEADERS} ${OGG_SOURCES})
# target_include_directories(ogg PUBLIC
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
#     $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
# )