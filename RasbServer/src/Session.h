#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

enum class Owner
{
    CLIENT = 1,
    ARDUINO,
    NONE
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

    void start(std::vector<Session*>& sessions)
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&Session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));

        // Get Owner
        if (firstConnect) {
            std::string getOwner = &data_[0];
            try
            {
                m_Owner = (Owner)atoi(getOwner.c_str());
                std::cout << "Current session Owner: " << getOwner.c_str() << "\n";
                firstConnect = false;
                sessions.push_back(this);
            }
            catch (std::exception e)
            {
                
            }
        }
    }
    const std::vector<std::string> get_messageQueue() { return messages; }
    Owner get_owner() const 
    { 
        return m_Owner; 
    }
    char* get_data() { return data_; }
    void clear_data() { memset(data_, '\0', sizeof(data_)); }
private:
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
    {
        if (!error)
        {
            boost::asio::async_read(socket_,
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
    Owner m_Owner = Owner::NONE;
    enum { max_length = 1024 };
    char data_[max_length];
    tcp::socket socket_;
    bool firstConnect = true;
    std::vector<std::string> messages;
};