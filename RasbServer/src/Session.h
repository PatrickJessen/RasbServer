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

struct Message {
    std::string data;
    Owner owner;
};

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(int id, tcp::socket socket);

public:
    void Start();
    void ResetMsg();
    void SendMsg(const std::string& message);
    const bool& IsConnected();
    void DoRead();
    
    // Getters
    int GetId() const { return m_Id; }
    const Message& GetMsg() { return m_Message; }
    const Owner& GetOwner() { return m_Owner; }
    const bool& GetIsConnected() { return m_IsConnected; }

    // Setters
    void SetIsConnected(const bool& value) { m_IsConnected = value; }


private:
    enum { BUFFER_SIZE = 1024 };
    Owner m_Owner = Owner::NONE;
    int m_Id;
    tcp::socket m_Socket;
    char m_Data[BUFFER_SIZE];
    Message m_Message;
    bool m_FirstConnect = true;
    bool m_IsConnected = false;
};