cmake_minimum_required (VERSION 3.8)

project ("Signature")

add_executable (Signature "Main.cpp" "MD5.h" "MD5.cpp" "Config.h" "Config.cpp" "Calculator.h" "Calculator.cpp" "Exceptions.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Signature PROPERTY CXX_STANDARD 20)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "filesig")