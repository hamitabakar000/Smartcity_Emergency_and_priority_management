# =============================================================================
# CMakeLists.txt pour Smart City Emergency Management System
# =============================================================================
cmake_minimum_required(VERSION 3.16)
project(SmartCityEmergency VERSION 1.0.0 LANGUAGES CXX)

# =============================================================================
# CONFIGURATION DU PROJET
# =============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options de compilation
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DEMOS "Build demos" ON)
option(ENABLE_DEBUG "Enable debug symbols" OFF)

# Configuration selon le mode (Debug/Release)
if(ENABLE_DEBUG OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE Debug)
    add_compile_definitions(DEBUG)
    message(STATUS "Building in Debug mode")
else()
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "Building in Release mode")
endif()

# =============================================================================
# CONFIGURATION RAYLIB
# =============================================================================
# Essayer de trouver Raylib avec pkg-config
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(RAYLIB QUIET raylib)
endif()

# Si pkg-config n'a pas trouvé Raylib, chercher manuellement
if(NOT RAYLIB_FOUND)
    message(STATUS "Raylib non trouvé via pkg-config, recherche manuelle...")
    
    # Chercher dans les chemins courants
    find_path(RAYLIB_INCLUDE_DIR raylib.h
        PATHS
        /usr/include
        /usr/local/include
        /opt/homebrew/include
        ${CMAKE_SOURCE_DIR}/external/raylib/include
    )
    
    find_library(RAYLIB_LIBRARY NAMES raylib
        PATHS
        /usr/lib
        /usr/local/lib
        /opt/homebrew/lib
        ${CMAKE_SOURCE_DIR}/external/raylib/lib
    )
    
    if(RAYLIB_INCLUDE_DIR AND RAYLIB_LIBRARY)
        set(RAYLIB_FOUND TRUE)
        set(RAYLIB_INCLUDE_DIRS ${RAYLIB_INCLUDE_DIR})
        set(RAYLIB_LIBRARIES ${RAYLIB_LIBRARY})
        message(STATUS "Raylib trouvé: ${RAYLIB_LIBRARY}")
    else()
        message(FATAL_ERROR "Raylib non trouvé. Installez-le avec: sudo apt install libraylib-dev")
    endif()
else()
    message(STATUS "Raylib trouvé via pkg-config")
endif()

# =============================================================================
# STRUCTURE DES RÉPERTOIRES
# =============================================================================
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(CONFIG_DIR ${CMAKE_SOURCE_DIR}/config)
set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)
set(TESTS_DIR ${CMAKE_SOURCE_DIR}/tests)
set(DEMOS_DIR ${CMAKE_SOURCE_DIR}/demos)

# Créer les répertoires de sortie
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# =============================================================================
# FICHIERS SOURCES
# =============================================================================
# Fichiers source principaux
set(MAIN_SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/simulation/simulation.cpp
    ${SRC_DIR}/vehicles/vehicle.cpp
    ${SRC_DIR}/infrastructure/traffic_light.cpp
    ${SRC_DIR}/infrastructure/road_network.cpp
)

# Fichiers d'en-tête
set(MAIN_HEADERS
    ${INCLUDE_DIR}/simulation/simulation.h
    ${INCLUDE_DIR}/vehicles/vehicle.h
    ${INCLUDE_DIR}/infrastructure/traffic_light.h
    ${INCLUDE_DIR}/infrastructure/road_network.h
    ${INCLUDE_DIR}/core/constants.h
    ${INCLUDE_DIR}/core/enums.h
    ${INCLUDE_DIR}/core/position.h
)

# =============================================================================
# EXÉCUTABLE PRINCIPAL
# =============================================================================
# Créer l'exécutable principal
add_executable(${PROJECT_NAME} ${MAIN_SOURCES} ${MAIN_HEADERS})

# Définir les répertoires d'inclusion
target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/core
    ${INCLUDE_DIR}/vehicles
    ${INCLUDE_DIR}/infrastructure
    ${INCLUDE_DIR}/simulation
    ${RAYLIB_INCLUDE_DIRS}
)

# Définir les bibliothèques à linker
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${RAYLIB_LIBRARIES}
    m
    pthread
    dl
)

# Définir les propriétés de la cible
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "smart_city_emergency"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Ajouter des définitions de compilation
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG=1>
    $<$<CONFIG:Debug>:_DEBUG=1>
)

# Options de compilation spécifiques
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wconversion
        -Wsign-conversion
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Debug>:-g3 -O0>
    )
    
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-g3>
    )
endif()

# Copier les assets vers le répertoire de build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${ASSETS_DIR}
    ${CMAKE_BINARY_DIR}/bin/assets
    COMMENT "Copying assets to build directory"
)

# Copier les fichiers de configuration
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CONFIG_DIR}
    ${CMAKE_BINARY_DIR}/bin/config
    COMMENT "Copying config files to build directory"
)

# =============================================================================
# TESTS UNITAIRE
# =============================================================================
if(BUILD_TESTS)
    message(STATUS "Building tests...")
    
    # Chercher Google Test
    find_package(GTest QUIET)
    
    if(GTest_FOUND)
        # Fichiers de test
        set(TEST_SOURCES
            ${TESTS_DIR}/test_vehicle.cpp
            ${TESTS_DIR}/test_traffic_light.cpp
            ${TESTS_DIR}/test_simulation.cpp
            ${TESTS_DIR}/test_position.cpp
        )
        
        # Créer l'exécutable de test
        add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES} ${MAIN_SOURCES})
        
        # Configurer les tests
        target_include_directories(${PROJECT_NAME}_tests PRIVATE
            ${INCLUDE_DIR}
            ${GTEST_INCLUDE_DIRS}
        )
        
        target_link_libraries(${PROJECT_NAME}_tests PRIVATE
            ${RAYLIB_LIBRARIES}
            ${GTEST_LIBRARIES}
            GTest::gtest
            GTest::gtest_main
            m
            pthread
            dl
        )
        
        # Définir le répertoire de sortie
        set_target_properties(${PROJECT_NAME}_tests PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
        )
        
        # Ajouter une cible pour lancer les tests
        add_custom_target(run_tests
            COMMAND ${CMAKE_BINARY_DIR}/tests/${PROJECT_NAME}_tests
            DEPENDS ${PROJECT_NAME}_tests
            COMMENT "Running unit tests"
        )
        
        message(STATUS "Tests enabled with Google Test")
        
    else()
        # Tests simples sans Google Test
        message(STATUS "Google Test not found, building simple test executable")
        
        set(SIMPLE_TEST_SOURCES
            ${TESTS_DIR}/simple_test.cpp
            ${SRC_DIR}/core/position.cpp
        )
        
        if(EXISTS ${TESTS_DIR}/simple_test.cpp)
            add_executable(simple_test ${SIMPLE_TEST_SOURCES})
            target_include_directories(simple_test PRIVATE ${INCLUDE_DIR})
            set_target_properties(simple_test PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
            )
        endif()
    endif()
endif()

# =============================================================================
# DÉMOS
# =============================================================================
if(BUILD_DEMOS)
    message(STATUS "Building demos...")
    
    # Démo simple
    if(EXISTS ${DEMOS_DIR}/demo_simple.cpp)
        add_executable(demo_simple ${DEMOS_DIR}/demo_simple.cpp)
        target_include_directories(demo_simple PRIVATE ${INCLUDE_DIR})
        target_link_libraries(demo_simple PRIVATE ${RAYLIB_LIBRARIES} m pthread dl)
        set_target_properties(demo_simple PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/demos"
        )
    endif()
    
    # Démo d'urgence
    if(EXISTS ${DEMOS_DIR}/demo_emergency.cpp)
        add_executable(demo_emergency ${DEMOS_DIR}/demo_emergency.cpp ${SRC_DIR}/vehicles/vehicle.cpp)
        target_include_directories(demo_emergency PRIVATE ${INCLUDE_DIR})
        target_link_libraries(demo_emergency PRIVATE ${RAYLIB_LIBRARIES} m pthread dl)
        set_target_properties(demo_emergency PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/demos"
        )
    endif()
endif()

# =============================================================================
# INSTALLATION
# =============================================================================
# Configuration de l'installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

install(DIRECTORY ${INCLUDE_DIR}/
    DESTINATION include/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${CONFIG_DIR}/
    DESTINATION share/${PROJECT_NAME}/config
)

install(DIRECTORY ${ASSETS_DIR}/
    DESTINATION share/${PROJECT_NAME}/assets
)

# =============================================================================
# TARGETS PERSONNALISÉS
# =============================================================================
# Nettoyage
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/tests
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/demos
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/lib
    COMMENT "Cleaning all build artifacts"
)

# Build et exécution
add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    COMMENT "Running ${PROJECT_NAME}"
)

# Build, exécution et nettoyage
add_custom_target(rebuild
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${PROJECT_NAME}
    COMMAND ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    COMMENT "Rebuilding and running ${PROJECT_NAME}"
)

# =============================================================================
# INFORMATIONS DE CONFIGURATION
# =============================================================================
message(STATUS "")
message(STATUS "==========================================")
message(STATUS "Smart City Emergency - Configuration Summary")
message(STATUS "==========================================")
message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "")
message(STATUS "Features:")
message(STATUS "  - Tests: ${BUILD_TESTS}")
message(STATUS "  - Demos: ${BUILD_DEMOS}")
message(STATUS "  - Debug: ${ENABLE_DEBUG}")
message(STATUS "")
message(STATUS "Raylib:")
message(STATUS "  - Found: ${RAYLIB_FOUND}")
message(STATUS "  - Include: ${RAYLIB_INCLUDE_DIRS}")
message(STATUS "  - Library: ${RAYLIB_LIBRARIES}")
message(STATUS "")
message(STATUS "Available targets:")
message(STATUS "  - make ${PROJECT_NAME}     # Build main executable")
message(STATUS "  - make run              # Build and run")
message(STATUS "  - make run_tests        # Run tests (if enabled)")
message(STATUS "  - make clean            # Clean build")
message(STATUS "  - make clean_all        # Clean everything")
message(STATUS "  - make rebuild          # Clean, rebuild and run")
message(STATUS "")
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "Output executable: ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}")
message(STATUS "==========================================")