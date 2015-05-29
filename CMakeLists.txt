cmake_minimum_required(VERSION 3.2)
project(mqttbroker)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(mqttbroker ${SOURCE_FILES} ServerApplication.cpp ServerApplication.h MqttListener.cpp MqttListener.h MqttServerConnection.cpp MqttServerConnection.h MqttServerConnectionFactory.cpp MqttServerConnectionFactory.h)

message(Project settings: ${CMAKE_BUILD_TYPE})

if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(LIB_ENDING d)
endif()

message(${LIB_ENDING})

target_link_libraries(mqttbroker PocoFoundation${LIB_ENDING} PocoUtil${LIB_ENDING} PocoNet${LIB_ENDING})
