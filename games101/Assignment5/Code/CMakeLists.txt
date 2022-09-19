cmake_minimum_required(VERSION 3.10)
project(RayTracing)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

include_directories(.)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(test)

add_executable(RayTracing ./test/main.cpp ./src/Object.hpp ./src/Vector.hpp ./src/Sphere.hpp ./src/global.hpp ./src/Triangle.hpp ./src/Scene.cpp ./src/Scene.hpp ./src/Light.hpp ./src/Renderer.cpp)
target_compile_options(RayTracing PUBLIC -Wall -Wextra -pedantic -Wshadow -Wreturn-type -fsanitize=undefined)
target_compile_features(RayTracing PUBLIC cxx_std_17)
target_link_libraries(RayTracing PUBLIC -fsanitize=undefined)
