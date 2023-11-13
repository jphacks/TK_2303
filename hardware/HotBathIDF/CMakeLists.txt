# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(ARDUINO_CMAKE_FILE ${CMAKE_SOURCE_DIR}/components/arduino/third_party/coreMQTT/mqttFilePaths.cmake)

if (EXISTS ${ARDUINO_CMAKE_FILE})
  include(${ARDUINO_CMAKE_FILE})
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32-peer)

