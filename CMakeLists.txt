cmake_minimum_required(VERSION 3.6)
project(Pairer)

set(CMAKE_CXX_STANDARD 11)

message("SRC Include: " ${PROJECT_SOURCE_DIR})
set(CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR} /opt/local/include /opt/local/lib)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(PAIRER_SRC_DIR     "${CMAKE_SOURCE_DIR}/projects/Pairer")
set(SELFTEST_SRC_DIR   "${CMAKE_SOURCE_DIR}/projects/PairerSelfTest")
set(THRIFT_SRV_SRC_DIR "${CMAKE_SOURCE_DIR}/projects/ThriftServer")

set(LIBFREERADIUS_INCL "$ENV{LIBFREERADIUS_INCL}")

message("Libfree radius include files: " ${LIBFREERADIUS_INCL})

include_directories(${PAIRER_SRC_DIR} ${THRIFT_SRV_SRC_DIR} ${LIBFREERADIUS_INCL})

message(STATUS "start running cmake...")

find_package(Boost 1.65.1 COMPONENTS serialization program_options REQUIRED)

if(Boost_FOUND)

    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories("/opt/local/include" ${CATCH_INCLUDE_DIR} ${PROJECT_SOURCE_DIR})
link_directories("/opt/local/lib" "/usr/local/lib")

include(Pairer)
include(ThriftServer)
include(PairerSelfTest)
include(AddCatch)


add_test(NAME PairerSelfTest COMMAND PairerSelfTest )
