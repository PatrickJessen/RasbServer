#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ip;

enum class Owner
{
    CLIENT = 1,
    ARDUINO
};

std::string message = "";

void getData(tcp::socket& socket)
{
    while (true)
    {
        streambuf buf;
        read_until(socket, buf, "\n");
        std::string data = buffer_cast<const char*>(buf.data());
        message = data;
        std::cout << "Received message: " << message << std::endl;
    }
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

    sendData(client_socket, std::to_string((int)Owner::CLIENT));
    std::string response;

    std::thread(&getData, std::ref(client_socket)).detach();

    while (true) {
        try
        {
            int randNum = rand() % 5;
            std::string msg = std::to_string(randNum);
            sendData(client_socket, msg);
            std::cout << "Send message: " << msg << "\n";
            
            Sleep(3000);
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
        client_socket.close();
    }
    return 0;
}