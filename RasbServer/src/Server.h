#include "Connection_Handler.h"
#include <vector>

class server
{
public:
    server(boost::asio::io_service& io_service, short port)
        : io_service_(io_service),
        acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
        start_accept();
        t = std::thread(&server::broadcast, this);
        io_service.run();
    }

    ~server()
    {
        t.join();
        for (int i = 0; i < sessions.size(); i++) {
            sessions[i]->socket().close();
        }
    }

private:
    void start_accept()
    {
        session* new_session = new session(io_service_);
        acceptor_.async_accept(new_session->socket(),
            boost::bind(&server::handle_accept, this, new_session,
                boost::asio::placeholders::error));

        sessions.push_back(new_session);
        std::cout << "new client joined\n";
    }

    void broadcast()
    {
        while (true)
        {
            std::string data = "";
            std::getline(std::cin, data);
            for (int i = 0; i < sessions.size() - 1; i++) {
                try
                {
                    write(sessions[i]->socket(), boost::asio::buffer(data + "\n"));
                }
                catch (std::exception e)
                {
                    sessions.erase(sessions.begin() + i);
                    std::cout << "client disconnected\n";
                }
            }
        }
    }

    void handle_accept(session* new_session,
        const boost::system::error_code& error)
    {
        if (!error)
        {
            new_session->start();
        }
        else
        {
            delete new_session;
        }

        start_accept();
    }
    std::vector<session*> sessions;
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    std::thread t;
};