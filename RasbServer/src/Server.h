#include "Connection_Handler.h"

class Server {
public:
    Server(io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();

        io_context.run();
        t.join();
    }

    void broadcast()
    {
        
    }
private:
    void do_accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    clients_.push_back(std::make_shared<Session>(std::move(socket), clients_));
                    clients_[0]->start();
                    t = std::thread(&Session::write, clients_[0]);
                }

                do_accept();
            });
    }


    tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<Session>> clients_;
    std::thread t;
};