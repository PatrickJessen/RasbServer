#include <boost/asio.hpp>
#include <iostream>
#include "Server.h"

//using namespace boost::asio;
//using namespace boost::asio::ip;
//
//// Driver program for receiving data from buffer
//std::string getData(tcp::socket& socket)
//{
//    streambuf buf;
//    read_until(socket, buf, "\n");
//    std::string data = buffer_cast<const char*>(buf.data());
//    return data;
//}
//
//// Driver program to send data
//void sendData(tcp::socket& socket, const std::string& message)
//{
//    write(socket,
//        buffer(message + "\n"));
//}
//
//int main(int argc, char* argv[])
//{
//    io_service io_service;
//
//    // Listening for any new incomming connection
//    // at port 9999 with IPv4 protocol
//    tcp::acceptor acceptor_server(
//        io_service,
//        tcp::endpoint(tcp::v4(), 9999));
//
//    // Creating socket object
//    tcp::socket server_socket(io_service);
//
//    // waiting for connection
//    acceptor_server.accept(server_socket);
//
//    std::string reply;
//
//    while (true) {
//
//        getline(std::cin, reply);
//        sendData(server_socket, reply);
//
//    }
//    return 0;
//}

int main() {
    try
    {
        boost::asio::io_service io_service;

        using namespace std; // For atoi.
        server s(io_service, 9999);

        io_service.run();
        io_service.stop();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}