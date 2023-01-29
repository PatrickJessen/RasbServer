#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ip;

enum class Owner
{
    CLIENT,
    ARDUINO
};

std::string getData(tcp::socket& socket)
{
    streambuf buf;
    read_until(socket, buf, "\n");
    std::string data = buffer_cast<const char*>(buf.data());
    return data;
}

void sendData(tcp::socket& socket, const std::string& message)
{
    write(socket, buffer(message + "\n"));
}

int main(int argc, char* argv[])
{
    bool connected = false;
    io_service io_service;
    // socket creation
    ip::tcp::socket client_socket(io_service);

    client_socket.connect(tcp::endpoint(address::from_string("127.0.0.1"), 9999));
    connected = true;
    sendData(client_socket, "" + (int)Owner::ARDUINO);
    std::string response;

    while (true) {
        try
        {
            std::string reply = "";
            std::cin >> reply;
            sendData(client_socket, reply);

            std::cout << "Sent message: " << reply << std::endl;
        }
        catch (std::exception e)
        {
            std::cout << "server is offline\n";
            client_socket.close();
            io_service.stop();
            connected = false;
            while (!connected)
            {
                try
                {

                    client_socket.connect(tcp::endpoint(address::from_string("127.0.0.1"), 9999));
                    std::cout << "Reconnected\n";
                    connected = true;
                }
                catch (std::exception e)
                {

                }
            }
        }
    }
    return 0;
}