#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <deque>

using namespace boost::asio;
using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, std::vector<std::shared_ptr<Session>>& clients)
        : socket_(std::move(socket)), clients_(clients)
    {
    }

    void start() {
        do_read();
    }

    void deliver(const std::string& msg) {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress) {
            do_write();
        }
    }

    void write()
    {
        while (true)
        {
            std::string msg = "";
            std::getline(std::cin, msg);
            strcpy_s(read_msg_, msg.c_str());
            std::string f(read_msg_, 1024);
            for (auto& client : clients_) {
                client->deliver(f);
            }
        }
    }
private:
    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(
            buffer(read_msg_),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string msg(read_msg_, length);
                    for (auto& client : clients_) {
                        client->deliver(msg);
                    }
                    do_read();
                }
            });
    }


    void do_write() {
        auto self(shared_from_this());
        boost::asio::async_write(
            socket_,
            boost::asio::buffer(write_msgs_.front()),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    write_msgs_.pop_front();
                    if (!write_msgs_.empty()) {
                        do_write();
                    }
                }
            });
    }

    tcp::socket socket_;
    std::vector<std::shared_ptr<Session>>& clients_;
    std::deque<std::string> write_msgs_;
    char read_msg_[1024];
};
