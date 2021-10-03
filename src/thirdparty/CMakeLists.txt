set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")

FetchContent_Declare(json
  GIT_REPOSITORY "https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent.git"
  GIT_TAG "v3.10.2"
)
FetchContent_MakeAvailable(json)

# header directories
add_subdirectory(libuiohook)
add_subdirectory(HTTPRequest)
if(UNIX)
add_subdirectory(frozen)
endif()
