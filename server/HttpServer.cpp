#include <server/HttpServer.h>
#include <core/Log.h>
#include <config/Argument.h>

namespace server {

void HttpServer::start()
{
    m_server.init();
    m_server.startService();
}

void HttpServer::stop()
{
    m_server.shutdown();
}

void HttpServer::registerObserver(std::string const& url, URLHandler const& handler)
{
    m_server.addHandler(url, handler);
}
}