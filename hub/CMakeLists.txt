SET(hub_source
    JsonRequest.cpp
    Hub.cpp)

ADD_LIBRARY(hub ${hub_source})
TARGET_LINK_LIBRARIES(hub
    utils crypto trie jsoncpp_lib_static Beast::Beast ${GSE_BOOST_LIBS})