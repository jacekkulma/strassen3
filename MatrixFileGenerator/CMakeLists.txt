﻿include_directories("../external/args/")
add_executable (MatrixFileGenerator "matrixFileGenerator.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MatrixFileGenerator PROPERTY CXX_STANDARD 20)
endif()
