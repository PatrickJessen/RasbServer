#include <iostream>
#include <boost/asio.hpp>
#include <map>
#include <mutex>
#include <queue>
#include <thread>

enum class Owner
{
    CLIENT = 1,
    ARDUINO,
    NONE
};

using namespace boost::asio;
using namespace boost::asio::ip;

const int BUFFER_SIZE = 1024;

struct Message {
    std::string data;
    Owner owner;
};

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(int id, tcp::socket socket)
        : id_(id), socket_(std::move(socket)) {}

    void Start() {
        DoRead();
    }

    int GetId() const { return id_; }
    const Message& GetMsg() { return message; }
    const Owner& GetOwner() { return m_Owner; }

    void SendMsg(const std::string& message) {
        auto self(shared_from_this());
        boost::asio::async_write(
            socket_, buffer(message, message.size()),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    // Handle write error
                }
            });
    }

private:
    void DoRead() {
        memset(data_, '\0', BUFFER_SIZE);
        auto self(shared_from_this());
        socket_.async_read_some(buffer(data_, BUFFER_SIZE),
            [this, self](boost::system::error_code ec,
                std::size_t length) {
                    if (!ec) {
                        message.data = "";
                        message.data = std::string(data_, length);
                        std::cout << message.data << " " << length << "\n";
                        if (firstConnect) {
                            m_Owner = (Owner)atoi(message.data.c_str());
                            if (m_Owner == Owner::CLIENT) {
                                std::cout << "Client joined the server\n";
                                message.owner = Owner::ARDUINO;
                            }
                            else if (m_Owner == Owner::ARDUINO) {
                                std::cout << "Arduino joined the server\n";
                                message.owner = Owner::CLIENT;
                            }
                            firstConnect = false;
                        }
                        DoRead();
                    }
                    else {
                        // Handle read error
                    }
            });
    }
    Owner m_Owner = Owner::NONE;
    int id_;
    tcp::socket socket_;
    char data_[BUFFER_SIZE];
    Message message;
    bool firstConnect = true;
};