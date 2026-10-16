cmake_minimum_required(VERSION 3.25)
project(openfhe_examples CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # IntelliSense
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Debug OpenFHE built by ./setup_openfhe.sh
set(OPENFHE_INSTALL_DIR "${CMAKE_SOURCE_DIR}/../openfhe-install" CACHE PATH "OpenFHE install prefix")
list(PREPEND CMAKE_PREFIX_PATH "${OPENFHE_INSTALL_DIR}")
find_package(OpenFHE CONFIG REQUIRED)

# OpenFHE_CXX_FLAGS contains -O3; the debug flags come after it, so -O0 wins
set(CMAKE_CXX_FLAGS "${OpenFHE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS "${OpenFHE_EXE_LINKER_FLAGS}")
set(CMAKE_BUILD_RPATH "${OpenFHE_LIBDIR}")   # run without LD/DYLD_LIBRARY_PATH

include_directories(
    ${OpenFHE_INCLUDE}
    ${OpenFHE_INCLUDE}/third-party/include
    ${OpenFHE_INCLUDE}/core
    ${OpenFHE_INCLUDE}/pke
    ${OpenFHE_INCLUDE}/binfhe)
link_directories(${OpenFHE_LIBDIR})

foreach(name ckks ckks_fbt bfv bgv)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE ${OpenFHE_SHARED_LIBRARIES})
endforeach()
