cmake_minimum_required(VERSION 3.20)
project(primecodec LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(primecodec_lib STATIC src/Codec.cpp)
target_include_directories(primecodec_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(primecodec src/main.cpp)
target_link_libraries(primecodec PRIVATE primecodec_lib)

# Place CLI binary in the project root for every build configuration.
set_target_properties(primecodec PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_SOURCE_DIR}"
)

include(CTest)
if(BUILD_TESTING)
    add_executable(primecodec_tests tests/smoke.cpp)
    target_link_libraries(primecodec_tests PRIVATE primecodec_lib)
    add_test(NAME codec_smoke COMMAND primecodec_tests)
endif()
