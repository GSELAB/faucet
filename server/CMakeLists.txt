SET(server_source
    WebSocket.cpp
    HttpServer.cpp)

ADD_LIBRARY(server ${server_source})
TARGET_LINK_LIBRARIES(server
    utils ${GSE_BOOST_LIBS})