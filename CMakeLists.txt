cmake_minimum_required(VERSION 3.5)
project(dessinGrapheOpti VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

set (CMAKE_CXX_STANDARD 17) # Impossible d'utiliser plus récent avec OGDF

find_package(OGDF CONFIG PATHS "/home/uha/Documents/OGDF")
find_package(nlohmann_json REQUIRED CONFIG PATHS "/home/uha/Documents/json-develop")

add_executable(dessinGrapheOpti 
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
    src/carte.cpp
    src/triangulation.cpp
    src/personnel.cpp
    src/stressMaj.cpp
    src/pivotMDS.cpp
)

target_include_directories(dessinGrapheOpti PRIVATE "/home/uha/Documents/OGDF/include")
target_include_directories(dessinGrapheOpti PRIVATE "/home/uha/Documents/json-develop/include")

target_link_libraries(dessinGrapheOpti 
    ${CMAKE_DL_LIBS}
    OGDF
    nlohmann_json::nlohmann_json
    stdc++fs
)