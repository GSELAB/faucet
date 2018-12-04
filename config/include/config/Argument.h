#include <string>

namespace config {

struct Argument {
    std::string m_localIP = "0.0.0.0";

    unsigned short m_localPort = 40404;

    std::string m_rpcIP = "0.0.0.0";

    std::string m_rpcPort = "50505";
};

extern Argument ARG;
}
