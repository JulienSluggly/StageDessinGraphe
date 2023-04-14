# FindGperftools.cmake

find_path(GPERFTOOLS_INCLUDE_DIRS gperftools/profiler.h)
find_library(GPERFTOOLS_PROFILER libprofiler.so)

if (GPERFTOOLS_INCLUDE_DIRS AND GPERFTOOLS_PROFILER)
  set(GPERFTOOLS_FOUND TRUE)
endif()

mark_as_advanced(GPERFTOOLS_INCLUDE_DIRS GPERFTOOLS_PROFILER)