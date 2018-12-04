#pragma once

#include <server/WebSocket.h>

namespace server {

class HttpServer {
public:
    HttpServer(unsigned short listenPort, chain::ChainID chainID = chain::GSE_UNKNOWN_NETWORK):
        m_server(listenPort, chainID) {}

    virtual ~HttpServer() {}

    void start();

    void stop();

    void registerObserver(std::string const& url, URLHandler const& handler);

private:
    WebSocket m_server;
};
}