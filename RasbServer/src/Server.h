#include "Session.h"
#include <vector>

class Server
{
public:
    Server(boost::asio::io_service& io_service, short port)
        : io_service_(io_service),
        acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
        start_accept();
        t = std::thread(&Server::broadcast, this);
        io_service.run();
    }

    ~Server()
    {
        t.join();
        for (int i = 0; i < sessions.size(); i++) {
            sessions[i]->socket().close();
        }
    }

private:
    void start_accept()
    {
        Session* new_session = new Session(io_service_);
        acceptor_.async_accept(new_session->socket(),
            boost::bind(&Server::handle_accept, this, new_session,
                boost::asio::placeholders::error));

        sessions.push_back(new_session);
        std::cout << "new client joined\n";
    }

    void broadcast()
    {
        while (true)
        {
            std::string msg = "";
            for (int i = 0; i < sessions.size() - 1; i++) {
                try
                {
                    if (sessions[i]->get_owner() == Owner::ARDUINO) {
                        std::string nextMsg(sessions[i]->get_data(), sizeof(sessions[i]->get_data()));
                        msg = nextMsg;
                    }
                    if (!msg.empty()) {
                        for (int j = 0; j < sessions.size(); j++) {
                            if (sessions[j]->get_owner() == Owner::CLIENT) {
                                write(sessions[j]->socket(), boost::asio::buffer(msg + "\n"));
                            }
                        }
                    }
                }
                catch (std::exception e)
                {
                    sessions.erase(sessions.begin() + i);
                    std::cout << "client disconnected\n";
                }
            }
        }
    }

    void handle_accept(Session* new_session, const boost::system::error_code& error)
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
    std::vector<Session*> sessions;
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    std::thread t;
};