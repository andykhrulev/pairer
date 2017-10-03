cmake_minimum_required(VERSION 3.6)

message(STATUS "adding Pairer ...")

set(PAIRER_SRC
    ${PAIRER_SRC_DIR}/main.cpp
    ${PAIRER_SRC_DIR}/RadiusMessage.cpp
    ${PAIRER_SRC_DIR}/RadiusMessageReceiver.cpp
    ${PAIRER_SRC_DIR}/SimpleSyncReceivedMessageHandler.cpp
    ${PAIRER_SRC_DIR}/RadiusProxyMsgReceiver.cpp
    ${PAIRER_SRC_DIR}/DataCache.cpp
    ${PAIRER_SRC_DIR}/SimpleMapCache.cpp
    ${PAIRER_SRC_DIR}/ThriftResultLogger.cpp
    ${PAIRER_SRC_DIR}/AppStats.cpp
    ${THRIFT_SRV_SRC_DIR}/Logger.cpp
    ${THRIFT_SRV_SRC_DIR}/RadiusPairLogger_constants.cpp
    ${THRIFT_SRV_SRC_DIR}/RadiusPairLogger_types.cpp
)

add_executable(Pairer ${PAIRER_SRC})

add_dependencies(Pairer catch)

target_compile_definitions(Pairer PRIVATE TWOBLUECUBES_CATCH_HPP_INCLUDED=1 DEBUG_ENABLED=1 PROXY_MODE_ENABLED)

target_link_libraries(Pairer libfreeradius-radius.dylib libthrift.dylib ${Boost_LIBRARIES})

