cmake_minimum_required(VERSION 3.12)
file(GLOB SOURCES *.cpp)

list(REMOVE_ITEM SOURCES "./test.cpp")
# file(GLOB HEADERS ./include/*.h)

# c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_BUILD_TYPE "Release")
project(main)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE user32 gdi32 kernel32 gdiplus)
# target_include_directories(${PROJECT_NAME} ./include)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")