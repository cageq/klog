cmake_minimum_required (VERSION 3.1.0)
project (klog)


include_directories(".")
include_directories("include")
include_directories("opt")
 

include_directories("include")
link_directories("lib") 
link_directories("/usr/local/lib") 

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(APPLE)
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wc++11-extensions -std=c++11 -stdlib=libc++  -g  -D NDEBUG")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wc++17-extensions -Wall -std=c++17 -stdlib=libc++  -Wno-unused-function -g  ")
  include_directories("/usr/local/Cellar/openssl/1.0.2t/include")
  link_directories("/usr/local/Cellar/openssl/1.0.2t/lib") 
  #ADD_DEFINITIONS(-fcoroutines-ts --stdlib=libc++ -Xclang -fconcepts-ts)
elseif(UNIX)
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11 -pthread  -D NDEBUG")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -pthread  -g  -O2 -Wno-unused-function ")
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pthread -pg -g -DNDEBUG -Wunused-value ")
elseif(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17  -D NDEBUG")
endif()


add_subdirectory(opt/fmt EXCLUDE_FROM_ALL)

add_executable(logger samples/logger.cpp  )
target_link_libraries(logger fmt)

