#include <hub/Hub.h>
#include <hub/JsonRequest.h>
#include <core/Log.h>
#include <core/JsonHelper.h>
#include <utils/Utils.h>

using namespace core;
using namespace core::js_util;
using namespace utils;

namespace hub {

enum URLCode {
    Default = 200,
};

void Hub::setKey(std::string const& key)
{

}

#define INTER 10000
void Hub::registerObserver(server::HttpServer& httpServer)
{
    httpServer.registerObserver("/faucet", [this] (std::string, std::string body, server::URLRequestCallback urlRC) {
        std::string ret;
        Json::Reader reader(Json::Features::strictMode());
        Json::Value root;
        if (reader.parse(body, root)) {
            bool allowed = false;
            {
                Guard l{x_timestamp};
                int64_t timestamp = currentTimestamp();
                if (timestamp > m_timestamp + INTER) {
                    allowed = true;
                    m_timestamp = timestamp;
                }
            }

            if (!allowed) {
                Json::Value jRet;
                jRet["status"] = 1;
                jRet["txHash"] = "";
                jRet["value"] = 0;
                ret = jRet.toStyledString();
            } else {
                std::string addressString = root["address"].asString();
                Address address(addressString);
                uint64_t value = (m_random() % 100000);
                value = value * 1000000;

                CINFO << "Send to address:" << address << "\tvalue:" << value;
                transfer(address, value);
                Json::Value jRet;
                jRet["status"] = 0;
                jRet["txHash"] = toJS(m_tx.getHash());
                jRet["value"] = m_tx.getValue();
                ret = jRet.toStyledString();
            }
        } else {
            ret = "Parse body failed, invalid format.\n";
        }

        urlRC(URLCode::Default, ret);
    });
}

void Hub::send(std::string const& cmd, Json::Value const& value, std::function<void(std::string const&)> callback)
{
    beast::http::request<beast::http::string_body> request;
    boost::asio::connect(m_socket,
        m_resolver.resolve(boost::asio::ip::tcp::resolver::query{m_rpcHost, m_rpcPort}));
    request.method(http::verb::post);
    request.version = m_version;
    request.set(http::field::host, m_rpcHost);
    request.set(http::field::user_agent, "Beast");
    request.set(http::field::content_type, "application/json");
    request.target(cmd);
    request.body = value.toStyledString();
    request.set(http::field::content_length, std::to_string(request.body.size()));
    beast::http::write(m_socket, request);
    if (callback) {
        beast::http::response<http::string_body> response;
        beast::flat_buffer buffer;
        beast::http::read(m_socket, buffer, response);
        callback(response.body);
    }

    boost::system::error_code ec;
    m_socket.shutdown(tcp::socket::shutdown_both,ec);
    if(ec && ec != boost::system::errc::not_connected)
        throw boost::system::system_error{ec};
}

void Hub::broadcast(core::Transaction& transaction)
{
    send("/push_transaction", toJson(transaction), [this] (std::string const& buffer) {
        std::cout << buffer << std::endl;
    });
}

void Hub::transfer(Address const& recipient, uint64_t value)
{
    try {
        Json::Value requestTransfer = toRequestTransfer(m_key.getAddress(), recipient, value);
        send("/create_transaction", requestTransfer, [this] (std::string const& buffer) {
            Json::Reader reader(Json::Features::strictMode());
            Json::Value root;
            if(reader.parse(buffer, root)) {
                Transaction tx = toTransaction(root);
                tx.sign(m_key.getSecret());
                m_tx = tx;
                broadcast(tx);
            } else {
                std::cout << "Parse json error\n";
            }
        });
    } catch (Exception& e) {
        CERROR << e.what();
    } catch(std::exception& e) {
        CERROR << e.what();
    }
}
}
