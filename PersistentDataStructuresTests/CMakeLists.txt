cmake_minimum_required(VERSION 3.14)

project(PersistentDataStructures_test)

enable_testing()

set(SOURCE_EXE "PersistentVectorTests.cpp" "PersistentMapTests.cpp"
				"PersistentListTests.cpp"
				)

add_executable(PersistentDataStructures_test ${SOURCE_EXE})

message(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/../PersistentDataStructures/include")

target_link_libraries(
  PersistentDataStructures_test
  gtest_main
  PersistDataStructs
)

include(GoogleTest)
gtest_discover_tests(PersistentDataStructures_test)
