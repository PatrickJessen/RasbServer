#include <boost/asio.hpp>
#include <iostream>
#include "Server.h"


int main() {
    try
    {
        boost::asio::io_service service;
        Server server(service, 9999);
        service.run();
        //std::thread t([&service]() { service.run(); });
        //t.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}