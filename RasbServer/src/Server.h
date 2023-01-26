#include "Connection_Handler.h"
#include <vector>


class Server
{
public:
    //constructor for accepting connection from client
    Server()
    : acceptor_(io_service, tcp::endpoint(tcp::v4(), 1234))
    {
        start_accept();
        std::thread t(&Server::write, this);
        io_service.run();
        write();
    }
    void handle_accept(Connection_Handler::pointer connection, const boost::system::error_code& err)
    {
        if (!err) {
            connection->write();
        }
        start_accept();
    }
    void write()
    {
        while (true)
        {
            for (int i = 0; i < sockets.size(); i++) {
                if (!sockets[i]->is_connected()) {
                    sockets.erase(sockets.begin() + i);
                }
                sockets[i]->test();
            }
            Sleep(1000);
        }
    }
private:
    void start_accept()
    {
        // socket
        Connection_Handler::pointer connection = Connection_Handler::create(io_service);

        // asynchronous accept operation and wait for a new connection.
        acceptor_.async_accept(connection->socket(), boost::bind(&Server::handle_accept, this, connection,
        boost::asio::placeholders::error));
        sockets.push_back(connection.get());
        //std::cout << "New user joined id: " << connection->socket().remote_endpoint().address().to_string().c_str() << "\n";
    }
    boost::asio::io_service io_service; 
    tcp::acceptor acceptor_;
    std::vector<Connection_Handler*> sockets;
    int userId = 0;
};