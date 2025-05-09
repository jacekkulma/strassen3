add_library(strassen3 "matrix.h")
target_include_directories(strassen3 PUBLIC ".")
set_target_properties(strassen3 PROPERTIES LINKER_LANGUAGE CXX)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET strassen3 PROPERTY CXX_STANDARD 20)
endif()
