cmake_minimum_required(VERSION 3.20)
project(rocky)

if(WIN32)
  set(CMAKE_PREFIX_PATH "$ENV{CONDA_PREFIX}/Library")
  set(LLVM_USE_SHARED_LIBS ON)
else()
  set(CMAKE_PREFIX_PATH "$ENV{CONDA_PREFIX}")
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
  set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
endif()

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
link_directories(${LLVM_LIBRARY_DIRS})

add_executable(rocky src/main.c src/jit.c)
target_include_directories(rocky PUBLIC include)
if(WIN32)
  target_link_libraries(rocky LLVM-C)

  add_custom_command(TARGET rocky POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${PROJECT_SOURCE_DIR}/.pixi/envs/default/Library/bin/LLVM-C.dll"
          "${CMAKE_BINARY_DIR}/LLVM-C.dll")
  add_custom_command(TARGET rocky POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${PROJECT_SOURCE_DIR}/.pixi/envs/default/Library/bin/zlib.dll"
          "${CMAKE_BINARY_DIR}/zlib.dll")
  add_custom_command(TARGET rocky POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${PROJECT_SOURCE_DIR}/.pixi/envs/default/Library/bin/zstd.dll"
          "${CMAKE_BINARY_DIR}/zstd.dll")

else()
  llvm_map_components_to_libnames(llvm_libs core support native target mc asmprinter orcjit executionengine)
  target_link_libraries(rocky ${llvm_libs})
endif()
