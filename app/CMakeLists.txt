add_executable (app "app.cpp")
target_link_libraries(app "strassen3")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET app PROPERTY CXX_STANDARD 20)
endif()
