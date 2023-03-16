cmake_minimum_required(VERSION 3.5)
project(dessinGrapheCmake VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

set (CMAKE_CXX_STANDARD 17)

set(OGDF_CONFIG_SEARCH_PATH CACHE PATH "/home/uha/Documents/OGDF")

find_package(OpenGL REQUIRED)
find_package(OGDF CONFIG PATHS "/home/uha/Documents/OGDF")
find_package(nlohmann_json REQUIRED CONFIG PATHS "/home/uha/Documents/json-develop")
find_package(OpenMP)

add_executable(dessinGrapheCmake 
    src/main.cpp
    src/grapheRecuitSimule.cpp
    src/grapheJsonIO.cpp
    src/grapheGenetique.cpp
    src/grapheDebug.cpp
    src/grapheScore.cpp
    src/grapheUtils.cpp
    src/graphePlacement.cpp
    src/geometrie.cpp
    src/intersection.cpp
    src/utilitaire.cpp
    src/carte.cpp
    src/triangulation.cpp
    src/personnel.cpp
    src/stressMaj.cpp
    src/pivotMDS.cpp
    src/fmmm.cpp
)

target_include_directories(dessinGrapheCmake PRIVATE "/home/uha/Documents/OGDF/include")
target_include_directories(dessinGrapheCmake PRIVATE "/home/uha/Documents/json-develop/include")
target_include_directories(dessinGrapheCmake PRIVATE "/home/uha/Documents/pugixml-1.13/src")

target_link_libraries(dessinGrapheCmake 
    glfw
    OpenGL::GL
    ${CMAKE_DL_LIBS}
    OGDF
    nlohmann_json::nlohmann_json
    OpenMP::OpenMP_CXX
    gomp
    -lstdc++fs
)