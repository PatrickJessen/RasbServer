#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace boost::asio;
using ip::tcp;

class Connection_Handler : public boost::enable_shared_from_this<Connection_Handler>
{

public:
  typedef boost::shared_ptr<Connection_Handler> pointer;
  Connection_Handler(boost::asio::io_service& io_service): sock(io_service){}
// creating the pointer
  static pointer create(boost::asio::io_service& io_service)
  {
      return pointer(new Connection_Handler(io_service));
  }
//socket creation
  tcp::socket& socket()
  {
    return sock;
  }
  void test()
  {
      std::string test = "test";
      boost::asio::write(sock, boost::asio::buffer(test, max_length));
  }

  void write()
  {
      sock.async_read_some(boost::asio::buffer(data, max_length), boost::bind(&Connection_Handler::handle_read,
        shared_from_this(),
            boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));

      sock.async_write_some(boost::asio::buffer(message, max_length), boost::bind(&Connection_Handler::handle_write,
        shared_from_this(),
            boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
  }

  void handle_read(const boost::system::error_code& err, size_t bytes_transferred)
  {
      if (!err) {
          std::cout << data << std::endl;
      }
      else {
          std::cerr << "error: " << err.message() << std::endl;
          sock.close();
          connected = false;
      }
  }

  void handle_write(const boost::system::error_code& err, size_t bytes_transferred)
  {
    if (!err) {
       std::cout << "Server sent Hello message!"<< std::endl;
    } else {
       std::cerr << "error: " << err.message() << std::endl;
       sock.close();
       connected = false;
    }
  }
  const bool& is_connected() { return connected; }
private:
    tcp::socket sock;
    std::string message="Hello From Server!";
    enum { max_length = 1024 };
    char data[max_length];
    bool connected = false;
};