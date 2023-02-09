#include "Server.h"

Server::Server(boost::asio::io_service& service, unsigned short port)
    : service(service), acceptor(service, tcp::endpoint(tcp::v4(), port)), next_id(1), running(true)
{
    DoAccept();
    message_thread = std::thread(&Server::MessageThread, this);
}

void Server::Stop() {
    running = false;
    message_thread.join();
}

void Server::AddMessage(const Message& message) {
    try
    {
        // Have a mutex to avoid message data being edited while being sent, otherwise we will get thrown a "cannot dereference string error"
        std::lock_guard<std::mutex> lock(message_queue_mutex);
        message_queue.push(message);
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
    }
}

/// <summary>
/// Recursive function that waits and accepts new sessions
/// Adds new in sessions to a vector
/// </summary>
void Server::DoAccept() {
    try
    {

        acceptor.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    int id = next_id;
                    ++next_id;
                    auto session = std::make_shared<Session>(id, std::move(socket));
                    session->Start();
                    {
                        std::lock_guard<std::mutex> lock(session_mutex);
                        session->SetIsConnected(true);
                        sessions.push_back(session);
                    }
                }
        DoAccept();
            });
    }
    catch (std::exception e)
    {
        std::cout << e.what() << "\n";
    }
}

void Server::MessageThread() {
    while (running) {
        std::lock_guard<std::mutex> message_lock(session_mutex);
        for (int i = 0; i < sessions.size(); i++) {
            Message message;
            // Check if socket is still available
            if (CheckDisconnections(i)) {
                continue;
            }
            {
                try
                {
                    // If no data is available continue the loop
                    if (sessions[i]->GetMsg().data == "") {
                        if (i == 0)
                            break;
                        continue;
                    }

                    // Add message object to queue
                    AddMessage(sessions[i]->GetMsg());
                    std::unique_lock<std::mutex> lock(message_queue_mutex);
                    if (message_queue.empty()) {
                        lock.unlock();
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        continue;
                    }
                    // Save next message
                    message = message_queue.front();
                    for (int j = 0; j < sessions.size(); j++) {
                        // Check who the message should be sent to
                        if (sessions[j]->GetOwner() == message.owner) {
                            try
                            {
                                // Send and reset the message
                                sessions[j]->SendMsg(message.data);
                                sessions[i]->ResetMsg();
                                sessions[j]->ResetMsg();
                                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                            }
                            catch (std::exception e)
                            {
                                std::cout << e.what() << std::endl;
                                sessions.erase(sessions.begin() + j);
                            }
                        }
                    }
                    // Remove message that was just sent
                    message_queue.pop();
                }
                catch (std::exception e)
                {
                    std::cout << e.what() << std::endl;
                }
            }
        }
    }
}

bool Server::CheckDisconnections(const int& i)
{
    try
    {
        if (sessions[i] != nullptr) {
            if (!sessions[i]->GetIsConnected()) {
                if (sessions[i]->GetOwner() == Owner::ARDUINO) {
                    std::cout << "Arduino disconnected\n";
                }
                else if (sessions[i]->GetOwner() == Owner::CLIENT) {
                    std::cout << "Client disconnected\n";
                }
                sessions.erase(sessions.begin() + i);
                return true;
            }
        }
        else {
            std::cout << "Error: " << i << "\n";
        }
        return false;
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
        sessions.erase(sessions.begin() + i);
        return true;
    }
    return false;
}
