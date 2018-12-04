#pragma once

#include <map>

#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <websocketpp/config/core.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

#include <core/JsonHelper.h>
#include <chain/Types.h>

namespace server {

#define TX_BUFFER_SIZE (200)
#define BLOCK_BUFFER_SIZE (20)
#define ACCOUNT_BUFFER_SIZE (20)

namespace ba = boost::asio;
namespace bi = boost::asio::ip;

using URLRequestCallback = std::function<void(int, std::string)>;
using URLHandler = std::function<void(std::string, std::string, URLRequestCallback)>;

struct AsioStubConfig: public websocketpp::config::asio {
    // pull default settings from our core config
    typedef websocketpp::config::asio core;
    typedef core::concurrency_type concurrency_type;
    typedef core::request_type request_type;
    typedef core::response_type response_type;
    typedef core::message_type message_type;
    typedef core::con_msg_manager_type con_msg_manager_type;
    typedef core::endpoint_msg_manager_type endpoint_msg_manager_type;
    typedef core::alog_type alog_type;
    typedef core::elog_type elog_type;
    typedef core::rng_type rng_type;
    typedef core::endpoint_base endpoint_base;

    static bool const enable_multithreading = true;

    struct transport_config : public core::transport_config {
        typedef core::concurrency_type concurrency_type;
        typedef core::elog_type elog_type;
        typedef core::alog_type alog_type;
        typedef core::request_type request_type;
        typedef core::response_type response_type;
        static bool const enable_multithreading = true;
    };

    typedef websocketpp::transport::asio::endpoint<transport_config>        transport_type;

    static const websocketpp::log::level elog_level =        websocketpp::log::elevel::all;
    static const websocketpp::log::level alog_level =        websocketpp::log::alevel::all;
};

typedef websocketpp::server<AsioStubConfig> WebsocketServer;
typedef websocketpp::connection_hdl ConnectHDL;
typedef WebsocketServer::message_ptr MessagePtr;
typedef websocketpp::http::parser::request Request;

class WebSocket {
public:
    WebSocket(unsigned short listenPort, chain::ChainID chainID = chain::GSE_UNKNOWN_NETWORK);

    ~WebSocket() {}

    bool init();

    void startService();

    bool shutdown();

    void onMessage(WebsocketServer* server, ConnectHDL hdl, MessagePtr msg);

    void onHttp(WebsocketServer* server, ConnectHDL hdl);

    void addHandler(std::string const& url, URLHandler const& handler) { m_urlHandlers.insert(std::make_pair(url, handler)); }

private:
    chain::ChainID m_chainID;

    unsigned short m_listenPort;

    WebsocketServer m_server;

    bool m_initialSuccess = false;

    std::map<std::string, URLHandler> m_urlHandlers;
};
}
