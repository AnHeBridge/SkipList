cmake_minimum_required(VERSION 2.8.8)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
include(common)

set(build_dir ${CMAKE_CURRENT_LIST_DIR}/build)

message(STATUS "BUILDDIR " ${build_dir})
message(STATUS "PROJECT_SOURCE_DIR" ${PROJECT_SOURCE_DIR})

enable_testing()
find_package(Threads REQUIRED)

if (gtest_build_samples)
	file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)
	# Test
	include(ExtProjectUtils)
	get_filename_component(DEPS_ROOT "${PROJECT_BINARY_DIR}/deps" ABSOLUTE)
	# Will download external CMakeable project from git repo, branch "master" and install it in $DEPS_ROOT
	# This also will create "googletest.git" target, which we'll use as dependency for our test project
	ExtProjectGit("https://github.com/google/googletest.git" "master" ${DEPS_ROOT} CMAKE_ARGS "${_OPT_CMAKE_ARGS}")

	# from list of files we'll create tests test_name.cpp -> test_name
	foreach(_test_file ${TEST_SRC_FILES})
		get_filename_component(_test_name ${_test_file} NAME_WE)
		add_executable(${_test_name} ${_test_file})
		add_dependencies(${_test_name} "googletest.git")
		target_link_libraries(${_test_name} gtest gtest_main ${CMAKE_THREAD_LIBS_INIT})
		add_test(${_test_name} ${_test_name})
		set_tests_properties(${_test_name} PROPERTIES TIMEOUT 5)
	endforeach()
endif()
