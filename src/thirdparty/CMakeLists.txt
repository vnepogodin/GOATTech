set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")

# header directories
add_subdirectory(libuiohook)
if(UNIX)
add_subdirectory(frozen)
endif()
