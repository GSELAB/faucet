#include <string>

#include <server/WebSocket.h>
#include <core/Log.h>
#include <chain/Types.h>

using namespace std;
using namespace core;
using namespace chain;

namespace server {

WebSocket::WebSocket(unsigned short listenPort, chain::ChainID chainID): m_listenPort(listenPort), m_chainID(chainID)
{
    if (m_chainID == GSE_UNKNOWN_NETWORK) {
        throw RPCException("Unknown Chain ID:" + toString(GSE_UNKNOWN_NETWORK));
    }
}

bool WebSocket::init()
{
    try {
        m_server.clear_access_channels(websocketpp::log::alevel::all);
        m_server.clear_error_channels(websocketpp::log::alevel::all);
        m_server.init_asio();
        m_server.set_reuse_addr(true);
        m_server.set_message_handler([&](ConnectHDL hdl, MessagePtr msg) {
            onMessage(&m_server, hdl, msg);
        });

        m_server.set_http_handler([&](ConnectHDL hdl) {
            onHttp(&m_server, hdl);
        });

        m_server.listen(m_listenPort);
        m_server.start_accept();

        m_initialSuccess = true;
    } catch(websocketpp::exception const& e) {
        CERROR << "RPC server init failed, " << e.what();
        throw e;
    }

    return true;
}

void WebSocket::startService()
{
    try {
        if (m_initialSuccess) {
            m_server.run();
        }
    } catch(websocketpp::exception const& e) {
        CERROR << "RPC server run failed, " << e.what();
    }
}

bool WebSocket::shutdown()
{
    return true;
}

void WebSocket::onMessage(WebsocketServer* server, ConnectHDL hdl, MessagePtr msg)
{
    server->send(hdl, msg->get_payload(), msg->get_opcode());
}

void WebSocket::onHttp(WebsocketServer* server, ConnectHDL hdl)
{
    try {
        WebsocketServer::connection_ptr con = server->get_con_from_hdl(hdl);
        bool secure = con->get_uri()->get_secure();
        bi::tcp::endpoint const ep = con->get_socket().lowest_layer().local_endpoint();

        Request const& req = con->get_request();
        std::string const& host = req.get_header("Host");
        if (host.empty()) {
            CERROR << "Host is empty";
            con->set_status(websocketpp::http::status_code::bad_request);
            return;
        }

        con->append_header("Content-type", "application/json");
        string const body = con->get_request_body();
        string const resource = con->get_resource();

        auto handler_itr = m_urlHandlers.find(resource);
        if (handler_itr != m_urlHandlers.end()) {
            handler_itr->second(resource, body, [con](int code, std::string body) {
                con->set_body(body);
                con->set_status(websocketpp::http::status_code::value(code));
                // con->send_http_response();
            });
        } else {
            CERROR << "Not found " << resource;
            con->set_body("Undefined " + resource + "\n");
            con->set_status(websocketpp::http::status_code::not_found);
        }

    } catch(Exception& e) {
        CERROR << "Error occur onHttp - " << e.what();
    } catch (websocketpp::exception& e) {
        CERROR << "Error occur onHttp - " << e.what();
    } catch (std::exception& e) {
        CERROR << "Error occur onHttp - " << e.what();
    }
}
}