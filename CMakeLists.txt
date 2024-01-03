cmake_minimum_required(VERSION 3.28.1)

project(multisockets VERSION 1.0.0 DESCRIPTION "libmultisockets provides a common interface for using sockets under different operating systems")


add_library(${PROJECT_NAME} SHARED src/multisockets.c)

target_include_directories(${PROJECT_NAME} PRIVATE include)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER include/multisockets.h)

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
