cmake_minimum_required(VERSION 3.0.0)
project(dessinGrapheCmake VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

set(OGDF_CONFIG_SEARCH_PATH CACHE PATH "/home/uha/Documents/OGDF")

find_package(OpenGL REQUIRED)
find_package(OGDF CONFIG PATHS "/home/uha/Documents/OGDF")
find_package(nlohmann_json REQUIRED CONFIG PATHS "/home/uha/Documents/json-develop")

add_executable(dessinGrapheCmake 
    src/main.cpp
)

target_include_directories(dessinGrapheCmake PRIVATE "/home/uha/Documents/OGDF/include")
target_include_directories(dessinGrapheCmake PRIVATE "/home/uha/Documents/json-develop/include")

target_link_libraries(dessinGrapheCmake 
    glfw
    OpenGL::GL
    ${CMAKE_DL_LIBS}
    OGDF
    nlohmann_json::nlohmann_json
    gomp
)