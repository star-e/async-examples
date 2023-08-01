# CMakeList.txt : CMake project for App, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)

# Add source to this project's executable.
add_executable(tutorial-000-helloworld
  "main.cpp"
)

set_target_properties(tutorial-000-helloworld PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_compile_features(tutorial-000-helloworld PRIVATE cxx_std_20)
target_include_directories(tutorial-000-helloworld PRIVATE ../..)

target_include_directories(tutorial-000-helloworld PUBLIC
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/external/libunifex/include
  ${CMAKE_BINARY_DIR}/external/libunifex/include
)

target_link_libraries(tutorial-000-helloworld PUBLIC unifex)

add_custom_command(TARGET tutorial-000-helloworld POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/external/libunifex/source/${CMAKE_BUILD_TYPE}/unifex.dll ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/unifex.dll
  COMMENT "libuniex dll and pdb"
)
