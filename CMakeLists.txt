cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

project(multisockets VERSION 1.0.1 DESCRIPTION "libmultisockets provides a common interface for using sockets under different operating systems")


add_library(${PROJECT_NAME} SHARED src/multisockets.c)

target_include_directories(${PROJECT_NAME} PRIVATE include)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER include/multisockets.h)

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})