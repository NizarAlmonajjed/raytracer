cmake_minimum_required(VERSION 3.20)
project(raytracer VERSION 0.1.0 LANGUAGES CXX)

# ---- Language standard -------------------------------------------------------
# Require exactly C++17 and turn off compiler-specific extensions (e.g. gnu++17),
# so the code stays portable between clang (macOS) and gcc (Linux CI).
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to an optimised build with debug info if the user didn't pick one.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

# ---- Core library ------------------------------------------------------------
# All ray tracer logic lives in a library so both the executable and the tests
# can link against the same code. main.cpp stays tiny.
add_library(rtcore
  src/camera.cpp
  src/hittable.cpp
  src/hittable_list.cpp
  src/image.cpp
  src/material.cpp
  src/sphere.cpp
  src/util.cpp
)
target_include_directories(rtcore PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(rtcore PRIVATE -Wall -Wextra -Wpedantic)

# ---- Executable --------------------------------------------------------------
add_executable(raytracer src/main.cpp)
target_link_libraries(raytracer PRIVATE rtcore)
target_compile_options(raytracer PRIVATE -Wall -Wextra -Wpedantic)

# ---- Tests -------------------------------------------------------------------
option(RT_BUILD_TESTS "Build unit tests" ON)

if(RT_BUILD_TESTS)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  # Don't install GoogleTest alongside our project.
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()

  add_executable(rt_tests
    tests/camera_test.cpp
    tests/hittable_test.cpp
    tests/image_test.cpp
    tests/material_test.cpp
    tests/ray_test.cpp
    tests/sphere_test.cpp
    tests/util_test.cpp
    tests/vec3_test.cpp
  )
  target_link_libraries(rt_tests PRIVATE rtcore GTest::gtest_main)
  target_compile_options(rt_tests PRIVATE -Wall -Wextra -Wpedantic)

  include(GoogleTest)
  gtest_discover_tests(rt_tests)
endif()
