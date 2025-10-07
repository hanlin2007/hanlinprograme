# src/CMakeLists.txt
set(MAIN_SOURCE main.cpp)

set(
NET_SOURCES
    net/Server.cpp
)

set(
HTTP_SOURCES
    http/Request.cpp
    http/Response.cpp
)

set(
HANDLER_SOURCES
    handlers/StaticHandler.cpp
    handlers/ProxyHandler.cpp
)

set(
ALL_SOURCES
    ${MAIN_SOURCE}
    ${NET_SOURCES}
    ${HTTP_SOURCES}
    ${HANDLER_SOURCES}
)

set(SOURCES ${ALL_SOURCES} PARENT_SCOPE)