# CMakeList.txt : CMake project for App, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)

# Add source to this project's executable.
add_executable(tutorial-000-helloworld
  "main.cpp"
)

target_compile_features(tutorial-000-helloworld PRIVATE cxx_std_20)

target_include_directories(tutorial-000-helloworld PUBLIC
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/external/libunifex/include
  ${CMAKE_BINARY_DIR}/external/libunifex/include
)

target_link_libraries(tutorial-000-helloworld PUBLIC unifex)
