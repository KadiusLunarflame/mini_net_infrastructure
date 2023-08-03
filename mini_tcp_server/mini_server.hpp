//
// Created by kadius on 03.08.23.
//

#ifndef MINI_TCP_SERVER_MINI_SERVER_HPP
#define MINI_TCP_SERVER_MINI_SERVER_HPP


#include <iostream>
#include <coroutine>
#include <boost/asio.hpp>
#include <future>

using namespace boost;
using asio::ip::tcp;

namespace await {

    struct async_read_awaiter {

        bool await_ready() {
            return false;
        }

        void await_suspend(std::coroutine_handle<> handle) {
            asio::async_read(socket_, buffer_, [this, handle](auto ec, auto bytes_read) mutable {
                ec_ = ec;
                bytes_read_ = bytes_read;
                handle.resume();
            });
        }

        size_t await_resume() {
            if (ec_) {
                throw std::system_error(ec_);
            }
            return bytes_read_;
        }

        tcp::socket &socket_;
        asio::mutable_buffer buffer_;
        std::error_code ec_;
        size_t bytes_read_;
    };

    auto async_read(tcp::socket &socket, asio::mutable_buffer buffer) {
        return async_read_awaiter{socket, buffer};
    }

    struct VoidPromise {
        void get_return_object() {
            return;  // Nop
        }

        std::suspend_never initial_suspend() noexcept {
            return {};
        }

        std::suspend_never final_suspend() noexcept {
            return {};
        }

        void set_exception(std::exception_ptr /*e*/) {
            std::abort();  // Not supported
        }

        void unhandled_exception() {
            std::abort();  // Not supported
        }

        void return_void() {
            // Nop
        }
    };

}//await

//coroutine returns void and needs this specialization for compiler to know what correct promise_type is
template <typename ... Args>
struct std::coroutine_traits<void, Args...> {
    using promise_type = await::VoidPromise;
};

//coroutine
//future?
void session(tcp::socket socket) {
    static constexpr size_t CRISP_SIZE = 10;
//    uint8_t crisp_header[CRISP_SIZE];
    std::array<uint8_t, CRISP_SIZE> crisp_header;
    //ignore exceptions
    //can't ignore exceptions, because the server would crash if a client dies
    try {
        for(;;) {
            auto bytes_read = co_await await::async_read(socket, asio::buffer(crisp_header));
            for(int i{}; i < 10; ++i) {
                std::cout << std::hex << (int)crisp_header[i];
            }
            std::cout << std::endl;
        }
    } catch (...) {
        // continue
    }
    //just loop
}

class Server {
public:
    Server(asio::io_context& io_context, in_port_t port): acceptor_(io_context, asio::ip::tcp::endpoint(tcp::v4(), port)) {

    }

    void accept_loop() {
        acceptor_.async_accept([this](std::error_code ec, tcp::socket client_socket){
            if(!ec) {
                std::cout << "a client connected" << std::endl;
                session(std::move(client_socket));
            }
            accept_loop();//not a recursion
        });
    }

    asio::ip::tcp::acceptor acceptor_;
};

//no need for a thread_pool
void Run(asio::io_context& io_context, size_t threads) {
    std::vector<std::thread> workers;

    for (size_t i = 1; i < threads; ++i) {
        workers.emplace_back([&io_context]() {
            io_context.run();
        });
    }
    io_context.run();

    for (auto& t : workers) {
        t.join();
    }
}


void serve(uint16_t port) {
    asio::io_context io_context;

    Server s(io_context, port);
    s.accept_loop();

    Run(io_context, 4);
}


#endif //MINI_TCP_SERVER_MINI_SERVER_HPP
