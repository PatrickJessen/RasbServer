#include "Session.h"

class Server {
public:
    Server(boost::asio::io_service& service, unsigned short port);

public:
    void Stop();
    void AddMessage(const Message& message);

private:
    void DoAccept();
    void MessageThread();
    bool CheckDisconnections(const int& i);

private:
    boost::asio::io_service& service;
    tcp::acceptor acceptor;
    int next_id;
    std::vector<std::shared_ptr<Session>> sessions;
    std::queue<Message> message_queue;
    std::mutex session_mutex;
    std::mutex message_queue_mutex;
    std::mutex message_mutex;
    std::thread message_thread;
    bool running;
};