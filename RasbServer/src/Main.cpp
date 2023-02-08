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

//#include <iostream>
//#include <boost/asio.hpp>
//#include <boost/array.hpp>
//
//using namespace std;
//using namespace boost::asio;
//using namespace boost::asio::ip;
//
//void handle_connection(tcp::socket socket) {
//    try {
//        for (;;) {
//            // Read data from the client
//            boost::array<char, 128> buf;
//            boost::system::error_code error;
//            size_t len = socket.read_some(buffer(buf), error);
//
//            // Check if any data was received
//            if (len == 0) {
//                cout << "No data received." << endl;
//                break;
//            }
//
//            // Check if the connection was closed by the client
//            if (error == error::eof)
//                break;
//            else if (error)
//                throw boost::system::system_error(error);
//
//            // Print the data received
//            cout << "Data received: ";
//            for (int i = 0; i < len; i++)
//                cout << buf[i];
//            cout << endl;
//
//            // Write the data back to the client
//            boost::asio::write(socket, buffer(buf, len));
//        }
//    }
//    catch (exception& e) {
//        cerr << e.what() << endl;
//    }
//}
//
//int main() {
//    try {
//        io_service io_service;
//
//        // Listen on all available interfaces on port 12345
//        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 9999));
//
//        for (;;) {
//            // Accept a new connection
//            tcp::socket socket(io_service);
//            acceptor.accept(socket);
//
//            // Start a new thread to handle the connection
//            thread(handle_connection, move(socket)).detach();
//        }
//    }
//    catch (exception& e) {
//        cerr << e.what() << endl;
//    }
//
//    return 0;
//}