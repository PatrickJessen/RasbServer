#include "Session.h"

class Server {
public:
    Server(boost::asio::io_service& service, unsigned short port)
        : service_(service),
        acceptor_(service, tcp::endpoint(tcp::v4(), port)),
        next_id_(1),
        running_(true) {
        DoAccept();
        message_thread_ = std::thread(&Server::MessageThread, this);
    }

    void Stop() {
        running_ = false;
        message_thread_.join();
    }

    void AddMessage(const Message& message) {
        std::lock_guard<std::mutex> lock(message_queue_mutex_);
        message_queue_.push(message);
    }

private:
    void DoAccept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    int id = next_id_;
                    ++next_id_;
                    auto session = std::make_shared<Session>(id, std::move(socket));
                    session->Start();
                    {
                        std::lock_guard<std::mutex> lock(session_map_mutex_);
                        sessions.push_back(session);
                    }
                }
                DoAccept();
            });
    }

    void MessageThread() {
        while (running_) {
            for (int i = 0; i < sessions.size(); i++) {
                Message message;
                AddMessage(sessions[i]->GetMsg());
                {
                    std::unique_lock<std::mutex> lock(message_queue_mutex_);
                    if (message_queue_.empty()) {
                        lock.unlock();
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        continue;
                    }
                    message = message_queue_.front();
                    //std::cout << message.data << "\n";
                    message_queue_.pop();
                    std::unique_lock<std::mutex> message_lock(session_map_mutex_);
                    for (int j = 0; j < sessions.size(); j++) {
                        if (sessions[j]->GetOwner() == message.owner) {
                            sessions[j]->SendMsg(message.data);
                            std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        }
                    }
                }
            }
        }
    }

    boost::asio::io_service& service_;
    tcp::acceptor acceptor_;
    int next_id_;
    std::vector<std::shared_ptr<Session>> sessions;
    std::mutex session_map_mutex_;
    std::queue<Message> message_queue_;
    std::mutex message_queue_mutex_;
    std::mutex message_mutex;
    std::thread message_thread_;
    bool running_;
};