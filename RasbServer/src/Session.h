#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

enum class State
{
    CLIENT_CONNECTED,
    CLIENT_SENT_MESSAGE,
    CLIENT_RECEIVE_MESSAGE,
    CLIENT_DISCONNECT
};

enum class Owner
{
    CLIENT,
    ARDUINO
};

using boost::asio::ip::tcp;

class Session
{
public:
    Session(boost::asio::io_service& io_service)
        : socket_(io_service)
    {
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&Session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        // Get Owner
        if (firstConnect) {
            std::string getOwner(data_, sizeof(data_));
            m_Owner = (Owner)atoi(getOwner.c_str());
            std::cout << "Current session Owner: " << getOwner.c_str() << "\n";
            firstConnect = false;
        }
    }
    Owner get_owner() const { return m_Owner; }
    char* get_data() { return data_; }
private:
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
    {
        if (!error)
        {
            boost::asio::async_write(socket_,
                boost::asio::buffer(data_, bytes_transferred),
                boost::bind(&Session::handle_write, this,
                    boost::asio::placeholders::error));
        }
        else
        {
            delete this;
        }
    }

    void handle_write(const boost::system::error_code& error)
    {
        if (!error)
        {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                boost::bind(&Session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            delete this;
        }
    }
private:
    Owner m_Owner = Owner::CLIENT;
    enum { max_length = 1024 };
    char data_[max_length];
    tcp::socket socket_;
    bool firstConnect = true;
};