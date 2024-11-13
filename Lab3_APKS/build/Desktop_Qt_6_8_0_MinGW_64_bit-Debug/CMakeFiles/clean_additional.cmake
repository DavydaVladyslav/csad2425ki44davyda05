# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Lab3_APKS_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Lab3_APKS_autogen.dir\\ParseCache.txt"
  "Lab3_APKS_autogen"
  )
endif()
