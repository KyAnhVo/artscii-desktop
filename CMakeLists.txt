cmake_minimum_required(VERSION 3.16)
project(artscii)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find needed packages
find_package(PNG REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# QT automations
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include Directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/core)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "core/*.cpp")
file(GLOB_RECURSE UI_FILES "ui/*.ui")
file(GLOB_RECURSE QRC_FILES "resources/*.qrc")

add_executable(artscii-desktop 
    ${SOURCES}
    ${UI_FILES}
    ${QRC_FILES}
)

target_link_libraries(artscii-desktop PNG::PNG Qt6::Widgets)

