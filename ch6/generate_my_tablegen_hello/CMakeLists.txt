cmake_minimum_required(VERSION 3.16)
project(tablegen-hello CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Where we'll put generated artifacts.
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

# TableGen input.
set(TD_FILE ${CMAKE_SOURCE_DIR}/tablegen/Hello.td)
set(GENERATED_JSON ${GENERATED_DIR}/hello.json)

# Try to locate llvm-tblgen in PATH.
find_program(LLVM_TBLGEN_EXECUTABLE NAMES llvm-tblgen REQUIRED)
message(STATUS "Using llvm-tblgen: ${LLVM_TBLGEN_EXECUTABLE}")

# Custom command: run llvm-tblgen to emit JSON.
add_custom_command(
  OUTPUT ${GENERATED_JSON}
  COMMAND ${LLVM_TBLGEN_EXECUTABLE} ${TD_FILE} -dump-json > ${GENERATED_JSON}
  DEPENDS ${TD_FILE}
  COMMENT "Generating JSON from TableGen: ${TD_FILE} -> ${GENERATED_JSON}"
  VERBATIM
)

# Create a phony target to make sure JSON is built before the executable runs.
add_custom_target(generate_tablegen_json ALL
  DEPENDS ${GENERATED_JSON}
)

# Our program.
add_executable(tablegen-hello src/main.cpp)
add_dependencies(tablegen-hello generate_tablegen_json)

# Tell the program where to find the generated file at runtime.
target_compile_definitions(tablegen-hello PRIVATE
  TBLGEN_JSON_PATH="$<TARGET_FILE_DIR:tablegen-hello>/../generated/hello.json"
)

# On Windows, the build dir layout is slightly different; also copy the file next to the binary.
add_custom_command(TARGET tablegen-hello POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GENERATED_JSON} $<TARGET_FILE_DIR:tablegen-hello>/hello.json
  COMMENT "Copying generated JSON next to the binary for convenience"
)
