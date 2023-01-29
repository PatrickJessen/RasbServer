#include <boost/asio.hpp>
#include <iostream>
#include "Server.h"


int main() {
    try
    {
        boost::asio::io_service io_service;

        Server s(io_service, 9999);

        io_service.run();
        io_service.stop();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}