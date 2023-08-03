#include "mini_client.hpp"

int main() {

    asio::io_context io_context;
    Client client(io_context);
//    io_context.run();

    client.connect("127.0.0.1", 6666);
    std::array<uint8_t, CRISP_SIZE> buf = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa};
    client.send(buf);

    return 0;
}
