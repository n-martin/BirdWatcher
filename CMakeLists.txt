cmake_minimum_required(VERSION 3.0.0)
project(BirdWatcher VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Get 32 or 64bit info
math(EXPR BITS "8*${CMAKE_SIZEOF_VOID_P}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include(CTest)
enable_testing()

# INCLUDES
include_directories(include)
if(UNIX)
    include_directories(/usr/local/include/spinnaker)
else()
    include_directories($ENV{SPIN_INSTALL}/include)
endif()

# LIBRARIES
add_library(SpinnakerLibrary SHARED IMPORTED)
add_library(SpinVideoLibrary SHARED IMPORTED)
if(UNIX)
    set_target_properties(SpinnakerLibrary PROPERTIES IMPORTED_LOCATION "/usr/local/lib/libSpinnaker.dylib")
    set_target_properties(SpinVideoLibrary PROPERTIES IMPORTED_LOCATION "/usr/local/lib/libSpinVideo.dylib")
else()
    set_target_properties(SpinnakerLibrary PROPERTIES IMPORTED_LOCATION "$ENV{SPIN_INSTALL}/bin${BITS}/libSpinnaker.dll")
    set_target_properties(SpinVideoLibrary PROPERTIES IMPORTED_LOCATION "$ENV{SPIN_INSTALL}/bin${BITS}/libSpinVideo.dll")
endif()

file(GLOB SOURCES "src/*.cpp")
add_executable(BirdWatcher ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(BirdWatcher SpinnakerLibrary SpinVideoLibrary)