cmake_minimum_required(VERSION 3.6)

message(STATUS "adding PairerSelfTest ...")

enable_testing(true)  # Enables unit-testing.

set(SELFTEST_SRC
    ${SELFTEST_SRC_DIR}/test_main.cpp
    ${PAIRER_SRC_DIR}/RadiusMessage.cpp
    ${PAIRER_SRC_DIR}/DataCache.cpp
    ${PAIRER_SRC_DIR}/DataCache.cpp
    ${PAIRER_SRC_DIR}/TimerPool.cpp
    ${PAIRER_SRC_DIR}/SimpleMapCache.cpp
    ${PAIRER_SRC_DIR}/AppStats.cpp
)
add_executable(PairerSelfTest ${SELFTEST_SRC})
add_dependencies(PairerSelfTest catch)
target_compile_definitions(PairerSelfTest PRIVATE CATCH_UNITTEST_ENABLED DEBUG_ENABLED)
target_link_libraries(PairerSelfTest libfreeradius-radius.dylib ${Boost_LIBRARIES})

