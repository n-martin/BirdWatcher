cmake_minimum_required(VERSION 3.0.0)
project(BirdWatcher VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Determine platform specific Spinnaker directory
math(EXPR BITS "8*${CMAKE_SIZEOF_VOID_P}")
if(UNIX AND NOT APPLE)
    set(SPINNAKER_ROOT ${PROJECT_SOURCE_DIR}/dependencies/spinnaker/linux)
elseif(APPLE)
    set(SPINNAKER_ROOT ${PROJECT_SOURCE_DIR}/dependencies/spinnaker/macOS)
else()
    set(SPINNAKER_ROOT ${PROJECT_SOURCE_DIR}/dependencies/spinnaker/windows)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include(CTest)
enable_testing()

# INCLUDES
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${SPINNAKER_ROOT}/../include)

# LIBRARIES
add_library(SpinnakerLibrary SHARED IMPORTED)
add_library(SpinVideoLibrary SHARED IMPORTED)
if(UNIX AND NOT APPLE)
    set_target_properties(SpinnakerLibrary PROPERTIES IMPORTED_LOCATION "${SPINNAKER_ROOT}/lib/libSpinnaker.so")
    set_target_properties(SpinVideoLibrary PROPERTIES IMPORTED_LOCATION "${SPINNAKER_ROOT}/lib/libSpinVideo.so")
elseif(APPLE)
    set_target_properties(SpinnakerLibrary PROPERTIES IMPORTED_LOCATION "${SPINNAKER_ROOT}/lib/libSpinnaker.dylib")
    set_target_properties(SpinVideoLibrary PROPERTIES IMPORTED_LOCATION "${SPINNAKER_ROOT}/lib/libSpinVideo.dylib")
else()
    set_target_properties(SpinnakerLibrary PROPERTIES IMPORTED_LOCATION "${SPINNAKER_ROOT}/bin${BITS}/libSpinnaker.dll")
    set_target_properties(SpinVideoLibrary PROPERTIES IMPORTED_LOCATION "${SPINNAKER_ROOT}/bin${BITS}/libSpinVideo.dll")
endif()

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(BirdWatcher ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(BirdWatcher SpinnakerLibrary SpinVideoLibrary)