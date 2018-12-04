#include <iostream>
#include <chrono>
#include <thread>

#include <config/Argument.h>
#include <hub/Hub.h>

using namespace config;
using namespace hub;
using namespace server;

void initArgument()
{
    ARG.m_localIP = "0.0.0.0";
    ARG.m_localPort = 40404;

    ARG.m_rpcIP = "132.232.52.156";
    ARG.m_rpcPort = "50505";
}

bool isRunning = true;
void signalHandler(int signal)
{
    std::string reason;
    switch (signal) {
        case SIGABRT:
            reason = "SIGABRT";
            break;
        case SIGTERM:
            reason = "SIGTERM";
            break;
        case SIGINT:
            reason = "SIGINT";
            break;
        default:
            reason = "UNKNOWN";
            break;
    }

    std::cout << "exit by signal " << reason << "\n";
    isRunning = false;
}

int main(int argc, char** argv)
{
    signal(SIGABRT, &signalHandler);
    signal(SIGTERM, &signalHandler);
    signal(SIGINT, &signalHandler);
    initArgument();
    Hub hub(ARG.m_rpcIP, ARG.m_rpcPort);
    HttpServer server(ARG.m_localPort, 256);

    hub.registerObserver(server);
    server.start();

    while (isRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    server.stop();
    return 0;
}