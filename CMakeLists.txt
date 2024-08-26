cmake_minimum_required(VERSION 3.10)
project(wxThemer)

# Set wxWidgets root and lib directory paths
set(wxWidgets_ROOT_DIR "c:/cpp/cppLibraries/wxWidgets_static")
set(wxWidgets_CONFIGURATION "mswu")

set(wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/lib/gcc_x64_lib")

# Set wxWidgets include directories
set(wxWidgets_INCLUDE_DIRS 
    "${wxWidgets_ROOT_DIR}/include" 
    "${wxWidgets_LIB_DIR}/${wxWidgets_CONFIGURATION}"
)

# Add include directories
include_directories(${wxWidgets_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include ${wxWidgets_LIB_DIR})


# Set the source files for the wxThemer project
set(SOURCE_FILES
    src/wxThemer.cpp
    src/wxThemerCheckBox.cpp
)

# Find wxWidgets package
find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net)

# Output the version of wxWidgets
message(STATUS "wxWidgets version: ${wxWidgets_VERSION}")

# Include wxWidgets
include(${wxWidgets_USE_FILE})

# Create a library target for wxThemer
add_library(wxThemer STATIC ${SOURCE_FILES})

# Link wxWidgets libraries
target_link_libraries(wxThemer ${wxWidgets_LIBRARIES} dwmapi)

# Specify the C++ standard
set_target_properties(wxThemer PROPERTIES CXX_STANDARD 17)
