#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ip;

std::string getData(tcp::socket& socket)
{
    streambuf buf;
    read_until(socket, buf, "\n");
    std::string data = buffer_cast<const char*>(buf.data());
    return data;
}

void sendData(tcp::socket& socket, const std::string& message)
{
    write(socket,
        buffer(message + "\n"));
}

int main(int argc, char* argv[])
{
    io_service io_service;
    // socket creation
    ip::tcp::socket client_socket(io_service);

    client_socket
        .connect(
            tcp::endpoint(
                address::from_string("127.0.0.1"),
                9999));

    // Getting username from user
    std::string response;
    sendData(client_socket, "test\n");

    // Infinite loop for chit-chat
    while (true) {
        // Fetching response
        response = getData(client_socket);

        // Popping last character "\n"
        response.pop_back();

        std::cout << "Server: " << response << std::endl;
    }
    return 0;
}