//
// Created by kadius on 03.08.23.
//

#ifndef MINI_TCP_CLIENT_MINI_CLIENT_HPP
#define MINI_TCP_CLIENT_MINI_CLIENT_HPP

#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>

using namespace boost;

using asio::ip::tcp;
static constexpr size_t CRISP_SIZE = 10;

class Client {
public:
    Client(asio::io_context& io_context): socket_(io_context){
    }

    ~Client() {
        socket_.shutdown(boost::asio::socket_base::shutdown_both);
        socket_.close();
    }

    void connect(const std::string& ip, uint16_t port) {
        socket_.connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
    }

    template<typename B>
    void send(/*std::array<uint8_t, CRISP_SIZE>*/B& buf) {
        asio::async_write(socket_, asio::buffer(buf), [](std::error_code ec, size_t bytes_sent){});
    }

private:
    tcp::socket socket_;
};



#endif //MINI_TCP_CLIENT_MINI_CLIENT_HPP
