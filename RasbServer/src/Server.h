#include "Session.h"
#include <vector>

struct Message
{
    std::string message;
    Owner to;
    Message(std::string msg, Owner to)
        : message(msg), to(to) {}
};

class Server
{
public:
    Server(boost::asio::io_service& io_service, short port)
        : io_service_(io_service),
        acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
        start_accept();
        arduinoReadThread = std::thread(&Server::read_from, this, Owner::ARDUINO);
        arduinoSendThread = std::thread(&Server::send_to, this, Owner::ARDUINO);
        clientSendThread = std::thread(&Server::send_to, this, Owner::CLIENT);
        clientReadThread = std::thread(&Server::read_from, this, Owner::CLIENT);
        io_service.run();
    }

    ~Server()
    {
        arduinoReadThread.join();
        clientSendThread.join();
        clientReadThread.join();
        arduinoSendThread.join();
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
    }

    void read_from(const Owner& owner)
    {
        while (true)
        {
            for (int i = 0; i < sessions.size(); i++) {
                try
                {
                    if (sessions[i]->get_owner() != owner) {
                        std::string nextMsg(sessions[i]->get_data(), sizeof(sessions[i]->get_data()));
                        if (nextMsg[0] != '\0') {
                            if (owner == Owner::ARDUINO) {
                                messageQueue.push_back(Message(nextMsg, Owner::CLIENT));
                            }
                            else if (owner == Owner::CLIENT) {
                                messageQueue.push_back(Message(nextMsg, Owner::ARDUINO));
                            }
                            sessions[i]->clear_data();
                        }
                    }
                }
                catch (std::exception e)
                {
                    sessions.erase(sessions.begin() + i);
                    std::cout << "client disconnected\n";
                }
            }
            Sleep(500);
        }
    }

    void send_to(const Owner& owner)
    {
        while (true)
        {
            while (!messageQueue.empty()) {
                for (int i = 0; i < sessions.size(); i++) {
                    try
                    {
                        if (messageQueue[0].to == sessions[i]->get_owner()) {
                            if (sessions[i]->get_owner() != owner) {
                                write(sessions[i]->socket(), boost::asio::buffer(messageQueue[0].message + "\n"));

                            }
                        }
                        if (i == sessions.size()) {
                            messageQueue.pop_back();
                        }
                    }
                    catch (std::exception e)
                    {
                        sessions.erase(sessions.begin() + i);
                        std::cout << "client disconnected\n";
                    }
                }
            }
            Sleep(500);
        }
    }

    void handle_accept(Session* new_session, const boost::system::error_code& error)
    {
        if (!error)
        {
            new_session->start(std::ref(sessions));
        }
        else
        {
            delete new_session;
        }

        start_accept();
    }

    void wait_for_assignment(Session* new_session)
    {
        while (new_session->get_owner() == Owner::NONE)
        {
            std::cout << "Waiting for session to get assigned\n";
            Sleep(1000);
        }
    }

    std::vector<Session*> sessions;
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    std::thread arduinoReadThread;
    std::thread arduinoSendThread;
    std::thread clientSendThread;
    std::thread clientReadThread;
    std::vector<std::string> msgQueueClient;
    std::vector<std::string> msgQueueArduino;
    std::vector<Message> messageQueue;
};