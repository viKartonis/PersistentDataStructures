cmake_minimum_required (VERSION 3.8)


set(HEADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(HEADER_LIB "${HEADER_PATH}/PersistentVector.h"
				"${HEADER_PATH}/PersistentMap.h"
				"${HEADER_PATH}/PersistentList.h"
				"${HEADER_PATH}/Utils.h")
set(SOURCE_LIB "${SOURCE_PATH}/Utils.cpp")

add_library(PersistDataStructs STATIC ${HEADER_LIB} ${SOURCE_LIB})
