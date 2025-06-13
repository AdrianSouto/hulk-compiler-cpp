cmake_minimum_required(VERSION 3.10)
project(HulkCompiler)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(LLVM REQUIRED CONFIG)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Include LLVM headers
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Include project headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Generate parser and lexer
BISON_TARGET(Parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
             DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp)
FLEX_TARGET(Lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

# Include generated headers directory
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Collect all source files (excluding main files)
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
)

# Add executable
add_executable(FlexBisonProject 
    ${SOURCES}
    "main.cpp"
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

# Link LLVM libraries
llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(FlexBisonProject ${llvm_libs})

# Add a simple executable for testing (if main_simple.cpp exists)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/main_simple.cpp)
    add_executable(hulk_simple
        main_simple.cpp
        ${SOURCES}
        ${BISON_Parser_OUTPUTS}
        ${FLEX_Lexer_OUTPUTS}
    )
    target_link_libraries(hulk_simple ${llvm_libs})
endif()

# Copy test files to build directory
file(GLOB TEST_FILES "*.hulk")
foreach(TEST_FILE ${TEST_FILES})
    configure_file(${TEST_FILE} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()

# Add custom target to run tests
add_custom_target(test
    COMMAND FlexBisonProject test_for_loop.hulk
    DEPENDS FlexBisonProject
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running for loop test"
)

# Print build information
message(STATUS "LLVM Version: ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM Include dirs: ${LLVM_INCLUDE_DIRS}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")