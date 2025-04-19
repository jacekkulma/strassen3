# CMakeList.txt : CMake project for strassen3, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (strassen3 "strassen3.cpp" "strassen3.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET strassen3 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
