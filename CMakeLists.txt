cmake_minimum_required(VERSION 3.5)
project(dessinGrapheCmake VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

set (CMAKE_CXX_STANDARD 17) # Impossible d'utiliser plus récent avec OGDF

list(APPEND CMAKE_MODULE_PATH "/home/uha/Documents/DessinGrapheCmake/src/cmake")

find_package(OpenGL REQUIRED)
find_package(OGDF CONFIG PATHS "/home/uha/Documents/OGDF")
find_package(nlohmann_json REQUIRED CONFIG PATHS "/home/uha/Documents/json-develop")
find_package(OpenMP)
find_package(Ceres REQUIRED)
find_package(Gperftools REQUIRED)

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
    src/ogdfFunctions.cpp
    src/carte.cpp
    src/triangulation.cpp
    src/personnel.cpp
    src/stressMaj.cpp
    src/pivotMDS.cpp
    src/fmmm.cpp
    src/solver.cpp
)

target_include_directories(dessinGrapheCmake PRIVATE "/home/uha/Documents/OGDF/include")
target_include_directories(dessinGrapheCmake PRIVATE "/home/uha/Documents/json-develop/include")
target_include_directories(dessinGrapheCmake PRIVATE "/home/uha/Documents/pugixml-1.13/src")

target_include_directories(dessinGrapheCmake PRIVATE "/home/uha/Documents/CPLEX_Studio2211/concert/include")
target_link_directories(dessinGrapheCmake PRIVATE "/home/uha/Documents/CPLEX_Studio2211/concert/lib/x86-64_linux/static_pic")

#target_include_directories(dessinGrapheCmake PRIVATE "/home/uha/Documents/CPLEX_Studio2211/cpoptimiser/include")
#target_link_directories(dessinGrapheCmake PRIVATE "/home/uha/Documents/CPLEX_Studio2211/cpoptimiser/lib/x86-64_linux/static_pic")

target_include_directories(dessinGrapheCmake PRIVATE "/home/uha/Documents/CPLEX_Studio2211/cplex/include")
target_link_directories(dessinGrapheCmake PRIVATE "/home/uha/Documents/CPLEX_Studio2211/cplex/lib/x86-64_linux/static_pic")

target_include_directories(dessinGrapheCmake PRIVATE ${GPERFTOOLS_INCLUDE_DIRS})

target_link_libraries(dessinGrapheCmake 
    glfw
    OpenGL::GL
    ${CMAKE_DL_LIBS}
    OGDF
    nlohmann_json::nlohmann_json
    OpenMP::OpenMP_CXX
    gomp
    stdc++fs
)

target_link_libraries(dessinGrapheCmake # Library used by cplex
    cplex
    concert
    ilocplex
    pthread
    m
    dl
    Ceres::ceres
)

target_link_libraries(dessinGrapheCmake
    ${GPERFTOOLS_PROFILER}
)