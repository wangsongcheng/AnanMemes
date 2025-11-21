cmake_minimum_required (VERSION 3.21)
project (demo)
if(MSVC)
    add_compile_options(/utf-8)
endif()
set(CMAKE_BUILD_TYPE Debug)
if(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
endif()
MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
# LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)

set(DEMO_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
message(${DEMO_INCLUDE_DIRS})
set(DEMO_LIB_DIRS ${PROJECT_SOURCE_DIR}/lib)
message(${DEMO_LIB_DIRS})
#可以把所有资源放到一个统一的文件夹内，然后直接复制那个文件夹
file(COPY ${PROJECT_SOURCE_DIR}/resource DESTINATION ${PROJECT_SOURCE_DIR}/build/)

file(GLOB DEMO_CPP ${PROJECT_SOURCE_DIR}/*.cpp)

add_executable(demo ${DEMO_CPP})
# target_link_libraries(demo PRIVATE)

target_include_directories(demo PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
target_link_directories(demo PRIVATE
    ${PROJECT_SOURCE_DIR}/lib
    "$ENV{VULKAN_SDK}/Lib"
)