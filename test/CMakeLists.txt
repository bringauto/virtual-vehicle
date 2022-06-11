CMAKE_MINIMUM_REQUIRED(VERSION 3.20)
PROJECT(VirtualVehicle)

SET(CMAKE_CXX_STANDARD 17)
#------------gtest-------------------#

FIND_PACKAGE(GTest REQUIRED)
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS} "${CMAKE_CURRENT_LIST_DIR}/include/")
FILE(GLOB_RECURSE source_test_files "${CMAKE_CURRENT_LIST_DIR}/source/*")
ADD_EXECUTABLE(vvutests ${source_test_files} ${CMAKE_CURRENT_LIST_DIR}/mainTests.cpp)
TARGET_INCLUDE_DIRECTORIES(vvutests PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include/")
TARGET_LINK_LIBRARIES(vvutests PUBLIC ${GTEST_LIBRARIES} pthread bringautolib)
INCLUDE(GoogleTest)
GTEST_DISCOVER_TESTS(vvutests)

ADD_CUSTOM_COMMAND(TARGET vvutests PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/maps ${CMAKE_CURRENT_BINARY_DIR}/maps)

FIND_PACKAGE(Python3 COMPONENTS Interpreter Development)

ADD_TEST(NAME smurf-linearRouteScenario COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/lib/StateSmurf/SmurfTestingScripts/CompareScenarios.py
		--scenario ${CMAKE_SOURCE_DIR}/test/smurf/linearRouteScenario/scenarios.json
		--executable ${CMAKE_BINARY_DIR}/VirtualVehicle
		--evaluator ${CMAKE_BINARY_DIR}/lib/StateSmurf/SmurfEvaluator/smurf_evaluator
		--output ${CMAKE_SOURCE_DIR}/test/smurf/linearRouteScenario/
		# --create-etalons
		)
ADD_TEST(NAME smurf-circularRouteScenario COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/lib/StateSmurf/SmurfTestingScripts/CompareScenarios.py
		--scenario ${CMAKE_SOURCE_DIR}/test/smurf/circularRouteScenario/scenarios.json
		--executable ${CMAKE_BINARY_DIR}/VirtualVehicle
		--evaluator ${CMAKE_BINARY_DIR}/lib/StateSmurf/SmurfEvaluator/smurf_evaluator
		--output ${CMAKE_SOURCE_DIR}/test/smurf/circularRouteScenario/
		# --create-etalons
		)

ADD_TEST(NAME smurf-duplicitStopScenario COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/lib/StateSmurf/SmurfTestingScripts/CompareScenarios.py
		--scenario ${CMAKE_SOURCE_DIR}/test/smurf/duplicitStopScenario/scenarios.json
		--executable ${CMAKE_BINARY_DIR}/VirtualVehicle
		--evaluator ${CMAKE_BINARY_DIR}/lib/StateSmurf/SmurfEvaluator/smurf_evaluator
		--output ${CMAKE_SOURCE_DIR}/test/smurf/duplicitStopScenario/
		# --create-etalons
		)