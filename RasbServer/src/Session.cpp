#include "Session.h"

Session::Session(int id, tcp::socket socket)
    : m_Id(id), m_Socket(std::move(socket))
{

}

void Session::Start() {
    DoRead();
}

void Session::ResetMsg()
{
    m_Message.data = "";
}

/// <summary>
/// Sends message async to socket
/// </summary>
/// <param name="message">Data we get from another session</param>
void Session::SendMsg(const std::string& message) {
    try
    {
        auto self(shared_from_this());
        boost::asio::async_write(
            m_Socket, buffer(message, message.size()),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    // Handle write error
                }
            });
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
    }
}

const bool& Session::IsConnected()
{
    while (true)
    {
        try
        {
            streambuf buf;
            read_until(m_Socket, buf, "");
        }
        catch (std::exception e)
        {
            m_IsConnected = false;
        }
    }
}

/// <summary>
/// Recursive function that reads async from socket and saves the data into message object
/// </summary>
void Session::DoRead() {
    try
    {
        auto self(shared_from_this());
        m_Socket.async_read_some(buffer(m_Data, BUFFER_SIZE),
            [this, self](boost::system::error_code ec,
                std::size_t length) {
                    if (!ec) {
                        if (m_Data != "")
                        {
                            m_Message.data = std::string(m_Data, length);
                            std::cout << m_Message.data << std::endl;

                            // If its the first time a socket connects we check if its a client or arduino
                            if (m_FirstConnect) {
                                m_Owner = (Owner)atoi(m_Message.data.c_str());
                                if (m_Owner == Owner::CLIENT) {
                                    std::cout << "Client joined the server\n";
                                    m_Message.owner = Owner::ARDUINO;
                                }
                                else if (m_Owner == Owner::ARDUINO) {
                                    std::cout << "Arduino joined the server\n";
                                    m_Message.owner = Owner::CLIENT;
                                }
                                m_FirstConnect = false;
                                m_IsConnected = true;
                                // Start thread to check if socket is still alive
                                std::thread(&Session::IsConnected, this).detach();
                            }
                        }
                        DoRead();
                    }
                    else {
                        // Handle read error
                    }
            });
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
    }
}