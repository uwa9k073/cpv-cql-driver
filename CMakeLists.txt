cmake_minimum_required(VERSION 3.15)
project(CQLDriver VERSION 0.6.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Options
option(BUILD_SHARED_LIBS "Build shared library" OFF)
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(CQL_ENABLE_DEBUG_LOGGER_BY_DEFAULT "Enable debug logger by default" OFF)

# Find Seastar
find_package(Seastar REQUIRED)
find_package(fmt REQUIRED)

# Include directories
include(GNUInstallDirs)

# Add library
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE PUBLIC_HEADERS "include/*.hpp")
file(GLOB_RECURSE INTERNAL_HEADERS "src/*.hpp")

if(BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED ${SOURCES} ${PUBLIC_HEADERS} ${INTERNAL_HEADERS})
else()
  add_library(${PROJECT_NAME} STATIC ${SOURCES} ${PUBLIC_HEADERS} ${INTERNAL_HEADERS})
endif()

# Set target properties
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra
    -Wno-unused-variable -Wno-unused-function
    -ftls-model=initial-exec -fPIC -fvisibility=default
)

# Link against Seastar
target_link_libraries(${PROJECT_NAME} PRIVATE Seastar::seastar fmt::fmt)

if(ENABLE_SANITIZERS)
  target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
  target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
endif()

# Debug logger option
if(CQL_ENABLE_DEBUG_LOGGER_BY_DEFAULT)
  target_compile_definitions(${PROJECT_NAME} PRIVATE CQL_ENABLE_DEBUG_LOGGER_BY_DEFAULT)
endif()

# Build type specific settings
if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo OR CMAKE_BUILD_TYPE MATCHES MinSizeRel)
  target_compile_options(${PROJECT_NAME} PRIVATE -O3)
  target_link_options(${PROJECT_NAME} PRIVATE -flto -fuse-ld=lld)
endif()

# Install rules
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/CQLDriver DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/licenses/CQLDriver)

# Tests
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# Examples
if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
