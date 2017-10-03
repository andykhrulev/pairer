cmake_minimum_required(VERSION 3.6)

message(STATUS "adding Thrift server...")

set(THRIFT_SRV_SRC
    ${THRIFT_SRV_SRC_DIR}/LoggerServer.cpp 
    ${THRIFT_SRV_SRC_DIR}/Logger.cpp
    ${THRIFT_SRV_SRC_DIR}/RadiusPairLogger_constants.cpp
    ${THRIFT_SRV_SRC_DIR}/RadiusPairLogger_types.cpp
)
add_executable(ThriftLoggingServer ${THRIFT_SRV_SRC})

target_compile_definitions(ThriftLoggingServer PRIVATE DEBUG_ENABLED=1)
target_link_libraries(ThriftLoggingServer libthrift.dylib ${Boost_LIBRARIES})
