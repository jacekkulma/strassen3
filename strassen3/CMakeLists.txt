add_library(strassen3 INTERFACE)
target_include_directories(strassen3 INTERFACE .)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET strassen3 PROPERTY CXX_STANDARD 20)
endif()
