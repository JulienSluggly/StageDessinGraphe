cmake_minimum_required(VERSION 3.5)
project(dessinGrapheReel VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

set (CMAKE_CXX_STANDARD 17) # Impossible d'utiliser plus récent avec OGDF

list(APPEND CMAKE_MODULE_PATH "/home/uha/Documents/DessinGrapheReel/src/cmake")

find_package(OpenGL REQUIRED)
find_package(OGDF CONFIG PATHS "/home/uha/Documents/OGDF")
find_package(nlohmann_json REQUIRED CONFIG PATHS "/home/uha/Documents/json-develop")

add_executable(dessinGrapheReel 
    src/main.cpp
    src/grapheRecuitSimule.cpp
    src/grapheJsonIO.cpp
    src/grapheDebug.cpp
    src/grapheScore.cpp
    src/grapheUtils.cpp
    src/graphePlacement.cpp
    src/intersection.cpp
    src/utilitaire.cpp
    src/ogdfFunctions.cpp
    src/personnel.cpp
    src/arg.cpp
)

target_include_directories(dessinGrapheReel PRIVATE "/home/uha/Documents/OGDF/include")
target_include_directories(dessinGrapheReel PRIVATE "/home/uha/Documents/json-develop/include")

target_link_libraries(dessinGrapheReel 
    glfw
    OpenGL::GL
    ${CMAKE_DL_LIBS}
    OGDF
    nlohmann_json::nlohmann_json
    stdc++fs
)