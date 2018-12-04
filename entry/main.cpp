#include <iostream>
#include <chrono>
#include <thread>

#include <config/Argument.h>
#include <hub/Hub.h>
#include <utils/Utils.h>
#include <core/Log.h>

using namespace config;
using namespace core;
using namespace hub;
using namespace server;
using namespace utils;

Hub* _hub;
HttpServer* _server;

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
    isRunning = false;
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

    if (_server) {
        _server->stop();
        delete _server;
        _server = NULL;
    }

    if (_hub) {
        delete _hub;
        _hub = NULL;
    }


    std::cout << "exit by signal " << reason << "\n";
}

bool shouldExit()
{
    return isRunning == false;
}

void doCheck()
{
    sleepMilliseconds(2000);
}

int main(int argc, char** argv)
{
    signal(SIGABRT, &signalHandler);
    signal(SIGTERM, &signalHandler);
    signal(SIGINT, &signalHandler);
    initArgument();
    Secret secret("");
    _hub = new Hub(secret, ARG.m_rpcIP, ARG.m_rpcPort);
    _server = new HttpServer(ARG.m_localPort, 256);
    _hub->registerObserver(*_server);
    _server->start();
    std::cout << "Faucet exit!\n";
    return 0;
}