# CMakeList.txt : CMake project for App, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)

# Add source to this project's executable.
add_executable(tutorial-002-lazy
  "main.cpp"
)

set_target_properties(tutorial-002-lazy PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_compile_features(tutorial-002-lazy PRIVATE cxx_std_20)
target_include_directories(tutorial-002-lazy PRIVATE ../..)


target_include_directories(tutorial-002-lazy PUBLIC
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/external/libunifex/include
  ${CMAKE_BINARY_DIR}/external/libunifex/include
)

target_link_libraries(tutorial-002-lazy PUBLIC unifex)

add_custom_command(TARGET tutorial-002-lazy POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/external/libunifex/source/unifex.dll ${CMAKE_BINARY_DIR}/bin/unifex.dll
  COMMENT "libuniex dll and pdb"
)
