if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE "Release")
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  option (ENABLE_SAN "Build with for Google's sanitizers such as: asan, msan, tsan or lsan." on)

  set (CMAKE_EXPORT_COMPILE_COMMANDS 1)
  set (DEBUG 1)
  set (TINTO_DEVEL_MODE 1)
  unset (NDEBUG)
  mark_as_advanced (
    DEBUG
    TINTO_DEVEL_MODE
  )

  # if (CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "GNU")
  #   set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -std=c11 -pedantic")

  #   if (ENABLE_ASAN)
  #     string (TOLOWER ${ENABLE_SAN} ENABLE_SAN)
  #   else ()
  #     set (ENABLE_SAN "off")
  #   endif ()

  #   if (${ENABLE_SAN} STREQUAL "asan" OR ${ENABLE_SAN} STREQUAL "on")
  #     set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=asan")
  #   elseif (${ENABLE_SAN} STREQUAL "msan")
  #     set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=memory")
  #   elseif (${ENABLE_SAN} STREQUAL "tsan")
  #     set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=thread")
  #   elseif (${ENABLE_SAN} STREQUAL "lsan")
  #     # set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=leak")
  #     message (FATAL_ERROR "Lsan not enabled yet.")
  #   endif ()
  # endif ()
endif ()

set (SOURCES "")
