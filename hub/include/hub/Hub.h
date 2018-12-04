#include <string>
#include <random>
#include <time.h>

#include <beast/http.hpp>
#include <beast/core.hpp>
#include <beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <crypto/GKey.h>
#include <server/HttpServer.h>

using tcp = boost::asio::ip::tcp;
namespace http = beast::http;

namespace hub {

class Hub {
public:
    Hub(std::string const& host, std::string const& port, int version = 11):
        m_rpcHost(host), m_rpcPort(port), m_version(version) {
        m_random.seed(time(0));
    }

    virtual ~Hub() {}

    void registerObserver(server::HttpServer& httpServer);

    void transfer(std::string const& recipient, uint64_t value);

protected:
    void send(std::string const& cmd, Json::Value const& value, std::function<void(std::string const&)> callback);

    void broadcast(core::Transaction& transaction);

private:
    std::string m_rpcHost;
    std::string m_rpcPort;
    int m_version;

    boost::asio::io_service m_ios;
    boost::asio::ip::tcp::resolver m_resolver{m_ios};
    boost::asio::ip::tcp::socket m_socket{m_ios};

    std::default_random_engine m_random;
    crypto::GKey m_key;
};
}