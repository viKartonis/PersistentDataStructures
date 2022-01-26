cmake_minimum_required (VERSION 3.14)

project ("PersistentDataStructures")
# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O2")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif(CMAKE_COMPILER_IS_GNUCXX)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W4 -EHsc")
endif(MSVC)

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)


add_subdirectory ("PersistentDataStructures")
add_subdirectory ("PersistentDataStructuresTests")
