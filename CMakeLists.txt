cmake_minimum_required(VERSION 3.20)
project(Hex) 

# Définir le standard C99
set(CMAKE_C_STANDARD 99)

# Ajouter le dossier include aux chemins d'inclusion
include_directories(${CMAKE_SOURCE_DIR})

# Configuration des flags de compilation
if(MINGW)
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} " -Wall -Wextra")
elseif(MSVC)
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} " /W4")
endif()

set(CMAKE_VERBOSE_MAKEFILE YES)

# Messages de debug
message("CMAKE_VERSION          = ${CMAKE_VERSION}")
message("CMAKE_GENERATOR        = ${CMAKE_GENERATOR}")
message("MINGW | MSVC           = ${MINGW} | ${MSVC}")
message("CMAKE_BUILD_TYPE       = ${CMAKE_BUILD_TYPE}")
message("CMAKE_C_FLAGS          = ${CMAKE_C_FLAGS}")
message("CMAKE_C_FLAGS_DEBUG    = ${CMAKE_C_FLAGS_DEBUG}")
message("CMAKE_C_FLAGS_RELEASE  = ${CMAKE_C_FLAGS_RELEASE}")

add_executable(hex 
    src/main.c 
    src/libraries.c 
    src/tourspecial.c 
    include/couleurs.h 
    include/libraries.h 
    ressources/ressources.rc
)